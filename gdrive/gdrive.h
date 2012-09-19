
#ifndef GDOCS_GDOCS_H
#define GDOCS_GDOCS_H

#include <memory>

#include <boost/function.hpp>

#include <QObject>
#include <QStringList>

#include "../lib/file_info.h"

class gdrive : public QObject {
    Q_OBJECT
public:
    
    explicit gdrive(QObject* parent = 0);
    virtual ~gdrive();
    
signals:
    void finished(int);

public:
	void delay(boost::function<void()> f);
	
    void list(const QString& path = QString());
	void get(const QString& path, const QString& output);
    
private slots:
    void init();
    bool refreshToken();

private:
	GoogleDrive::FileInfoList request_items(const QString& path);
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};


class Explorer
{
public:
    
    Explorer(const GoogleDrive::FileInfoList& list);
    
    void cd(const QString& path);
    
    bool isDir() const;
    bool isFile() const;
    
    QString path() const;

    const GoogleDrive::FileInfo& current() const;
   
private:
    QStringList path_;
    const GoogleDrive::FileInfoList& list_;
    GoogleDrive::FileInfoList path_list_; 
};




#endif
