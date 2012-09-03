#ifndef GOOGLE_DRIVE_COMMAD_FILE_LIST_H
#define GOOGLE_DRIVE_COMMAD_FILE_LIST_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandFileList : public AuthorizedCommand
{
    Q_OBJECT
public:
    explicit CommandFileList(Session* session);

    FileInfoList files() const {return files_;}
    
public slots:
    /*! \see about query https://developers.google.com/drive/search-parameters */
    void exec(const QString& query = QString());

    /*! Get specified folder file list. The root folder id can be obtained by
        CommandAbout */
    void execForFolder(const QString& folderId);

signals:
    void finished(const GoogleDrive::FileInfoList& files);

protected:
    void reexecuteQuery();

private slots:
    void queryFinished();

private:
    FileInfoList files_;
    QString query_;
};

}

#endif // COMMAD_FILE_LIST_H
