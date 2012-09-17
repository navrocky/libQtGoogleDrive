
#include <iostream>
#include <boost/program_options.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QMetaType>

#include "gdrive.h"
#include "tools.h"

using namespace boost::program_options;

void show_help(const options_description& desc)
{
    std::cout << "\nGoogle Drive CLI " << "\n\n";
    std::cout << desc << "\n";
}

typedef variables_map* variables_map_ptr;

Q_DECLARE_METATYPE(variables_map_ptr);
// qRegisterMetaType<variables_map>("variables_map");

int main(int argc, char** argv)
{
    std::string path;
    
    try
    {
        options_description desc("Available options");
        desc.add_options()
            ("help", "Produce help message")
            ("debug,d", "Produce debug messages")
            ("ls", value<std::string>(&path), "list of files")
        ;

        variables_map vm;
        try
        {
            store(parse_command_line(argc, argv, desc), vm);
        }
        catch(const std::exception& e)
        {
            std::cerr << "\nError: " << e.what() << std::endl;
            show_help(desc);
            return EXIT_FAILURE;
        }
        notify(vm);

 
        QCoreApplication app(argc, argv);
        
        app.setOrganizationName("prog-org-ru-developers");
        app.setApplicationName("gdrive-cli");
        
        gdrive cli;
        finilizer f;
        QObject::connect(&cli, SIGNAL(finished(int)), &f, SLOT(exit(int)));
        
        if (vm.count("ls"))
        {
            const QString path = QString::fromLocal8Bit(vm["ls"].as<std::string>().c_str());
            if (!QMetaObject::invokeMethod(&cli, "list", Qt::QueuedConnection, Q_ARG(QString, path)))
                throw std::runtime_error("can't find method list");
        }
        else
        {
            show_help(desc);
            return EXIT_SUCCESS;
        }
        
        return app.exec();

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}








