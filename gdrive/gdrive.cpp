
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

#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <boost/property_tree/json_parser.hpp>
#include "gdrive.h"

using namespace GoogleDrive;

const QString c_clientId = "1077721689698.apps.googleusercontent.com";
const QString c_clientSecret = "HoJXcAl4JdCDHEohBVAvTW9a";

const QString c_refreshToken = "refreshToken";

struct gdrive::Pimpl {
    
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

gdrive::gdrive(QObject* parent)
    : QObject(parent)
    , p_(new Pimpl)
{
    QTimer::singleShot(0, this, SLOT(init()));
}

gdrive::~gdrive()
{
}

void gdrive::init()
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
}


bool gdrive::refreshToken()
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

void gdrive::finish()
{
    p_->loop.quit();
}

void gdrive::error(QString message)
{
    p_->error = message;
    p_->loop.exit(1);
}


void gdrive::list(const variables_map& vars)
{
    CommandFileList cmd(&p_->session);
    QObject::connect(&cmd, SIGNAL(finished(const GoogleDrive::FileInfoList)), SLOT(finish()));
    QObject::connect(&cmd, SIGNAL(error(QString)), SLOT(error(QString)));
    
    cmd.exec();
    
    if (p_->loop.exec() != 0)
        throw std::runtime_error(("can't obtain list:" + p_->error).toStdString());
    
    foreach(const GoogleDrive::FileInfo& info, cmd.files()) {
        if (info.isRoot())
        {
            std::cout << "[root] " << info.title().toLocal8Bit().constData() << std::endl;
        }
    }
    
    foreach(const GoogleDrive::FileInfo& info, cmd.files()) {
        if (info.isFolder())
        {
            std::cout << "[dir]  " << info.title().toLocal8Bit().constData() << std::endl;
        }
    }

    foreach(const GoogleDrive::FileInfo& info, cmd.files()) {
        if (!info.isFolder())
        {
            std::cout << "[file] " << info.title().toLocal8Bit().constData() << std::endl;
            
//             if (info.title() == "Ермак")
            {
//                 QJson::Serializer serializer;
//                 QByteArray s =  serializer.serialize(info.rawData());
// 
//                 //this is holy shit workaround, because of QJson::Serializer doesn't
//                 //indent output text, but boost::json_parser does!
//                 std::stringstream ss;
//                 ss.str(QString(s).toStdString());
// 
//                 boost::property_tree::ptree ptree;
//                 boost::property_tree::read_json( ss, ptree );
//                 boost::property_tree::write_json( ss, ptree );
// 
//                 std::cerr << ss.str() << std::endl;
            }
            
            
        }
    }

    
    emit finished(EXIT_SUCCESS);
}


