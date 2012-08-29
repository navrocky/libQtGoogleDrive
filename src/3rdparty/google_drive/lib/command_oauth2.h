#ifndef GOOGLE_DRIVE_COMMAND_OAUTH2_H
#define GOOGLE_DRIVE_COMMAND_OAUTH2_H

#include <QString>
#include <QUrl>

#include "command.h"

namespace GoogleDrive
{

class CommandOAuth2 : public Command
{
    Q_OBJECT
public:
    CommandOAuth2(Session*);

    QString clientId() const {return clientId_;}
    void setClientId(const QString& v) {clientId_ = v;}

    QString clientSecret() const {return clientSecret_;}
    void setClientSecret(const QString& v) {clientSecret_ = v;}

    QUrl getLoginUrl() const;

signals:
    void finished();

public slots:
    void requestAuthToken(const QString& authCode);

private slots:
    void requestAuthTokenFinished();

private:
    QString clientId_;
    QString clientSecret_;
};

}

#endif
