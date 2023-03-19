#ifndef WIZARD_H
#define WIZARD_H

#include <QMainWindow>

#include <QStandardPaths>

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
};
#endif // WIZARD_H
