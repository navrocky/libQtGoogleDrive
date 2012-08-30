#include "session.h"

#include <QNetworkAccessManager>

namespace GoogleDrive
{

Session::Session(QNetworkAccessManager *manager, QObject *parent)
    : QObject(parent)
    , manager_(manager)
{
}

QNetworkAccessManager *Session::networkManager() const
{
    return manager_;
}


}
