
#ifndef GDOCS_GDOCS_H
#define GDOCS_GDOCS_H

#include <memory>
#include <QObject>

class gdrive : public QObject {
    Q_OBJECT
public:
    
    explicit gdrive(QObject* parent = 0);
    virtual ~gdrive();
    
signals:
    void finished(int);
    
public slots:
    void list(const QString& path = QString());
    
private slots:
    void init();
    bool refreshToken();
    void finish();
    
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif
