#ifndef GOOGLE_DRIVE_COMMAND_H
#define GOOGLE_DRIVE_COMMAND_H

#include <QObject>
#include <QVariant>
#include <QScopedPointer>

class QNetworkRequest;
class QNetworkReply;

namespace GoogleDrive
{

class Session;
class CommandPrivate;

/**
 * @brief Base class for all GAPI commands.
 *
 */
class Command : public QObject
{
    Q_OBJECT
public:

    enum Error
    {
        /**
         * No error, command finished successfully
         */
        NoError,

        /**
         * Full cycle authorization is needed
         */
        AuthorizationNeeded,

        /**
         * Other unclassified errors
         */
        UnknownError
    };

    explicit Command(Session* session);
    ~Command();

    bool autoDelete() const;
    void setAutoDelete(bool v);

    Error error() const;
    QString errorString() const;

signals:
    /**
     * Emitted when command is finished in any case (error or success).
     * Check error code provided by error().
     * @sa error, errorString
     */
    void finished();

    /**
     * Emitted if command execution is failed.
     * @param code error code
     * @sa errorString
     */
    void error(GoogleDrive::Command::Error code);

protected:
    Session* session() const;

    void emitStarted();
    void emitSuccess();
    void emitError(Error code, const QString& msg);
    void tryAutoDelete();

    bool checkJsonReplyError(const QVariantMap&);
    bool parseJsonReply(QNetworkReply* reply, QVariantMap&);
    bool checkInvalidReply(QNetworkReply* reply);

protected:
    CommandPrivate* const d_ptr;
    Command(CommandPrivate* dd, Session* session);

private:
    void init();

    Q_DECLARE_PRIVATE(Command)
};

}

#endif
