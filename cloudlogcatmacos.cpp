#include "cloudlogcatmacos.h"
#include "ui_cloudlogcatmacos.h"

CloudLogCATQt::CloudLogCATQt(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CloudLogCATMacOS)
{
    ui->setupUi(this);
    timerId = startTimer(1000);

    // Setup HTTP Request Managers:
    frequencyManager = new QNetworkAccessManager(this);
    connect(frequencyManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(callbackFrequency(QNetworkReply*))
    );

    modeManager = new QNetworkAccessManager(this);
    connect(modeManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(callbackMode(QNetworkReply*))
    );

    cloudLogManager = new QNetworkAccessManager(this);

    // Setup Settings File:
    settingsFile = QApplication::applicationDirPath() + "/settings.ini"; 
    loadSettings();
}

CloudLogCATQt::~CloudLogCATQt()
{
    killTimer(timerId);
    delete ui;
}

void CloudLogCATQt::timerEvent(QTimerEvent *event)
{
    getMode();
    getFrequency();
}

QString CloudLogCATQt::parseXML(QString xml)
{
    /*
    * <?xml version="1.0"?>
    * <methodResponse>
    *     <params>
    *       <param>
    *         <value>XXX</value>
    *       </param>
    *     </params>
    * </methodResponse>
    */

    QXmlStreamReader reader(xml);
    while(!reader.atEnd() && !reader.hasError()) {
        if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "value") {
            return reader.readElementText();
        }
    }

    return QString("ERROR");
}

void CloudLogCATQt::uploadToCloudLog()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QByteArray data;

    QUrlQuery query;
    query.addQueryItem("key", ui->cloudLogKey->text());
    query.addQueryItem("radio","CloudLogCATMacOS"); // TODO
    query.addQueryItem("frequency", frequency);
    query.addQueryItem("mode", mode);
    query.addQueryItem("timestamp", currentTime.toString("yyyy/MM/dd hh:mm"));

    data = query.toString(QUrl::FullyEncoded).toUtf8();

    QUrl url = QUrl(ui->cloudLogUrl->text());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

    cloudLogManager->post(request, data);

    qDebug() << "Update Cloud Log: " << data;
    ui->statusbar->showMessage("Cloudlog Updated: " + currentTime.toString("yyyy/MM/dd hh:mm:ss"));
}

void CloudLogCATQt::loadSettings()
{
    qDebug() << "LOAD";
    QSettings settings(settingsFile, QSettings::NativeFormat);

    ui->cloudLogUrl->setText(settings.value("cloudLogUrl","").toString());
    ui->cloudLogKey->setText(settings.value("cloudLogKey","").toString());
    ui->FLDigiHostname->setText(settings.value("FLRigHostname", "localhost").toString());
    ui->FLDigiPort->setText(settings.value("FLRigPort", "12345").toString());
}

void CloudLogCATQt::callbackFrequency(QNetworkReply *rep)
{
    QString f = parseXML(QString(rep->readAll()));
    if(f != frequency) { // Update UI and Cloudlog
        frequency = f;
        ui->lcdNumber->display(frequency.toDouble()/1000.0/1000.0);
        qDebug() << frequency;
        uploadToCloudLog();
    }
}

void CloudLogCATQt::callbackMode(QNetworkReply *rep)
{
    QString m = parseXML(QString(rep->readAll()));
    if(m != mode) { // Update UI and Cloudlog
        mode = m;
        ui->mode->setText(mode);
        qDebug() << mode;
        uploadToCloudLog();
    }
}

void CloudLogCATQt::getFromFLRig(QString command, QNetworkAccessManager *manager)
{
    QByteArray data;
    data.append("<?xml version=\"1.0\"?><methodCall><methodName>"
                + command
                + "</methodName><params></params></methodCall>");

    QUrl url = QUrl("http://"+ui->FLDigiHostname->text());
    url.setPort(ui->FLDigiPort->text().toInt());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    manager->post(request, data);
}

void CloudLogCATQt::getFrequency()
{
    getFromFLRig("rig.get_vfo", frequencyManager);
}

void CloudLogCATQt::getMode()
{
    getFromFLRig("rig.get_mode", modeManager);
}

void CloudLogCATQt::on_save_clicked()
{
    qDebug() << "SAVE";
    QSettings settings(settingsFile, QSettings::NativeFormat);

    settings.setValue("cloudLogUrl",   ui->cloudLogUrl->text());
    settings.setValue("cloudLogKey",   ui->cloudLogKey->text());
    settings.setValue("FLRigHostname", ui->FLDigiHostname->text());
    settings.setValue("FLRigPort",     ui->FLDigiPort->text());
}
