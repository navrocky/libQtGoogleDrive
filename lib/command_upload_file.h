#ifndef GOOGLE_DRIVE_COMMAND_UPLOAD_FILE_H
#define GOOGLE_DRIVE_COMMAND_UPLOAD_FILE_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandUploadFilePrivate;

/**
 * @brief Uploads file contents
 *
 * This command uploads file contents with metadata to the GD.
 * If provided FileInfo contains valid file ID than file metadata and data will
 * be rewritten, otherwise the new file will be created.
 *
 * After uploading
 */
class CommandUploadFile : public AuthorizedCommand
{
    Q_OBJECT
public:
    enum UploadType
    {
        /**
         * Uploading with single request. Acceptable if the data you are sending
         * is small enough to upload again in its entirety if the connection
         * fails.
         */
        Multipart,

        /**
         * This method allows you to resume an upload operation after a
         * communication failure has interrupted the flow of data.
         * @warning Not realized at this moment
         */
        Resumable
    };

    CommandUploadFile(Session* session);

    /**
     * Returns uploading type. Default value is Multipart.
     * @sa setUploadType
     */
    UploadType uploadType() const;

    /**
     * Change uploading type.
     * @sa uploadType
     */
    void setUploadType(UploadType);

    /**
     * Resulting file info after successfully executed command.
     * @return FileInfo information about uploaded file.
     */
    const FileInfo& resultFileInfo() const;

public slots:
    /**
     * Execute upload command. After successfully execution resultFileInfo will
     * contain updated file information.
     * @param fileInfo file meta information. If FileInfo::id is not empty then
     *        command will overwrite file with specified id, otherwise the new
     *        file will be created.
     * @param fileData QIODevice that provides file data. Command doesn't own
     *        this object, so it must exists during command execution.
     * @sa finished, error, progress
     */
    void exec(const FileInfo& fileInfo, QIODevice* fileData);

signals:
    /**
     * This signal is emitted during uploading process.
     * @param bytesUploaded current amount of uploaded data
     * @param total total amount of data to be upload
     */
    void progress(qint64 bytesUploaded, qint64 total);

protected:
    void reexecuteQuery();

private slots:
    void requestFinished();

private:
    void multipartUpload();

    Q_DECLARE_PRIVATE(CommandUploadFile)
};

}

#endif
