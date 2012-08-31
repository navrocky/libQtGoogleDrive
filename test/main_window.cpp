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
#include <QtDebug>

#include "../lib/command_oauth2.h"

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
    connect(ui->actionEdit_options, SIGNAL(triggered()), SLOT(showOptionsDialog()));
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
    session_->setRefreshToken(refreshToken);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showOptionsDialog()
{
    OptionsDialog dlg(this);
    dlg.exec();
}

void MainWindow::login()
{
    QSettings s;
    QString clientId = s.value(cClientId).toString();
    QString clientSecret = s.value(cClientSecret).toString();
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

    oauth2_ = new CommandOAuth2(session_);
    oauth2_->setClientId(clientId);
    oauth2_->setClientSecret(clientSecret);
    oauth2_->setScope(CommandOAuth2::FullAccessScope);
    connect(oauth2_.data(), SIGNAL(finished()), SLOT(authFinished()));
    connect(oauth2_.data(), SIGNAL(error(QString)), SLOT(error(QString)));

    QDesktopServices::openUrl(oauth2_->getLoginUrl());

    QString code = QInputDialog::getText(this, tr("Login"),
                                         tr("Enter the authorization code from web browser"));
    if (code.isEmpty())
        return;

    oauth2_->requestAccessToken(code);
}

void MainWindow::authFinished()
{
    oauth2_->deleteLater();
    QSettings s;
    s.setValue(cRefreshToken, session_->refreshToken());
    s.setValue(cAccessToken, session_->accessToken());

    writeHint(tr("Authorization finished. <br>Access token: <b>%1</b><br><br>Refresh token: <b>%2</b>")
              .arg(session_->accessToken())
              .arg(session_->refreshToken()));
}

void MainWindow::error(const QString &msg)
{
    writeError(tr("Authorization error occurred: %1").arg(msg));
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
