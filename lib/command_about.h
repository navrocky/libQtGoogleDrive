#ifndef GOOGLE_DRIVE_COMMAND_ABOUT_H
#define GOOGLE_DRIVE_COMMAND_ABOUT_H

#include "authorized_command.h"
#include "about_info.h"

namespace GoogleDrive
{

class CommandAboutPrivate;

/**
 * @brief Information about GD account.
 * @sa https://developers.google.com/drive/v2/reference/about/get
 */
class CommandAbout : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandAbout(Session* session);

    /**
     * Returns about information as a result of command execution.
     */
    const AboutInfo& resultInfo() const;

public slots:
    /**
     * Executing command.
     * @sa resultInfo
     */
    void exec();

protected:
    void reexecuteQuery();

private slots:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandAbout)
};

}

#endif
