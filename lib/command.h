#ifndef GOOGLE_DRIVE_COMMAND_H
#define GOOGLE_DRIVE_COMMAND_H

#include <QObject>

namespace GoogleDrive
{

class Session;

// abstract GAPI command
class Command : public QObject
{
    Q_OBJECT
public:
    Command(Session* session);

signals:
    void error(QString msg);

protected:
    Session* session() const {return session_;}

    void throwError(const QString& msg);

private:
    Session* session_;

};

}

#endif
