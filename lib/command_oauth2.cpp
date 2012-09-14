#include "command_oauth2.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariant>
#include <QByteArray>
#include <QStringList>
#include <QtDebug>

#include <qjson/parser.h>

#include "session.h"

namespace GoogleDrive
{

CommandOAuth2::CommandOAuth2(Session* s)
    : Command(s)
{
}

void CommandOAuth2::setScope(AccessScopes scopes)
{
    QStringList sl;
    const QString prefix("https://www.googleapis.com/auth/drive");

    if (scopes.testFlag(FileScope))
        sl << prefix + ".file";

    if (scopes.testFlag(AppsReadonyScope))
        sl << prefix + ".apps.readonly";

    if (scopes.testFlag(ReadonlyScope))
        sl << prefix + ".readonly";

    if (scopes.testFlag(ReadonlyMetadataScope))
        sl << prefix + ".readonly.metadata";

    if (scopes.testFlag(FullAccessScope))
        sl << prefix;

    scope_ = sl.join("%20");
}

QUrl CommandOAuth2::getLoginUrl() const
{
    Q_ASSERT(!scope_.isEmpty());
    if (scope_.isEmpty())
    {
        qCritical() << "Scope isn't specified";
        return QUrl();
    }

    Q_ASSERT(!session()->clientId().isEmpty());
    if (session()->clientId().isEmpty())
    {
        qCritical() << "ClientID isn't specified";
        return QUrl();
    }

    const QString responseType("code");
    const QString redirectUri("urn:ietf:wg:oauth:2.0:oob");

    return QUrl(QString ("https://accounts.google.com/o/oauth2/auth?client_id=%1&scope=%2&response_type=%3&redirect_uri=%4")
                .arg(session()->clientId())
                .arg(scope_)
                .arg(responseType)
                .arg(redirectUri));
}

void CommandOAuth2::requestAccessToken(const QString &authCode)
{
    emitStarted();
    QNetworkRequest request(QUrl("https://accounts.google.com/o/oauth2/token"));
    QString str = QString("code=%1&client_id=%2&client_secret=%3&grant_type=%4&redirect_uri=%5")
            .arg(authCode)
            .arg(session()->clientId())
            .arg(session()->clientSecret())
            .arg("authorization_code")
            .arg("urn:ietf:wg:oauth:2.0:oob");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = session()->networkManager()->post(request, str.toUtf8());
    connect (reply, SIGNAL(finished()), this, SLOT(requestAccessTokenFinished()));
    reply->setParent(this);
}

void CommandOAuth2::requestAccessTokenFinished()
{
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReply(reply))
        return;

    QVariantMap map;
    if (!parseJsonReply(reply, map))
        return;

    QString accessToken = map["access_token"].toString();
    QString refreshToken = map["refresh_token"].toString();
    if (accessToken.isEmpty() || refreshToken.isEmpty())
    {
        emitError(UnknownError, tr("Access or refresh token is empty in reply to \"%1\"")
                  .arg(reply->url().toString()));
        return;
    }

    session()->setAccessToken(accessToken);
    session()->setRefreshToken(refreshToken);

    emit finished();
    emitSuccess();
}

}
