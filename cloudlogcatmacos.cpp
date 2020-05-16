#include "cloudlogcatmacos.h"
#include "ui_cloudlogcatmacos.h"

CloudLogCATMacOS::CloudLogCATMacOS(QWidget *parent)
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
}

CloudLogCATMacOS::~CloudLogCATMacOS()
{
    killTimer(timerId);
    delete ui;
}

void CloudLogCATMacOS::on_pushButton_clicked()
{

}

void CloudLogCATMacOS::timerEvent(QTimerEvent *event)
{
    qDebug() << "Update...";
    getMode();
    getFrequency();
}

QString CloudLogCATMacOS::parseXML(QString xml)
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
}

void CloudLogCATMacOS::callbackFrequency(QNetworkReply *rep)
{
    double frequency = parseXML(QString(rep->readAll())).toDouble()/1000.0/1000.0;
    ui->lcdNumber->display(frequency);
    qDebug() << frequency;
}

void CloudLogCATMacOS::callbackMode(QNetworkReply *rep)
{
    QString mode = parseXML(QString(rep->readAll()));
    ui->mode->setText(mode);
    qDebug() << mode;
}

void CloudLogCATMacOS::getFromFLRig(QString command, QNetworkAccessManager *manager)
{
    QByteArray data;
    data.append("<?xml version=\"1.0\"?><methodCall><methodName>"
                + command
                + "</methodName><params></params></methodCall>");

    QUrl url = QUrl("http://"+ui->FLDigiHostname->text());
    url.setPort(ui->FLDigiPort->text().toInt());

    manager->post(QNetworkRequest(url), data);
}

void CloudLogCATMacOS::getFrequency()
{
    qDebug() << "getFreq...";
    getFromFLRig("rig.get_vfo", frequencyManager);
}

void CloudLogCATMacOS::getMode()
{
    qDebug() << "getMode...";
    getFromFLRig("rig.get_mode", modeManager);
}
