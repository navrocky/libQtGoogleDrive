
#include <iostream>
#include <stdexcept>

#include <QCoreApplication>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QSettings>
#include <QTimer>
#include <QtGui/QDesktopServices>

#include "../lib/session.h"
#include "../lib/command_oauth2.h"
#include "../lib/command_file_list.h"

#include "gdrive.h"

using namespace GoogleDrive;

const QString c_clientId = "1077721689698.apps.googleusercontent.com";
const QString c_clientSecret = "HoJXcAl4JdCDHEohBVAvTW9a";

const QString c_refreshToken = "refreshToken";

struct GDrive::Pimpl {
    
    Pimpl()
        : session(&manager)
    {     
        session.setClientId(c_clientId);
        session.setClientSecret(c_clientSecret);
    }
    
    QNetworkAccessManager manager;
    Session session;
    QEventLoop loop;    
    QString error;
};

GDrive::GDrive(QObject* parent)
    : QObject(parent)
    , p_(new Pimpl)
{
    QTimer::singleShot(0, this, SLOT(init()));
}

GDrive::~GDrive()
{
}

void GDrive::init()
{
    QSettings s;
    
    if (!s.value(c_refreshToken).isNull())
    {
        p_->session.setRefreshToken(s.value(c_refreshToken).value<QString>());
    }
    else
    {
        refreshToken();
        s.setValue(c_refreshToken, p_->session.refreshToken());
    }
    
    list();
}


bool GDrive::refreshToken()
{
    CommandOAuth2 cmd(&p_->session);
    cmd.setScope(CommandOAuth2::FullAccessScope);
    QObject::connect(&cmd, SIGNAL(finished()), SLOT(finish()));
    QObject::connect(&cmd, SIGNAL(error(QString)), SLOT(error(QString)));

    QDesktopServices::openUrl(cmd.getLoginUrl());

    std::cout << "Enter auth code:" << std::endl;
    std::string code;
    std::cin >> code;
    
    cmd.requestAccessToken(code.c_str());
    
    if (p_->loop.exec() != 0)
        throw std::runtime_error(("can't obtain refresh tocken:" + p_->error).toStdString());
}

void GDrive::finish()
{
    p_->loop.quit();
}

void GDrive::error(QString message)
{
    p_->error = message;
    p_->loop.exit(1);
}


void GDrive::list()
{
    CommandFileList cmd(&p_->session);
    QObject::connect(&cmd, SIGNAL(finished(const GoogleDrive::FileInfoList)), SLOT(finish()));
    QObject::connect(&cmd, SIGNAL(error(QString)), SLOT(error(QString)));
    
    cmd.exec();
    
    if (p_->loop.exec() != 0)
        throw std::runtime_error(("can't obtain list:" + p_->error).toStdString());
    
    foreach(const GoogleDrive::FileInfo& info, cmd.files()) {
        std::cerr << info.title().toStdString() << std::endl;
    }
}


