
#ifndef GDOCS_GDOCS_H
#define GDOCS_GDOCS_H

#include <memory>
#include <boost/program_options.hpp>
#include <QObject>

class gdrive : public QObject {
    Q_OBJECT
public:
    
    typedef boost::program_options::variables_map variables_map;
    
    explicit gdrive(QObject* parent = 0);
    virtual ~gdrive();
    
signals:
    void finished(int);
    
public slots:
    void list(const variables_map& vars = variables_map());
    
private slots:
    void init();
    bool refreshToken();
    void finish();
    
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif
