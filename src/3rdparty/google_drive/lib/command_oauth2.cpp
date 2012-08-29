#include "command_oauth2.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariant>
#include <QByteArray>

#include <qjson/parser.h>

#include "session.h"

namespace GoogleDrive
{

CommandOAuth2::CommandOAuth2(Session* s)
    : Command(s)
{
}

QUrl CommandOAuth2::getLoginUrl() const
{
    const QString scope("https://www.googleapis.com/auth/drive");
    const QString responseType("code");
    const QString redirectUri("urn:ietf:wg:oauth:2.0:oob");

    return QUrl(QString ("https://accounts.google.com/o/oauth2/auth?client_id=%1&scope=%2&response_type=%3&redirect_uri=%4")
                .arg(clientId_)
                .arg(scope)
                .arg(responseType)
                .arg(redirectUri));
}

void CommandOAuth2::requestAuthToken(const QString &authCode)
{
    QNetworkRequest request(QUrl("https://accounts.google.com/o/oauth2/token"));
    QString str = QString("code=%1&client_id=%2&client_secret=%3&grant_type=%4&redirect_uri=%5")
            .arg(authCode)
            .arg(clientId_)
            .arg(clientSecret_)
            .arg("authorization_code")
            .arg("urn:ietf:wg:oauth:2.0:oob");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = session()->networkManager()->post(request, str.toUtf8());
    connect (reply, SIGNAL(finished()), this, SLOT(requestAuthTokenFinished()));
    reply->setParent(this);
}

void CommandOAuth2::requestAuthTokenFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (!reply)
        return;

    reply->deleteLater();

    QByteArray data = reply->readAll();

    bool ok = false;
    QVariant res = QJson::Parser().parse(data, &ok);
    if (!ok)
    {
        throwError(tr("Invalid json response from \"%1\"").arg(reply->url().toString()));
        return;
    }

    const QString errorKey("error");
    QVariantMap map = res.toMap();

    if (map.contains (errorKey))
    {
        throwError(tr("Error returned \"%1\" from \"%2\"")
                   .arg(map[errorKey].toString()).arg(reply->url().toString()));
        return;
    }

    const QString accessTokenKey("access_token");
    QString accessToken;
    if (map.contains(accessTokenKey))
        accessToken = map[accessTokenKey].toString();

    const QString refreshTokenKey("refresh_token");
    QString refreshToken;
    if (map.contains (refreshTokenKey))
        refreshToken = map[refreshTokenKey].toString();

    Q_ASSERT(!accessToken.isEmpty() && !refreshToken.isEmpty());

    if (accessToken.isEmpty() || refreshToken.isEmpty())
    {
        throwError(tr("Access or refresh token is empty in reply to \"%1\"")
                   .arg(reply->url().toString()));
        return;
    }

    session()->setAccessToken(accessToken);
    session()->setRefreshToken(refreshToken);

    emit finished();
}

}
