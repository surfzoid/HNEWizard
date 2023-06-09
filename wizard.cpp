#include "wizard.h"
#include "ui_wizard.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>

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

    process = new QProcess(this);
    //connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus){ Wizard::processFinished(exitCode,exitStatus); });

    connect(process, &QProcess::stateChanged, this, &Wizard::addlabelstatus);
    connect(process, &QProcess::readyReadStandardError, this, &Wizard::processreadyReadStandardError);
    connect(process, &QProcess::readyReadStandardOutput, this, &Wizard::processreadyReadStandardOutput);
    connect(process, &QProcess::errorOccurred, this, &Wizard::QPerrorOccurred);

    //check py version and addapt py-hiknetextractor to OS
    Requiere();

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
        {
            newFileData.append(line + '\n');
            continue;
        }

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

    QString HikLocation = "";
    foreach (QString Srce, SrcDir) {
        HikLocation.append(Srce + "/HikNetExtractor\n\n");
        if (QDir(Srce + "/HikNetExtractor").exists())
            return Srce;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Missing depandancys");
    msgBox.addButton(QMessageBox::Open);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.addButton("RPM", QMessageBox::AcceptRole);
    msgBox.addButton("Deb", QMessageBox::ApplyRole);
    msgBox.setText("hiknetextractor-python not found in\n" + HikLocation + "\nIf you are under Linux you can use Deb or RPM button and follow instruction to add repository and install hiknetextractor-python in the good directory, if you are not Linuxien, you need to use the open button to download and extract manualy hiknetextractor-python in the good directory.");
    int Answer = msgBox.exec();
    if (Answer == QMessageBox::Open)
     QDesktopServices::openUrl(QUrl("https://github.com/surfzoid/HikNetExtractor/archive/refs/heads/main.zip", QUrl::TolerantMode));
    if (Answer == 0)
     QDesktopServices::openUrl(QUrl("https://copr.fedorainfracloud.org/coprs/surfzoid/HikNetExtractor/", QUrl::TolerantMode));
    if (Answer == 1)
     QDesktopServices::openUrl(QUrl("https://launchpad.net/~surfzoid/+archive/ubuntu/hikvision", QUrl::TolerantMode));
    exit(1);
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
#if (defined(_WIN32))
    process->start("control", QStringList() << "schedtasks");
#elif ((defined(__linux__) | defined(__APPLE__)) &  !defined(__ANDROID__))
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

    processIsFinished = false;
    process->start("systemctl", QStringList() << "--user" << "enable" << ui->CamNameED->text().toUtf8() + ".timer" << "--now");

    while (!process->waitForFinished(10000)) {

    }

    process->start("systemctl", QStringList() << "--user" << "enable" << ui->CamNameED->text().toUtf8() + ".service" << "--now");
#endif

}

void Wizard::on_BtnDelTimer_released()
{
#if (defined(_WIN32))
    process->start("control", QStringList() << "schedtasks");
#elif ((defined(__linux__) | defined(__APPLE__)) &  !defined(__ANDROID__))
    int ret = QMessageBox::warning(this,tr("Are you sure? "),"Permanently delete timer for this device",QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;
    QFile::remove(SystemdPath + ui->CamNameED->text() + ".service");
    QFile::remove(SystemdPath + ui->CamNameED->text() + ".timer");
    QFile::remove(SystemdPath + "multi-user.target.wants/" + ui->CamNameED->text() + ".service");
    QFile::remove(SystemdPath + "multi-user.target.wants/" + ui->CamNameED->text() + ".timer");
    ui->TxtDebug->append("Timer deleted");
#endif
}

void Wizard::on_BtnDuplicate_released()
{
    bool ok;
    QString NewDevice = QInputDialog::getText(this, tr("New Device"), tr("Enter a new devce name: "), QLineEdit::Normal,ui->CamNameED->text(), &ok);

    if (ok && !NewDevice.isEmpty())
    {
        ui->CamNameED->setText(NewDevice);
        Wizard::on_BtnAdd_released();
        Wizard::on_BtnCronCreate_released();
    }
}

void Wizard::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->TxtDebug->append(QString::number(exitCode));
    ui->TxtDebug->append(QString::number(exitStatus));
    processIsFinished = true;
}

void Wizard::addlabelstatus(QProcess::ProcessState newState)
{
    /*switch(newState) {
        <...>
    };
    if (newState == 0)
    ui->TxtDebug->append(QString::number(newState));*/
}


void Wizard::processreadyReadStandardError()
{
    ui->TxtDebug->append(process->readAllStandardError());
}

void Wizard::processreadyReadStandardOutput()
{
    ui->TxtDebug->append(process->readAllStandardOutput());
}


void Wizard::Requiere()
{
    ProcCase=0;
    process->start("python", QStringList() << "--version");

}


void Wizard::QPerrorOccurred(QProcess::ProcessError error)
{
    //0=py vers;1=hiknetextractor-python;
    switch(ProcCase) {
    case 0:
        if (error==0)
        {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Python Helper");
#if (defined(_WIN32))
        msgBox.addButton(QMessageBox::Open);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setText("Python not found. Do you want to open the download page and install it?");
        if (msgBox.exec() == QMessageBox::Open)
         QDesktopServices::openUrl(QUrl("https://www.python.org/downloads/windows/", QUrl::TolerantMode));
#elif ((defined(__linux__) | defined(__APPLE__)) &  !defined(__ANDROID__))
        msgBox.setText("Python not found. Install it with your package manager");
        msgBox.exec();

#endif
        }
        break;

    case 1:
        break;

    default: ;
        ProcCase=-1;
        break;
    };
}
