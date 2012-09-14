#include "session.h"

#include <QPointer>

#include <QNetworkAccessManager>

namespace GoogleDrive
{

class SessionPrivate
{
public:
    QPointer<QNetworkAccessManager> manager;
    QString clientId;
    QString clientSecret;
    QString accessToken;
    QString refreshToken;
};

Session::Session(QNetworkAccessManager* manager, QObject* parent)
    : QObject(parent)
    , d_ptr(new SessionPrivate)
{
    Q_D(Session);
    d->manager = manager;
}

Session::~Session()
{
    delete d_ptr;
}

QNetworkAccessManager *Session::networkManager() const
{
    Q_D(const Session);
    return d->manager;
}

QString Session::clientId() const
{
    Q_D(const Session);
    return d->clientId;
}

void Session::setClientId(const QString &v)
{
    Q_D(Session);
    d->clientId = v;
}

QString Session::clientSecret() const
{
    Q_D(const Session);
    return d->clientSecret;
}

void Session::setClientSecret(const QString &v)
{
    Q_D(Session);
    d->clientSecret = v;
}

QString Session::accessToken() const
{
    Q_D(const Session);
    return d->accessToken;
}

void Session::setAccessToken(const QString &v)
{
    Q_D(Session);
    d->accessToken = v;
}

QString Session::refreshToken() const
{
    Q_D(const Session);
    return d->refreshToken;
}

void Session::setRefreshToken(const QString &v)
{
    Q_D(Session);
    d->refreshToken = v;
}

}
