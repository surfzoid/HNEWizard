#ifndef WIZARD_H
#define WIZARD_H

#include <QMainWindow>

#include <QStandardPaths>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Wizard; }
QT_END_NAMESPACE

class Wizard : public QMainWindow
{
    Q_OBJECT

public:
    Wizard(QWidget *parent = nullptr);
    ~Wizard();

private slots:
    void on_BtnPath_released();

    void on_BtnAdd_released();

    void on_BtnCancel_released();

    void on_BtnSavePath_released();

    void on_BtnLoad_released();

    void on_PathED_textEdited(const QString &arg1);

    void on_BtnCronCreate_released();

    void on_BtnDelTimer_released();

    void on_BtnDuplicate_released();

private:
    Ui::Wizard *ui;
    QString Lastpath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString GetSrce();
    QString GetParam(QString ToSplit);

    enum ConfParam{url = 0,
                    username = 1,
                    password = 2,
                    Channel = 3
                   };

    QString SystemdServiceTemplate = "[Unit]\n"
"Description=Run HikNetExtractor to backup videos of CAM device.\n"
"\n"
"[Service]\n"
"Type=simple\n"
"ExecStart=HikNetExtractor.py\n"
"\n"
"[Install]\n"
"WantedBy=multi-user.target";

    QString SystemdTimerTemplate = "[Unit]\n"
"Description=Run HikNetExtractor to backup videos of CAM device.\n"
"\n"
"[Timer]\n"
"OnUnitActiveSec=00MMmin\n"
"AccuracySec=1us\n"
"\n"
"[Install]\n"
"WantedBy=multi-user.target\n";

    QString SystemdPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/systemd/user/";

};
#endif // WIZARD_H
