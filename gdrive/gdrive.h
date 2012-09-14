
#ifndef GDOCS_GDOCS_H
#define GDOCS_GDOCS_H

#include <memory>
#include <QObject>

class GDrive : public QObject {
    Q_OBJECT
public:
    explicit GDrive(QObject* parent = 0);
    virtual ~GDrive();
    
public slots:
    void list();
    
private slots:
    void init();
    bool refreshToken();
    void finish();
    void error(QString message);
    
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif
