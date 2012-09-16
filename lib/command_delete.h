#ifndef GOOGLE_DRIVE_COMMAND_DELETE_H
#define GOOGLE_DRIVE_COMMAND_DELETE_H

#include "authorized_command.h"

namespace GoogleDrive
{

class CommandDeletePrivate;

/**
 * @brief Delete file
 * Permanently deletes a file by ID. Skips the trash.
 */
class CommandDelete : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandDelete(Session* session);

public slots:
    /**
     * Execute command.
     * @param id file ID which will be deleted.
     */
    void exec(const QString& id);

protected:
    void reexecuteQuery();

private slots:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandDelete)
};

}

#endif
