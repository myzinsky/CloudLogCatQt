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

#ifndef CLOUDLOGCATMACOS_H
#define CLOUDLOGCATMACOS_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QNetworkReply>
#include <QSettings>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class CloudLogCATQt; }
QT_END_NAMESPACE

class CloudLogCATQt : public QMainWindow
{
    Q_OBJECT

public:
    CloudLogCATQt(QWidget *parent = nullptr);
    ~CloudLogCATQt();

private slots:
    void callbackFrequency(QNetworkReply *rep);
    void callbackMode(QNetworkReply *rep);
    void callbackCloudLog(QNetworkReply *rep);
    void callbackPower(QNetworkReply *rep);
    void callbackPropMode();
    void callbackSatellite();

    void on_save_clicked();

private:
    Ui::CloudLogCATQt *ui;
    int timerId;
    void getFrequency();
    void getMode();
    void getPower();
    void getFromFLRig(QString command, QNetworkAccessManager *manager);
    QString parseXML(QString xml);
    void uploadToCloudLog();
    void loadSettings();

    QNetworkAccessManager *frequencyManager;
    QNetworkAccessManager *modeManager;
    QNetworkAccessManager *powerManager;
    QNetworkAccessManager *cloudLogManager;
    double frequency;
    double realTxFrequency;
    double realRxFrequency;
    QString mode;
    QString propModeDesc;
    QString propModeShort;
    QStringList propMode;
    QStringList propModes;
    QStringList satNames;
    QString satelliteDesc;
    QString satelliteShort;
    QStringList satellite;
    double txOffset;
    double rxOffset;
    int power;

    QString settingsFile;

protected:
    void timerEvent(QTimerEvent *event);

};
#endif // CLOUDLOGCATMACOS_H
