
#include <iostream>
#include <stdexcept>
#include <boost/bind.hpp>

#include <QCoreApplication>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QSettings>
#include <QTimer>
#include <QDesktopServices>
#include <QStringList>

#include "../lib/session.h"
#include "../lib/command_oauth2.h"
#include "../lib/command_about.h"
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

    QDesktopServices::openUrl(cmd.getLoginUrl());

    std::cout << "Enter auth code:" << std::endl;
    std::string code;
    std::cin >> code;
    
    cmd.requestAccessToken(code.c_str());
    
    if (p_->loop.exec() != 0)
        throw std::runtime_error(("can't obtain refresh token:" + p_->error).toStdString());
}

void gdrive::finish()
{
    Command* cmd = qobject_cast<Command*>(sender());
    if (cmd->error() == Command::NoError)
    {
        p_->loop.quit();
    }
    else
    {
        p_->error = cmd->errorString();
        p_->loop.exit(1);
    }
}

void gdrive::list(const QString& path)
{
    QString query;
    
    foreach(const QString& token, path.split("/")) {
        if (token.isEmpty()) continue;
        
        query.isEmpty()
            ? query += QString("title = '%1'").arg(token)
            : query += QString(" or title = '%1'").arg(token);
    }
    
    if (query.isEmpty())
    {
        CommandAbout about_cmd(&p_->session);
        about_cmd.exec();
        
        if (!about_cmd.waitForFinish())
            throw std::runtime_error(about_cmd.errorString().toLocal8Bit().constData());
        
        const QString root_id = about_cmd.resultInfo().rootFolderId();
        query = QString("'%1' in parents").arg(root_id);
    }
    
    CommandFileList ls_cmd(&p_->session);
    ls_cmd.exec(query);
    
    if (!ls_cmd.waitForFinish())
        throw std::runtime_error(ls_cmd.errorString().toLocal8Bit().constData());
    
    FileInfoList files = ls_cmd.files();
    
    foreach(const QString& token, path.split("/")) {
        if (token.isEmpty()) continue;
        
        FileInfoList::const_iterator it = std::find_if(files.begin(), files.end(), boost::bind(&FileInfo::title, _1) == token);
        if (it == files.end())
            throw std::runtime_error((QString("No such file:") + QString(token)).toLocal8Bit().constData());
    }
    
    
//     foreach(const QString& token, path.split("/")) {
//         FileInfoList::const_iterator it = std::find_if(files.begin(), files.end(), boost::bind(&FileInfo::title, _1) == token);
//         if (it == files.end())
//             throw std::runtime_error((QString("No such file:") + QString(token)).toLocal8Bit().constData());
//     }
    
/*    
    
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
    }*/

    foreach(const GoogleDrive::FileInfo& info, ls_cmd.files()) {
//         if (!info.isFolder())
        {
            std::cout << "[file] " << info.title().toLocal8Bit().constData() << std::endl;
//             std::cout << "    -- " << info.id().toStdString() << std::endl;
            
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


