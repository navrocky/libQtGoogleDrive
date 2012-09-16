#ifndef GOOGLE_DRIVE_COMMAND_GET_H
#define GOOGLE_DRIVE_COMMAND_GET_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandGetPrivate;

/**
 * @brief Gets a file's metadata by ID.
 */
class CommandGet : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandGet(Session* session);

    QString fields() const;
    void setFields(const QString& v);

    const FileInfo& resultFileInfo() const;

public slots:
    void exec(const QString& id);

protected:
    void reexecuteQuery();

private slots:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandGet)
};

}

#endif
