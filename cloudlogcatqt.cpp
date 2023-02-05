/*
 * Copyright (c) 2020, Matthias Jung (DL9MJ)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

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

    powerManager = new QNetworkAccessManager(this);
    connect(powerManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(callbackPower(QNetworkReply*))
    );

    cloudLogManager = new QNetworkAccessManager(this);
    connect(cloudLogManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(callbackCloudLog(QNetworkReply*))
    );

    QObject::connect(ui->propMode,
		    SIGNAL(currentTextChanged(QString)),
		    this,
		    SLOT(callbackPropMode())
    );

    QObject::connect(ui->satellite,
		    SIGNAL(currentTextChanged(QString)),
		    this,
		    SLOT(callbackSatellite())
    );

    // Setup prop modes
    propModes = (QStringList()<<""
                              <<"AS|Aircraft Scatter"
                              <<"AUE|Aurora-E"
                              <<"AUR|Aurora"
                              <<"BS|Back scatter"
                              <<"ECH|EchoLink"
                              <<"EME|Earth-Moon-Earth"
                              <<"ES|Sporadic E"
                              <<"F2|F2 Reflection"
                              <<"FAI|Field Aligned Irregularities"
                              <<"INTERNET|Internet-assisted"
                              <<"ION|Ionoscatter"
                              <<"IRL|IRLP"
                              <<"MS|Meteor scatter"
                              <<"RPT|Terrestrial or atmospheric repeater or transponder"
                              <<"RS|Rain scatter"
                              <<"SAT|Satellite"
                              <<"TEP|Trans-equatorial"
                              <<"TR|Tropospheric ducting"
    );
    ui->propMode->addItems(propModes);

    // Setup satellite names
    QFile file("sat.dat");
    QTextStream stream(&file);
    QString line;
    satNames.append("");

    if(file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
	    while (!stream.atEnd())
	    {
		    line = stream.readLine ();
		    if(!line.isNull ())
		    {
			    satNames.append(line);
		    }
	    }
	    stream.flush();
	    file.close();
    }
    ui->satellite->addItems(satNames);

    // Setup Settings File:
    settingsFile = QApplication::applicationDirPath() + "/settings.ini"; 
    loadSettings();
    txOffset = ui->TXOffset->text().toDouble();
    qDebug() << "TX Offset:" << txOffset << "Hz";
    rxOffset = ui->RXOffset->text().toDouble();
    qDebug() << "RX Offset:" << rxOffset << "Hz";
    propModeDesc = ui->propMode->currentText();
    propMode = propModeDesc.split('|');
    qDebug() << "Prop Mode:" << propMode[0];
    satelliteDesc = ui->satellite->currentText();
    satellite = satelliteDesc.split('|');
    qDebug() << "Sat:" << satellite[0];
    realTxFrequency = 0.0;
    realRxFrequency = 0.0;

    // Set Status Bar
    ui->statusbar->showMessage("(c) 2020 DL9MJ");

    // Set Placeholders
    ui->cloudLogUrl->setPlaceholderText("https://yourdomain.com/index.php/api/radio");
    ui->cloudLogKey->setPlaceholderText("cl632adab771259");
    ui->cloudLogIdentifier->setPlaceholderText("Rig Name");
    ui->FLRigHostname->setPlaceholderText("localhost");
    ui->FLRigPort->setPlaceholderText("12345");
    ui->TXOffset->setPlaceholderText("0");
    ui->RXOffset->setPlaceholderText("0");

    // Set Input Constraints
    QRegExp identifierRe("[ \\w\\d-_#]{0,50}");
    QRegExpValidator *idValidator = new QRegExpValidator(identifierRe, this);
    ui->cloudLogIdentifier->setValidator(idValidator);
    QRegExp loRe("\\d*");
    QRegExpValidator *loValidator = new QRegExpValidator(loRe, this);
    ui->TXOffset->setValidator(loValidator);
    ui->RXOffset->setValidator(loValidator);
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
    getPower();
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
        if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "i4") {
            return reader.readElementText();
        }
    }
    reader.clear();
    reader.addData(xml);
    while(!reader.atEnd() && !reader.hasError()) {
        if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "value") {
            return reader.readElementText();
        }
    }

    return QString("ERROR");
}

void CloudLogCATQt::uploadToCloudLog()
{
    // Prevent upload until variables are set
    if (ui->cloudLogKey->text() == "" || mode == "") {
        return;
    }
    QDateTime currentTime = QDateTime::currentDateTime();
    QByteArray data;

    propMode = propModeDesc.split('|');
    satellite = satelliteDesc.split('|');
    QString str = QString("")
                + "{"
                + "\"key\" : \"" + ui->cloudLogKey->text() + "\" ,"
                + "\"radio\" : \"CloudLogCATQt (" + ui->cloudLogIdentifier->text() + ")\" ,"
                + "\"prop_mode\" : \"" + propMode[0] + "\" ,"
                + "\"frequency\" : \"" + QString{ "%1" }.arg( realTxFrequency, 1, 'f', 0) + "\" ,"
                + "\"mode\" : \"" + mode + "\" ,";
    		if (propMode[0] == "SAT") {
			str += "\"sat_name\" : \"" + satellite[0] + "\" ,"
                            + "\"frequency_rx\" : \"" + QString{ "%1" }.arg( realRxFrequency, 1, 'f', 0) + "\" ,"
                            + "\"mode_rx\" : \"" + mode + "\" ,";
		}
		str += "\"power\" : \"" + QString{ "%1" }.arg(power) + "\" ,"
                + "\"timestamp\" : \"" + currentTime.toString("yyyy/MM/dd hh:mm") + "\""
                + "}";
    data = str.toUtf8();

    QUrl url = QUrl(ui->cloudLogUrl->text());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    cloudLogManager->post(request, data);

    qDebug() << "Update Cloud Log: " << data;
    ui->statusbar->showMessage("Cloudlog Updated: "
                               + currentTime.toString("yyyy/MM/dd hh:mm:ss")
                               + " | (c) 2020 DL9MJ");
}

void CloudLogCATQt::loadSettings()
{
    qDebug() << "LOAD";
    QSettings settings(settingsFile, QSettings::IniFormat);

    ui->cloudLogUrl->setText(settings.value("cloudLogUrl","").toString());
    ui->cloudLogKey->setText(settings.value("cloudLogKey","").toString());
    ui->cloudLogIdentifier->setText(settings.value("cloudLogIdentifier","").toString());
    ui->FLRigHostname->setText(settings.value("FLRigHostname", "localhost").toString());
    ui->FLRigPort->setText(settings.value("FLRigPort", "12345").toString());
    ui->TXOffset->setText(settings.value("TXOffset", "0").toString());
    ui->RXOffset->setText(settings.value("RXOffset", "0").toString());
    propModeShort = settings.value("PropMode", "").toString();
    for (int i=0; i<propModes.size(); i++) {
            QStringList temp = propModes[i].split('|');
            if (propModeShort == temp[0]) {
                 propModeDesc = propModes[i];
            }
    }
    int index = ui->propMode->findText(propModeDesc);
    ui->propMode->setCurrentIndex(index);

    satelliteShort = settings.value("Sat", "").toString();
    for (int i=0; i<satNames.size(); i++) {
            QStringList temp = satNames[i].split('|');
            if (satelliteShort == temp[0]) {
                 satelliteDesc = satNames[i];
            }
    }
    index = ui->satellite->findText(satelliteDesc);
    ui->satellite->setCurrentIndex(index);
}

void CloudLogCATQt::callbackFrequency(QNetworkReply *rep)
{
    double f = parseXML(QString(rep->readAll())).toDouble();

    if(f != frequency) { // Update UI and Cloudlog
        frequency = f;
        realTxFrequency = frequency;
        if (txOffset != 0.0) {
            realTxFrequency += txOffset;
        }
        if (rxOffset != 0.0) {
            realRxFrequency = frequency + rxOffset;
        }
        ui->lcdNumber->display(QString{ "%1" }.arg(realTxFrequency/1000.0/1000.0, 6, 'f', 5, '0' ));
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

void CloudLogCATQt::callbackPower(QNetworkReply *rep)
{
    int p = parseXML(QString(rep->readAll())).toInt();
    if(p != power) { // Update UI and Cloudlog
        power = p;
        ui->Power->setValue(power);
        uploadToCloudLog();
    }

}

void CloudLogCATQt::callbackPropMode()
{
    propModeDesc = ui->propMode->currentText();
    QStringList temp = propModeDesc.split('|');
    if (temp[0] == "SAT") {
	    ui->satellite->setEnabled(true);
    } else {
	    ui->satellite->setEnabled(false);
    }
    uploadToCloudLog();
}

void CloudLogCATQt::callbackSatellite()
{
    satelliteDesc = ui->satellite->currentText();
    uploadToCloudLog();
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

void CloudLogCATQt::getPower()
{
    getFromFLRig("rig.get_power", powerManager);
}

void CloudLogCATQt::on_save_clicked()
{
    qDebug() << "SAVE";
    QSettings settings(settingsFile, QSettings::IniFormat);

    propModeDesc = ui->propMode->currentText();
    QStringList propMode = propModeDesc.split('|');
    satelliteDesc = ui->satellite->currentText();
    QStringList satellite = satelliteDesc.split('|');
    settings.setValue("cloudLogUrl",         ui->cloudLogUrl->text());
    settings.setValue("cloudLogKey",         ui->cloudLogKey->text());
    settings.setValue("cloudLogIdentifier",  ui->cloudLogIdentifier->text());
    settings.setValue("FLRigHostname",       ui->FLRigHostname->text());
    settings.setValue("FLRigPort",           ui->FLRigPort->text());
    settings.setValue("TXOffset",            ui->TXOffset->text());
    settings.setValue("RXOffset",            ui->RXOffset->text());
    settings.setValue("Power",               ui->Power->text());
    settings.setValue("PropMode",            propMode[0]);
    settings.setValue("Sat",                 satellite[0]);
    txOffset = ui->TXOffset->text().toDouble();
    rxOffset = ui->RXOffset->text().toDouble();
}
