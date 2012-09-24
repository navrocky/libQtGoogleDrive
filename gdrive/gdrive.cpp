
#include <iostream>
#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/range/adaptors.hpp>

#include <QCoreApplication>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QSettings>
#include <QTimer>
#include <QDesktopServices>
#include <QStringList>
#include <QFile>

#include "../lib/session.h"
#include "../lib/command_oauth2.h"
#include "../lib/command_about.h"
#include "../lib/command_file_list.h"
#include "../lib/command_download_file.h"

#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <boost/property_tree/json_parser.hpp>
#include "gdrive.h"

using namespace boost::adaptors;

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

	boost::function<void()> delayed;
    QNetworkAccessManager manager;
    Session session;
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
        refresh_token();
        s.setValue(c_refreshToken, p_->session.refreshToken());
    }
    p_->delayed();
}

void gdrive::refresh_token()
{
    CommandOAuth2 cmd(&p_->session);
    cmd.setScope(CommandOAuth2::FullAccessScope);

    QDesktopServices::openUrl(cmd.getLoginUrl());

    std::cout << "Enter auth code:" << std::endl;
    std::string code;
    std::cin >> code;

    cmd.requestAccessToken(code.c_str());

    if (!cmd.waitForFinish())
        throw std::runtime_error(("can't obtain refresh token:" + p_->error).toStdString());
}

FileInfoList gdrive::request_items(const QString &path)
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

    return ls_cmd.files();
}


void gdrive::list(const boost::program_options::variables_map& vm)
{
	assert(!p_->delayed);
    p_->delayed = [&] () {
        const QString path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());
        FileInfoList files = request_items(path);

        FileInfoExplorer e(files);
        if (!e.cd(path))
            throw std::runtime_error(e.error().toLocal8Bit().constData());
        
        if (!e.path().isEmpty())
        {
            if (e.isDir())
            {
                CommandFileList ls_cmd(&p_->session);
                ls_cmd.execForFolder(e.current().id());
                if (!ls_cmd.waitForFinish())
                    throw std::runtime_error(ls_cmd.errorString().toLocal8Bit().constData());
                
                files = ls_cmd.files();
            }
            else if (e.isFile())
            {
                files = FileInfoList() << e.current();
            }
        }
    
        
        foreach(const GoogleDrive::FileInfo& info, files) {
            std::cout << "[file] " << info.title().toLocal8Bit().constData() << std::endl;
        }
        
        emit finished(EXIT_SUCCESS);
    };
}

QUrl extract_download_link(const FileInfo& info, const QString& format)
{
	if (!info.downloadUrl().isEmpty())
	{
		return info.downloadUrl();
	}
	else if (info.exportList().contains(format))
	{
		return info.exportList().value(format);
	}
	else
	{
		throw std::runtime_error("invalid export format");
	}
}

void gdrive::get(const boost::program_options::variables_map& vm)
{
	assert(!p_->delayed);	
	p_->delayed = [&] () {
		const QString path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());
		const QString format = QString::fromLocal8Bit(vm.at("format").as<std::string>().c_str());
		
		const FileInfoList files = request_items(path);

		FileInfoExplorer e(files);
		if (!e.cd(path))
			throw std::runtime_error(e.error().toLocal8Bit().constData());

		if (e.path().isEmpty())
			throw std::runtime_error("can't download empty path");

		if (e.isDir())
			throw std::runtime_error("can't download directory");

		if (e.isFile())
		{
			QString output;
			QFile file(output);

			bool b = (output.isEmpty())
				? file.open(stdout, QIODevice::WriteOnly)
				: file.open(QIODevice::ReadWrite | QIODevice::Truncate);

			if (!b)
				throw std::runtime_error(std::string("Can't open output: ")
					+ ((output.isEmpty()) ? "STDOUT" : output.toLocal8Bit().constData()) );

			CommandDownloadFile dwn_cmd(&p_->session);
			dwn_cmd.exec(extract_download_link(e.current(), format), &file);
			if (!dwn_cmd.waitForFinish())
					throw std::runtime_error(dwn_cmd.errorString().toLocal8Bit().constData());
		}

		emit finished(EXIT_SUCCESS);
	};
}

void gdrive::formats(const boost::program_options::variables_map& vm)
{
	assert(!p_->delayed);	
    p_->delayed = [&] () {
        const QString path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());    
        const FileInfoList files = request_items(path);

        FileInfoExplorer e(files);
        if (!e.cd(path))
            throw std::runtime_error(e.error().toLocal8Bit().constData());

        if (!e.path().isEmpty())
        {
            foreach(const QString& format, e.current().exportList().toStdMap() | map_keys) {
                std::cout << format.toLocal8Bit().constData() << std::endl;
            }
        }
        emit finished(EXIT_SUCCESS);
    };
}


FileInfoExplorer::FileInfoExplorer(const FileInfoList& list)
    : list_(list)
{
}

bool FileInfoExplorer::cd(const QStringList& list)
{
	foreach (const QString& token, list) {
		if (token.isEmpty()) continue;

		const auto finder = [&] (const FileInfo& info) {
			return info.title() == token
				&& (path_list_.isEmpty() || info.parents().contains(path_list_.back().id()) );
		};
		
		const FileInfoList::const_iterator it = std::find_if(list_.begin(), list_.end(), finder);

		
		if (it == list_.end())
		{
			error_ = "there is no item with name: " + token;
			return false;
		}
		else if (std::find_if(it + 1, list_.end(), finder) != list_.end())
		{
			error_ = "there is more than one item with name: " + token;
			return false;
		}

		path_list_ << *it;
		path_ << token;
	}

	return true;
}

bool FileInfoExplorer::cd(const QString& path)
{
	return cd(path.split("/"));
}

QString FileInfoExplorer::path() const
{
    return path_.join("/");
}


bool FileInfoExplorer::isDir() const
{
    return path_list_.back().isFolder();
}

bool FileInfoExplorer::isFile() const
{
    return !path_list_.back().isFolder();
}

const FileInfo& FileInfoExplorer::current() const
{
    return path_list_.back();
}




