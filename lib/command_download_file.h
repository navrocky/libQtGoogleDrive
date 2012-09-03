#ifndef GOOGLE_DRIVE_COMMAND_DOWNLOAD_FILE_H
#define GOOGLE_DRIVE_COMMAND_DOWNLOAD_FILE_H

#include "authorized_command.h"

class QIODevice;
class QUrl;

namespace GoogleDrive
{

// TODO Download realization with Partial GET
/**
 * @brief Download file command
 */
class CommandDownloadFile : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandDownloadFile(Session* session);
    ~CommandDownloadFile();

    /**
     * This method returns current receive buffer size. By default receive buffer
     * size is 16 Kb.
     * @sa setBufferSize
     */
    qint64 bufferSize() const;

    /**
     * This method change receive buffer size.
     * @param size new receive buffer size
     * @sa bufferSize
     */
    void setBufferSize(qint64 size);

    /**
     * This method returns total amount of received data in bytes.
     */
    qint64 received() const;

public slots:
    void exec(const QUrl& downloadUrl, QIODevice* out);

signals:
    void progress(qint64 bytesReceived);

    /**
     * This signal emits when all of the file data were successfully received.
     * @sa error
     */
    void finished();

protected:
    void reexecuteQuery();

private slots:
    void requestFinished();
    void readyRead();

private:
    struct Impl;
    Impl* d;
};

}

#endif
