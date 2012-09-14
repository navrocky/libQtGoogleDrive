#ifndef GOOGLE_DRIVE_COMMAD_FILE_LIST_H
#define GOOGLE_DRIVE_COMMAD_FILE_LIST_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandFileListPrivate;

/**
 * @brief The CommandFileList class
 */
class CommandFileList : public AuthorizedCommand
{
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param session
     */
    explicit CommandFileList(Session* session);

    /**
     * If command success then this method returns received file list.
     * @return received file list.
     */
    FileInfoList files() const;
    
public slots:
    /**
     * Executes command.
     * @sa about query https://developers.google.com/drive/search-parameters
     * @sa files
     */
    void exec(const QString& query = QString());

    /**
     * Get specified folder file list. The root folder id can be obtained by
     * CommandAbout.
     * @sa CommandAbout::rootFolder, files
     */
    void execForFolder(const QString& folderId);

protected:
    void reexecuteQuery();

private slots:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandFileList)
};

}

#endif
