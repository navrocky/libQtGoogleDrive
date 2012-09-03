#include "main_window.h"

#include <QAction>
#include <QInputDialog>
#include <QSettings>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>
#include <QTextEdit>
#include <QScrollBar>
#include <QTime>
#include <QStatusBar>
#include <QtDebug>
#include <QFile>

#include "../lib/command_oauth2.h"
#include "../lib/command_file_list.h"
#include "../lib/command_download_file.h"

#include "options_dialog.h"
#include "ui_main_window.h"
#include "options.h"

using namespace GoogleDrive;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , manager_(new QNetworkAccessManager(this))
    , session_(new Session(manager_, this))
{
    ui->setupUi(this);
    connect(ui->actionLogin, SIGNAL(triggered()), SLOT(login()));
    connect(ui->actionGet_file_list, SIGNAL(triggered()), SLOT(getFileList()));
    connect(ui->actionEdit_options, SIGNAL(triggered()), SLOT(showOptionsDialog()));
    connect(ui->actionDownload_file, SIGNAL(triggered()), SLOT(downloadFile()));
    writeText(tr("<p><h1>Welcome to the Qt Google Drive API test</h1></p><br>"));

    QSettings s;
    QString clientId = s.value(cClientId).toString();
    QString clientSecret = s.value(cClientSecret).toString();
    QString refreshToken = s.value(cRefreshToken).toString();
    if (clientId.isEmpty() || clientSecret.isEmpty())
    {
        writeHint(tr("<br>Firstly you have to fill Client ID and Client Secret in the options dialog."
                     " After that you can try to login and execute other operations."), false);
    }
    session_->setClientId(clientId);
    session_->setClientSecret(clientSecret);
    session_->setRefreshToken(refreshToken);
    connect(session_,
            SIGNAL(started(GoogleDrive::Command*)),
            SLOT(started(GoogleDrive::Command*)));
    connect(session_,
            SIGNAL(finished(GoogleDrive::Command*)),
            SLOT(finished(GoogleDrive::Command*)));
    connect(session_,
            SIGNAL(error(GoogleDrive::Command*,QString)),
            SLOT(error(GoogleDrive::Command*,QString)));
    connect(session_,
            SIGNAL(reauthorizationNeeded(GoogleDrive::Command*,QString)),
            SLOT(reauthorizationNeeded(GoogleDrive::Command*,QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::started(Command *cmd)
{
    executingCommands_.insert(cmd);
    updateStatusBar();
}

void MainWindow::finished(Command *cmd)
{
    executingCommands_.remove(cmd);
    updateStatusBar();
}

void MainWindow::showOptionsDialog()
{
    OptionsDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    QSettings s;
    QString clientId = s.value(cClientId).toString();
    QString clientSecret = s.value(cClientSecret).toString();
    QString refreshToken = s.value(cRefreshToken).toString();
    session_->setClientId(clientId);
    session_->setClientSecret(clientSecret);
    session_->setRefreshToken(refreshToken);
}

void MainWindow::login()
{
    QSettings s;
    QString clientId = session_->clientId();
    QString clientSecret = session_->clientSecret();
    if (clientId.isEmpty() || clientSecret.isEmpty())
    {
        QMessageBox mb(QMessageBox::Warning, tr("Your options is incompleted"),
                             tr("<p>Firstly you must specify your application ClientID and ClientSecret in the options dialog.</p>"));
        mb.setParent(this);
        mb.setInformativeText("<p>Read more <a href=\"https://developers.google.com/drive/quickstart#enable_the_drive_api\">https://developers.google.com/drive/quickstart#enable_the_drive_api</a></p>");
        mb.setTextFormat(Qt::RichText);
        mb.exec();
        showOptionsDialog();
        return;
    }

    CommandOAuth2* cmd = new CommandOAuth2(session_);
    cmd->setAutoDelete(true);
    cmd->setScope(CommandOAuth2::FullAccessScope);
    connect(cmd, SIGNAL(finished()), SLOT(authFinished()));

    QDesktopServices::openUrl(cmd->getLoginUrl());

    QString code = QInputDialog::getText(this, tr("Login"),
                                         tr("Enter the authorization code from web browser"));
    if (code.isEmpty())
        return;

    cmd->requestAccessToken(code);
}

void MainWindow::authFinished()
{
    QSettings s;
    s.setValue(cRefreshToken, session_->refreshToken());
    s.setValue(cAccessToken, session_->accessToken());

    writeHint(tr("Authorization finished. <br>Access token: <b>%1</b><br><br>Refresh token: <b>%2</b>")
              .arg(session_->accessToken())
              .arg(session_->refreshToken()));
}

void MainWindow::error(GoogleDrive::Command* cmd, const QString &msg)
{
    writeError(msg);
    executingCommands_.remove(cmd);
    updateStatusBar();
}

void MainWindow::reauthorizationNeeded(Command *cmd, const QString &msg)
{
    writeError(tr("Reauthorization needed: %1").arg(msg));
    executingCommands_.remove(cmd);
    updateStatusBar();
}

void MainWindow::getFileList()
{
    CommandFileList* cmd = new CommandFileList(session_);
    cmd->setAutoDelete(true);
    connect(cmd,
            SIGNAL(finished(GoogleDrive::FileInfoList)),
            SLOT(getFileListFinished(GoogleDrive::FileInfoList)));
    cmd->exec();
}

void MainWindow::getFileListFinished(const FileInfoList& list)
{
    writeHint(tr("File list aquired"));
    foreach (const FileInfo& fi, list)
    {
        writeInfo(fi.title());
    }
}

void MainWindow::downloadFile()
{


    CommandDownloadFile* cmd = new CommandDownloadFile(session_);
    cmd->setAutoDelete(true);
    QFile* f = new QFile("downloaded.file", cmd);
    f->open(QFile::WriteOnly);
    connect(cmd, SIGNAL(finished()), SLOT(downloadFileFinished()));
    connect(cmd, SIGNAL(progress(qint64)), SLOT(downloadProgress(qint64)));
    cmd->exec(QUrl("https://doc-0o-a8-docs.googleusercontent.com/docs/securesc/c01rdi4mpormcu350sf2rcrekuvrj182/l1psq31tt4aki2ahnomn8qh3ju00tbfd/1346702400000/07864899089838459059/07864899089838459059/0B0M_HQIvDuFDMkRyV3ByTnVaV2M?h=16653014193614665626&e=download&gd=true")
              , f);
}

void MainWindow::downloadFileFinished()
{
    writeHint("finished");
}

void MainWindow::downloadProgress(qint64 v)
{
    writeHint(tr("download progress: %1").arg(v));
}

void MainWindow::writeInfo(const QString &msg, bool time)
{
    writeMessage(msg, Qt::black, time);
}

void MainWindow::writeText(const QString &msg)
{
    QTextEdit* te = ui->textEdit;
    QScrollBar* scroll = te->verticalScrollBar();
    bool atEnd = scroll->value() == scroll->maximum();
    te->moveCursor(QTextCursor::End);
    te->insertHtml(msg);
    if (atEnd)
        scroll->setValue(scroll->maximum());
}

void MainWindow::writeMessage(const QString &msg, QColor col, bool time)
{
    QString timeStr;
    if (time)
    {
        timeStr = QString("<span style=\"font-size: 7pt;\">%1 : </span>")
                .arg(QTime::currentTime().toString("hh:mm:ss"));
    }
    writeText(QString("%1<span style=\"color: %2;\">%3</span><br>")
              .arg(timeStr).arg(col.name()).arg(msg));
}

void MainWindow::updateStatusBar()
{
    if (executingCommands_.isEmpty())
    {
        statusBar()->clearMessage();
        return;
    }
    QStringList sl;
    foreach (Command* cmd, executingCommands_)
    {
        sl << cmd->metaObject()->className();
    }
    statusBar()->showMessage(tr("Executing commands: %1").arg(sl.join(", ")));
}

void MainWindow::writeHint(const QString &msg, bool time)
{
    writeMessage(msg, Qt::blue, time);
}

void MainWindow::writeError(const QString &msg, bool time)
{
    writeMessage(msg, Qt::red, time);
}

void MainWindow::writeDelimiter()
{
    writeText("<hr>");
}
