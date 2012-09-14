#ifndef GOOGLE_DRIVE_COMMAND_DOWNLOAD_FILE_H
#define GOOGLE_DRIVE_COMMAND_DOWNLOAD_FILE_H

#include "authorized_command.h"

class QIODevice;
class QUrl;

namespace GoogleDrive
{

class CommandDownloadFilePrivate;

// TODO: Download realization with Partial GET
/**
 * @brief Download file content
 */
class CommandDownloadFile : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandDownloadFile(Session* session);

    /**
     * This method returns current receive buffer size. By default receive buffer
     * size is 16 kB.
     * @sa setBufferSize
     */
    qint64 bufferSize() const;

    /**
     * This method change receive buffer size. Default buffer size is 16 kB
     * @param size new receive buffer size
     * @sa bufferSize
     */
    void setBufferSize(qint64 size);

public slots:
    /**
     * Start downloading process.
     * @param downloadUrl download or export url provided by FileInfo::downloadUrl
     * @param out output device for storing received file content. It can be a
     *        QFile, QBuffer, etc.
     */
    void exec(const QUrl& downloadUrl, QIODevice* out);

signals:
    /**
     * This signal is emitted during downloading process.
     * @param bytesReceived current amount of received data
     * @param total total amount of data to be received
     */
    void progress(qint64 bytesReceived, qint64 total);

protected:
    void reexecuteQuery();

private slots:
    void requestFinished();
    void readyRead();

private:
    Q_DECLARE_PRIVATE(CommandDownloadFile)
};

}

#endif
