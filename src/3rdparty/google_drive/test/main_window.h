#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QNetworkAccessManager>

#include "../lib/session.h"
#include "../lib/command_oauth2.h"

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
    void showOptionsDialog();
    void login();
    void authFinished();
    void error(const QString& msg);
    
private:
    Ui::MainWindow *ui;
    QNetworkAccessManager* manager_;
    QPointer<GoogleDrive::Session> session_;
    QPointer<GoogleDrive::CommandOAuth2> oauth2_;
};

#endif // MAIN_WINDOW_H
