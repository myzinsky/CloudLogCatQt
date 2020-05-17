#ifndef CLOUDLOGCATMACOS_H
#define CLOUDLOGCATMACOS_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QNetworkReply>
#include <QSettings>

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

    void on_save_clicked();

private:
    Ui::CloudLogCATQt *ui;
    int timerId;
    void getFrequency();
    void getMode();
    void getFromFLRig(QString command, QNetworkAccessManager *manager);
    QString parseXML(QString xml);
    void uploadToCloudLog();
    void loadSettings();

    QNetworkAccessManager *frequencyManager;
    QNetworkAccessManager *modeManager;
    QNetworkAccessManager *cloudLogManager;
    QString frequency;
    QString mode;

    QString settingsFile;

protected:
    void timerEvent(QTimerEvent *event);

};
#endif // CLOUDLOGCATMACOS_H
