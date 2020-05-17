#include "cloudlogcatqt.h"
#include "ui_cloudlogcatqt.h"

CloudLogCATQt::CloudLogCATQt(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CloudLogCATQt)
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
    connect(cloudLogManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(callbackCloudLog(QNetworkReply*))
    );

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

    QString str = QString("")
                + "{"
                + "\"key\" : \"" + ui->cloudLogKey->text() + "\","
                + "\"radio\" : \"CloudLogCATQt\" ,"
                + "\"frequency\" : \"" + frequency + "\","
                + "\"mode\" : \"" + mode + "\","
                + "\"timestamp\" : \"" + currentTime.toString("yyyy/MM/dd hh:mm") + "\""
                + "}";
    data = str.toUtf8();

    QUrl url = QUrl(ui->cloudLogUrl->text());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
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
    ui->FLRigHostname->setText(settings.value("FLRigHostname", "localhost").toString());
    ui->FLRigPort->setText(settings.value("FLRigPort", "12345").toString());
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

void CloudLogCATQt::callbackCloudLog(QNetworkReply *rep)
{
    qDebug () << QString(rep->readAll());
}

void CloudLogCATQt::getFromFLRig(QString command, QNetworkAccessManager *manager)
{
    QByteArray data;
    data.append("<?xml version=\"1.0\"?><methodCall><methodName>"
                + command
                + "</methodName><params></params></methodCall>");

    QUrl url = QUrl("http://"+ui->FLRigHostname->text());
    url.setPort(ui->FLRigPort->text().toInt());

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
    settings.setValue("FLRigHostname", ui->FLRigHostname->text());
    settings.setValue("FLRigPort",     ui->FLRigPort->text());
}
