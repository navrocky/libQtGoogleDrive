#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QSet>
#include <QNetworkAccessManager>

#include "../lib/session.h"
#include "../lib/command.h"
#include "../lib/file_info.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void started(GoogleDrive::Command* cmd);
    void finished(GoogleDrive::Command* cmd);
    void error(GoogleDrive::Command* cmd, const QString& msg);
    void reauthorizationNeeded(GoogleDrive::Command* cmd, const QString& msg);

    void showOptionsDialog();
    void login();
    void authFinished();
    void getFileList();
    void getFileListFinished(const GoogleDrive::FileInfoList& list);

    void downloadFile();
    void downloadFileFinished();
    void downloadProgress(qint64 v);
    
private:
    void writeInfo(const QString& msg, bool time = true);
    void writeHint(const QString& msg, bool time = true);
    void writeError(const QString& msg, bool time = true);
    void writeDelimiter();
    void writeText(const QString& msg);
    void writeMessage(const QString& msg, QColor col, bool time);
    void updateStatusBar();

    Ui::MainWindow *ui;
    QNetworkAccessManager* manager_;
    GoogleDrive::Session* session_;
    QSet<GoogleDrive::Command*> executingCommands_;
};

#endif // MAIN_WINDOW_H
