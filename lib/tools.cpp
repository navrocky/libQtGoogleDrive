#include "tools.h"
#include "defines.h"

namespace GoogleDrive
{
namespace Internal
{

void setRequestAccessToken(QNetworkRequest& request, const QString& accessToken)
{
    request.setRawHeader(cAuthorization, QString("Bearer %1").arg(accessToken).toLatin1());
}

}
}
