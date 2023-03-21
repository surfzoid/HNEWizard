#include "wizard.h"
#include "ui_wizard.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

Wizard::Wizard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Wizard)
{
    ui->setupUi(this);
    ui->PathED->setText(Lastpath);
    ui->SavePathEd->setText(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    Wizard::on_BtnLoad_released();
    for (int i =1;i<60 ;i++ ) {
        ui->CmbxCronFreq->addItem(QString::number(i));
    }
}

Wizard::~Wizard()
{
    delete ui;
}


void Wizard::on_BtnPath_released()
{

    QUrl _IntputFolder = QFileDialog::getExistingDirectory(this,
                                                           (tr("Select Folder to read")), Lastpath);
    //Hist = FsDialog->history();

    if (_IntputFolder.isEmpty() == false) {
        Lastpath = _IntputFolder.path();
        ui->PathED->setText(Lastpath);
    }
    return;
}

void Wizard::on_BtnAdd_released()
{
    QString DestDir = Lastpath + "/" + ui->CamNameED->text() + "/";
    QString SrcDir = GetSrce() + "/HikNetExtractor/template/";

    if (!QDir(DestDir).exists()) {
        QDir().mkdir(DestDir);
    }

    QDir directory(SrcDir);
    QStringList filters;
    filters << "*.py";
    QStringList filesList = directory.entryList(QDir::Files);
    directory.setNameFilters(filters);

    QString fileName;
    foreach(fileName, filesList)
    {
        if(fileName != "config.py")
            if(QFile::remove(DestDir + fileName))
                qDebug() << "Removed: " << fileName;

        QFile::copy(SrcDir + fileName, DestDir + fileName);
        qDebug() << "Copied: " << fileName;

    }

    QString newFileData;
    QFile ConfFs(DestDir + "config.py");

    if(!ConfFs.open(QIODevice::ReadOnly))
        exit(1);

    QTextStream in(&ConfFs);

    //Read file line by line until it reaches the end
    while(!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith("#"))
            goto append;

        if (line.startsWith("url"))
            line = "url = '" + ui->IpEd->text() + ":" + ui->PortHttpEd->text() + "'";

        if (line.startsWith("username"))
            line = "username = '" + ui->UserEd->text() + "'";

        if (line.startsWith("password"))
            line = "password = '" + ui->PassEd->text() + "'";

        if (line.startsWith("Channel"))
            line = "Channel = '" + ui->ChannelEd->text() + "'";

        if (line.startsWith("SavePath"))
            line = "SavePath = '" + ui->SavePathEd->text() + "'";

        if (line.startsWith("CamName"))
            line = "CamName = '" + ui->CamNameED->text() + "'";

        if (line.startsWith("DaysToKeep"))
            line = "DaysToKeep = '" + ui->DaysToKeepEd->text() + "'";

        if (line.startsWith("UtcTOfset")){
            if (ui->UtcTOfset->isChecked()){
                line = "UtcTOfset = 'yes'";
            }
            else
            {
                line = "UtcTOfset = 'no'";
            }
        }
append:
        newFileData.append(line + '\n');
    }

    ConfFs.close();

    if(!ConfFs.open(QIODevice::WriteOnly | QIODevice::Truncate))
        exit(1);

    //Write the data to the output file
    QTextStream out(&ConfFs);
    out << newFileData;
    //ConfFs.write(newFileData.toUtf8().data());

    ConfFs.flush();
    ConfFs.close();
}

QString Wizard::GetSrce()
{
    QStringList SrcDir =  QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    foreach (QString Srce, SrcDir) {
        if (QDir(Srce + "/HikNetExtractor").exists())
            return Srce;
    }

    return "/usr/share";
}

void Wizard::on_BtnCancel_released()
{
    close();
}

void Wizard::on_BtnSavePath_released()
{
    QUrl _IntputFolder = QFileDialog::getExistingDirectory(this,
                                                           (tr("Select Folder to save videos")), ui->SavePathEd->text());
    //Hist = FsDialog->history();

    if (_IntputFolder.isEmpty() == false)
        ui->SavePathEd->setText(_IntputFolder.path() + "/");

    return;
}

void Wizard::on_BtnLoad_released()
{

    QFile ConfFs(ui->PathED->text()  + "/" + ui->CamNameED->text() + "/"+ "config.py");
    if (!ConfFs.exists())
        return;

    if(!ConfFs.open(QIODevice::ReadOnly))
        exit(1);

    QTextStream in(&ConfFs);

    //Read file line by line until it reaches the end
    while(!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith("url"))
        {
            QStringList IpPort = GetParam(line).split(":");
            ui->IpEd->setText( IpPort.at(0));
            if (IpPort.length()>1) {
                ui->PortHttpEd->setText( IpPort.at(1));
            }
            else {
                ui->PortHttpEd->setText( "80");
            }

        }

        if (line.startsWith("username"))
            ui->UserEd->setText(GetParam(line));

        if (line.startsWith("password"))
            ui->PassEd->setText(GetParam(line));

        if (line.startsWith("Channel"))
            ui->ChannelEd->setText(GetParam(line));

        if (line.startsWith("SavePath"))
            ui->SavePathEd->setText(GetParam(line));

        if (line.startsWith("CamName"))
            ui->CamNameED->setText(GetParam(line));

        if (line.startsWith("DaysToKeep"))
            ui->DaysToKeepEd->setText(GetParam(line));

        if (line.startsWith("UtcTOfset")){
            if (GetParam(line) == "yes"){
                ui->UtcTOfset->setChecked(true);
            }
            else
            {
                ui->UtcTOfset->setChecked(false);
            }
        }
    }

    ConfFs.close();

    //add the load of timer value here
    QFile SystemdTimer(SystemdPath + ui->CamNameED->text() + ".timer");
    if (!SystemdTimer.exists())
        return;

    if(!SystemdTimer.open(QIODevice::ReadOnly))
        exit(1);

    QTextStream inTime(&SystemdTimer);

    //Read file line by line until it reaches the end
    while(!inTime.atEnd())
    {
        QString FindTime = inTime.readLine();

        if (FindTime.startsWith("OnUnitActiveSec="))
        {
            FindTime = FindTime.remove("OnUnitActiveSec=").remove("min");

            ui->CmbxCronFreq->setCurrentIndex(ui->CmbxCronFreq->findText(FindTime));
            break;
        }
    }

    SystemdTimer.close();
}

QString Wizard::GetParam(QString ToSplit)
{
    QStringList RetL = ToSplit.split(" = ");
    QString Ret = RetL.at(1);
    return Ret.remove("'").remove("\"");
}

void Wizard::on_PathED_textEdited(const QString &arg1)
{
    Lastpath = arg1;
}

void Wizard::on_BtnCronCreate_released()
{
    QString SystemdServiceTemplatecache = SystemdServiceTemplate;
    QString SystemdTimerTemplatecache = SystemdTimerTemplate;

    SystemdServiceTemplate = SystemdServiceTemplate.replace("CAM",ui->CamNameED->text()).replace("HikNetExtractor.py",ui->PathED->text()  + "/" + ui->CamNameED->text() + "/"+ "HikNetExtractor.py");
    SystemdTimerTemplate = SystemdTimerTemplate.replace("CAM",ui->CamNameED->text()).replace("00MM",ui->CmbxCronFreq->currentText());

    QFile SystemdService(SystemdPath + ui->CamNameED->text() + ".service");
    QFile SystemdTimer(SystemdPath + ui->CamNameED->text() + ".timer");

    if(!SystemdService.open(QIODevice::WriteOnly | QIODevice::Truncate))
        exit(1);
    if(!SystemdTimer.open(QIODevice::WriteOnly | QIODevice::Truncate))
        exit(1);

    SystemdService.write(SystemdServiceTemplate.toUtf8());
    SystemdTimer.write(SystemdTimerTemplate.toUtf8());

    SystemdService.flush();
    SystemdTimer.flush();

    SystemdService.close();
    SystemdTimer.close();

    SystemdServiceTemplate = SystemdServiceTemplatecache;
    SystemdTimerTemplate = SystemdTimerTemplatecache;

    QProcess process;
    process.start("systemctl", QStringList() << "--user" << "enable" << ui->CamNameED->text() + ".timer" << "--now");

}

void Wizard::on_BtnDelTimer_released()
{
    int ret = QMessageBox::warning(this,tr("Are you sure? "),"Permanently delete timer for this device",QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;
    QFile::remove(SystemdPath + ui->CamNameED->text() + ".service");
    QFile::remove(SystemdPath + ui->CamNameED->text() + ".timer");
}

void Wizard::on_BtnDuplicate_released()
{

}
