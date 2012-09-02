#ifndef GOOGLE_DRIVE_COMMAND_REFRESH_TOKEN_H
#define GOOGLE_DRIVE_COMMAND_REFRESH_TOKEN_H

#include "command.h"

namespace GoogleDrive
{

/*!
\internal
\brief Internal command.

This command refreshing access token using refresh token.

\attention You shouldn't use this command directly.
*/
class CommandRefreshToken : public Command
{
    Q_OBJECT
public:
    CommandRefreshToken(Session* session);

public slots:
    void exec();

signals:
    void finished();

private slots:
    void queryFinished();
};

}

#endif
