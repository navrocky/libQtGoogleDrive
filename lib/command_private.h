#ifndef GOOGLE_DRIVE_COMMAND_PRIVATE_H
#define GOOGLE_DRIVE_COMMAND_PRIVATE_H

#include "command.h"

class QEventLoop;

namespace GoogleDrive
{

class Session;

class CommandPrivate
{
public:
    CommandPrivate()
        : session(0)
        , autoDelete(false)
        , error(Command::NoError)
        , loop(0)
    {}

    virtual ~CommandPrivate() {}

    Session* session;
    bool autoDelete;
    Command::Error error;
    QString errorString;
    QEventLoop* loop;
};

}

#endif
