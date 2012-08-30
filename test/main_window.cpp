#include "main_window.h"

#include <QAction>
#include <QInputDialog>
#include <QSettings>
#include <QDesktopServices>
#include <QMessageBox>

#include "../lib/command_oauth2.h"

#include "options_dialog.h"
#include "ui_main_window.h"

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
    session_ = new Session(manager_, this);

    oauth2_ = new CommandOAuth2(session_);
    // TODO: get from options
    oauth2_->setClientId("956138661352.apps.googleusercontent.com");
    oauth2_->setClientSecret("z28p15ZT80Yrq6TrVYKXQdyA");
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

    QMessageBox::information(this, tr("Authorization finished"),
                             tr("Access token: <b>%1</b><br><br>Refresh token: <b>%2</b>")
                             .arg(session_->accessToken())
                             .arg(session_->refreshToken()));
}

void MainWindow::error(const QString &msg)
{
    QMessageBox::critical(this, tr("Error occurred"), msg);
}
