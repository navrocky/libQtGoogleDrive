#ifndef GOOGLE_DRIVE_COMMAD_FILE_LIST_H
#define GOOGLE_DRIVE_COMMAD_FILE_LIST_H

#include "command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandFileList : public Command
{
    Q_OBJECT
public:
    explicit CommandFileList(Session* session);

    FileInfoList files() const {return files_;}
    
public slots:
    /*! \see about query https://developers.google.com/drive/search-parameters */
    void exec(const QString& query = QString());

signals:
    void finished(const FileInfoList& files);

private slots:
    void queryFinished();

private:
    void execute();

    FileInfoList files_;
    QString query_;
};

}

#endif // COMMAD_FILE_LIST_H
