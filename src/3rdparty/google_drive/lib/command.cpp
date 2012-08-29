#include "command.h"

#include <QtDebug>

#include "session.h"

namespace GoogleDrive
{

Command::Command(Session* session)
    : QObject(session)
    , session_(session)
{
}

void Command::throwError(const QString &msg)
{
    emit error(msg);
    qDebug() << metaObject()->className() << "error:" << msg;
}

}
