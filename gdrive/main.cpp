
#include <iostream>
#include <boost/bind.hpp>

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
	std::string filename;
	std::string format;
    
    try
    {
        options_description desc("Available options");
        desc.add_options()
            ("help", "Produce help message")
            ("debug,d", "Produce debug messages")
            ("ls", value<std::string>(&path), "list of files")
			("get", value<std::string>(&path), "download file")
			("output", value<std::string>(&filename), "output filename")
			("format", value<std::string>(&format), "export format")
			("formats", value<std::string>(&path), "list of formats")
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

		if (vm.count("help"))
        {
			show_help(desc);
            return EXIT_SUCCESS;
		}
		
        QCoreApplication app(argc, argv);
        
        app.setOrganizationName("prog-org-ru-developers");
        app.setApplicationName("gdrive-cli");
        
        gdrive cli;
        finilizer f;
        QObject::connect(&cli, SIGNAL(finished(int)), &f, SLOT(exit(int)));

        if (vm.count("ls"))
        {
            const QString qpath = path.c_str();
			cli.delay(boost::bind(&gdrive::list, &cli, qpath));
        }
        else if (vm.count("formats"))
        {
            const QString qpath = path.c_str();
			cli.delay(boost::bind(&gdrive::formats, &cli, qpath));
        }        
        else if (vm.count("get"))
        {
            const QString qpath = path.c_str();
			const QString output = filename.c_str();
			const QString qformat = format.c_str();
			cli.delay(boost::bind(&gdrive::get, &cli, qpath, output, qformat));
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








