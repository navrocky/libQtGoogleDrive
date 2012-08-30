#ifndef GOOGLE_DRIVE_SESSION_H
#define GOOGLE_DRIVE_SESSION_H

#include <QObject>
#include <QPointer>

class QNetworkAccessManager;

namespace GoogleDrive
{

class Session : public QObject
{
    Q_OBJECT
public:
    Session(QNetworkAccessManager* manager, QObject* parent = 0);

    QNetworkAccessManager* networkManager() const;

    QString accessToken() const {return accessToken_;}
    void setAccessToken(const QString& v) {accessToken_ = v;}

    QString refreshToken() const {return refreshToken_;}
    void setRefreshToken(const QString& v) {refreshToken_ = v;}

private:
    QPointer<QNetworkAccessManager> manager_;

    QString accessToken_;
    QString refreshToken_;
};

}

#endif
