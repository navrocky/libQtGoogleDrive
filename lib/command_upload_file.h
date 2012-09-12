#ifndef GOOGLE_DRIVE_COMMAND_UPLOAD_FILE_H
#define GOOGLE_DRIVE_COMMAND_UPLOAD_FILE_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

/**
 * @brief Upload file contents
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
         */
        Resumable
    };

    CommandUploadFile(Session* session);
    ~CommandUploadFile();

    /**
     * Returns uploading type. Default value is Multipart.
     * @sa setUploadType
     */
    UploadType uploadType() const;
    void setUploadType(UploadType);

    void exec(const FileInfo& fileInfo, QIODevice* fileData);

signals:
    /**
     * This signal is emitted during uploading process.
     * @param bytesUploaded current amount of uploaded data
     * @param total total amount of data to be upload
     */
    void progress(qint64 bytesUploaded, qint64 total);

    /**
     * This signal emits when all of the file data were successfully uploaded.
     * @param fileInfo returned from server file information
     * @sa error signal
     */
    void finished(const GoogleDrive::FileInfo& fileInfo);

protected:
    void reexecuteQuery();

private slots:
    void requestFinished();

private:
    void multipartUpload();

    struct Impl;
    Impl* d;
};

}

#endif
