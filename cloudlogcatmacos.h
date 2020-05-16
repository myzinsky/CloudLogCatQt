#ifndef CLOUDLOGCATMACOS_H
#define CLOUDLOGCATMACOS_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class CloudLogCATMacOS; }
QT_END_NAMESPACE

class CloudLogCATMacOS : public QMainWindow
{
    Q_OBJECT

public:
    CloudLogCATMacOS(QWidget *parent = nullptr);
    ~CloudLogCATMacOS();

private slots:
    void on_pushButton_clicked();    
    void callbackFrequency(QNetworkReply *rep);
    void callbackMode(QNetworkReply *rep);

private:
    Ui::CloudLogCATMacOS *ui;
    int timerId;
    void getFrequency();
    void getMode();
    void getFromFLRig(QString command, QNetworkAccessManager *manager);
    QString parseXML(QString xml);

    QNetworkAccessManager *frequencyManager;
    QNetworkAccessManager *modeManager;

protected:
    void timerEvent(QTimerEvent *event);

};
#endif // CLOUDLOGCATMACOS_H
