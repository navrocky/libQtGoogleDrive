
#include <iostream>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <boost/program_options.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QMetaType>
#include <QDir>

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

static const QStringList c_commands = {
	"ls", "cat",
};


std::pair<std::string, std::string> reg_foo(const std::string& s)
{
	return std::make_pair(s, std::string());
}

typedef std::vector<std::string> Args;

int main(int argc, char** argv)
{
	const Args args = detail::cmdline(argc, argv).args;
	
	std::vector<std::string> positional_commands;
	
    std::string path;
	std::string filename;
	std::string format;
	
    
    try
    {

		positional_options_description cmd_pd;
		cmd_pd.add("command", -1);
			
        options_description main_desc("Available options");
        main_desc.add_options()
			("command", value<Args>(&positional_commands)->required()->multitoken(), c_commands.join(" | ").toLocal8Bit().constData())
            ("help", 	"Produce help message")
            ("debug,d", "Produce debug messages")
//             ("ls", value<std::string>(&path)->implicit_value("/tmp/test"), "list of files")
// 			("get", value<std::string>(&path), "download file")
// 			("output", value<std::string>(&filename), "output filename")
// 			("format", value<std::string>(&format), "export format")
// 			("formats", value<std::string>(&path), "list of formats")

        ;

		typedef std::map<QString, std::pair<positional_options_description, options_description>> Commands;
		Commands cmds;
		
		{
			auto& ls = cmds["ls"];
			positional_options_description& ls_pd = ls.first;
			options_description& ls_desc = ls.second;
			
			ls_pd.add("path", 1);

			ls_desc.add_options()
				("path", value<std::string>(&path)->default_value(QDir::rootPath().toLocal8Bit().constData()), "list of files")
				("long,l", "long format")
			;
		}

	
		std::cerr << "1"<<std::endl;
		auto args = detail::cmdline(argc, argv).args;
		
		parsed_options parsed = command_line_parser(argc, argv)
			.options(main_desc).positional(cmd_pd).allow_unregistered().run();


		std::cerr << "3" <<std::endl;
		variables_map vm;
		std::cerr << "4" <<std::endl;
		store(parsed, vm);

		std::cerr << "5" <<std::endl;


		
		if (vm.count("help"))
        {
			show_help(main_desc);
            return EXIT_SUCCESS;
		}
        
        notify(vm);



		if (vm.count("command"))
        {
			const std::string& command = positional_commands.front();
			std::cerr << "command:" << command << std::endl;

			const Args cmd_args(++std::find(args.begin(), args.end(), command), args.end());

			BOOST_FOREACH(const std::string& opt, cmd_args) {
				std::cerr << "new args:" << opt <<std::endl;
			}
			
			auto& ls = cmds["ls"];


			parsed_options parsed2 = command_line_parser(cmd_args)
				.positional(ls.first).options(ls.second).run();

			std::cerr << "parsed" << std::endl;
				
			store(parsed2, vm);

			std::cerr << "store" << std::endl;

			if (vm.count("debug"))
			{
				std::cerr << "DEBUG"<<std::endl;
			}
			if (vm.count("path"))
			{
				std::cerr << "PATH:" << vm["path"].as<std::string>() <<std::endl;
			}

			std::cerr << "end" << std::endl;
			return 0;
		}
		
		
        QCoreApplication app(argc, argv);
        
        app.setOrganizationName("prog-org-ru-developers");
        app.setApplicationName("gdrive-cli");
        
        gdrive cli;
        finilizer f;
        QObject::connect(&cli, SIGNAL(finished(int)), &f, SLOT(exit(int)));

        if (vm.count("ls"))
        {
			std::cerr << "LS"<<std::endl;
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
            show_help(main_desc);
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








