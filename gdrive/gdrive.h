
#ifndef GDOCS_GDOCS_H
#define GDOCS_GDOCS_H

#include <memory>
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
    
public slots:
    void list(const QString& path = QString());
    
private slots:
    void init();
    bool refreshToken();
    void finish();
    
    
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
