#include "main_window.h"

#include <QAction>
#include <QInputDialog>
#include <QSettings>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>
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
{
    ui->setupUi(this);
    connect(ui->actionLogin, SIGNAL(triggered()), SLOT(login()));
    connect(ui->actionEdit_options, SIGNAL(triggered()), SLOT(showOptionsDialog()));
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
    delete session_;

    QSettings s;
    QString clientId = s.value(cClientId).toString();
    QString clientSecret = s.value(cClientSecret).toString();
    QString refreshToken = s.value(cRefreshToken).toString();
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

    session_ = new Session(manager_, this);

    if (!refreshToken.isEmpty())
    {
        session_->setRefreshToken(refreshToken);
        session_->setAccessToken(s.value(cAccessToken).toString());
        authorized();
        return;
    }

    oauth2_ = new CommandOAuth2(session_);
    oauth2_->setClientId(clientId);
    oauth2_->setClientSecret(clientSecret);
    connect(oauth2_.data(), SIGNAL(finished()), SLOT(authFinished()));
    connect(oauth2_.data(), SIGNAL(error(QString)), SLOT(error(QString)));

    QDesktopServices::openUrl(oauth2_->getLoginUrl());

    QString code = QInputDialog::getText(this, tr("Login"),
                                         tr("Enter the authorization code from web browser"));
    if (code.isEmpty())
        return;

    oauth2_->requestAuthToken(code);
}

void MainWindow::authFinished()
{
    oauth2_->deleteLater();
    QSettings s;
    s.setValue(cRefreshToken, session_->refreshToken());
    s.setValue(cAccessToken, session_->accessToken());
    QMessageBox::information(this, tr("Authorization finished"),
                             tr("Access token: <b>%1</b><br><br>Refresh token: <b>%2</b>")
                             .arg(session_->accessToken())
                             .arg(session_->refreshToken()));
    authorized();
}

void MainWindow::error(const QString &msg)
{
    QMessageBox::critical(this, tr("Error occurred"), msg);
}

void MainWindow::authorized()
{
    qDebug() << "authorized";
}
