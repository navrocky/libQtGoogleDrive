#ifndef GOOGLE_DRIVE_COMMAND_H
#define GOOGLE_DRIVE_COMMAND_H

#include <QObject>
#include <QVariant>

class QNetworkRequest;
class QNetworkReply;

namespace GoogleDrive
{

class Session;

/*! \brief Base class for all GAPI commands.


*/
class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(Session* session);

    bool autoDelete() const {return autoDelete_;}
    void setAutoDelete(bool v) {autoDelete_ = v;}

signals:
    void error(QString msg);

protected:
    Session* session() const {return session_;}

    void emitStarted();
    void emitFinished();
    void emitError(const QString& msg);
    void tryAutoDelete();

    bool checkJsonReplyError(const QVariantMap&);
    bool parseJsonReply(QNetworkReply* reply, QVariantMap&);
    bool checkInvalidReply(QNetworkReply* reply);

private:
    Session* session_;
    bool autoDelete_;
};

}

#endif
