#ifndef GOOGLE_DRIVE_COMMAND_PRIVATE_H
#define GOOGLE_DRIVE_COMMAND_PRIVATE_H

#include "command.h"

namespace GoogleDrive
{

class Session;

class CommandPrivate
{
public:
    virtual ~CommandPrivate() {}

    Session* session;
    bool autoDelete;
    Command::Error error;
    QString errorString;
};

}

#endif
