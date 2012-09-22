
#include <iostream>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>

#include <boost/program_options.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QMetaType>
#include <QDir>

#include "gdrive.h"
#include "tools.h"

using namespace boost::adaptors;
using namespace boost::program_options;

void show_help(const options_description& desc)
{
    std::cout << "\nGoogle Drive CLI " << "\n\n";
    std::cout << desc << "\n";
}

typedef std::map<QString,
    std::tuple<
        positional_options_description,
        options_description,
        boost::function<void (gdrive&, const variables_map&)
        >
    >
> Commands;

Commands make_commands_desc()
{

    Commands cmds;
    
    {
        auto& cmd = cmds["ls"];
        
        std::get<0>(cmd).add("path", 1);

        std::get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
            ("long,l", "long format")
        ;
        
        std::get<2>(cmd) = boost::bind(&gdrive::list, _1, _2);
    }
    
    {
        auto& cmd = cmds["cat"];
        
        std::get<0>(cmd).add("path", 1);

        std::get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
        ;
        
        std::get<2>(cmd) = boost::bind(&gdrive::list, _1, _2);
    }
    
    {
        auto& cmd = cmds["formats"];
        
        std::get<0>(cmd).add("path", 1);

        std::get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
        ;
        
        std::get<2>(cmd) = boost::bind(&gdrive::formats, _1, _2);
    }

    return cmds;
}

static const Commands c_commands_desc = make_commands_desc();

QStringList commands()
{
    QStringList ret;
    BOOST_FOREACH(const QString& cmd, c_commands_desc | map_keys) {
        ret << cmd;
    }
    return ret;
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
        variables_map vm;
        
		positional_options_description cmd_pd;
		cmd_pd.add("command", -1);
			
        options_description main_desc("Available options");
        main_desc.add_options()
			("command", value<Args>(&positional_commands)->required()->multitoken(), commands().join(" | ").toLocal8Bit().constData())
            ("help", 	"Produce help message")
            ("debug,d", "Produce debug messages")
//             ("ls", value<std::string>(&path)->implicit_value("/tmp/test"), "list of files")
// 			("get", value<std::string>(&path), "download file")
// 			("output", value<std::string>(&filename), "output filename")
// 			("format", value<std::string>(&format), "export format")
// 			("formats", value<std::string>(&path), "list of formats")

        ;

		parsed_options parsed = command_line_parser(args)
			.options(main_desc).positional(cmd_pd).allow_unregistered().run();

        store(parsed, vm);

		if (vm.count("help") && !vm.count("command"))
        {
			show_help(main_desc);
            return EXIT_SUCCESS;
		}

        notify(vm);

        gdrive cli;
        finilizer f;
        QObject::connect(&cli, SIGNAL(finished(int)), &f, SLOT(exit(int)));    
        
        if (vm.count("command"))
        {
			const std::string& command = positional_commands.front();
			const Args cmd_args(++std::find(args.begin(), args.end(), command), args.end());

            Commands::const_iterator it = c_commands_desc.find(QString::fromLocal8Bit(command.c_str()));
            if (it == c_commands_desc.end())
                throw std::runtime_error("no such command");
            
			const auto& desc = it->second;

			parsed_options parsed = command_line_parser(cmd_args)
				.positional(std::get<0>(desc)).options(std::get<1>(desc)).allow_unregistered().run();

			store(parsed, vm);

            if (vm.count("help"))
            {
                show_help(std::get<1>(desc));
                return EXIT_SUCCESS;
            }
            
            std::get<2>(desc)(cli, vm);
		}
        else
        {
            show_help(main_desc);
            return EXIT_SUCCESS;
        }
		
		
        QCoreApplication app(argc, argv);
        
        app.setOrganizationName("prog-org-ru-developers");
        app.setApplicationName("gdrive-cli");
        



//         if (vm.count("ls"))
//         {
// 			std::cerr << "LS"<<std::endl;
//             const QString qpath = path.c_str();
// 			cli.list(vm);
//         }
//         else if (vm.count("formats"))
//         {
//             const QString qpath = path.c_str();
// 			cli.delay(boost::bind(&gdrive::formats, &cli, qpath));
//         }        
//         else if (vm.count("get"))
//         {
//             const QString qpath = path.c_str();
// 			const QString output = filename.c_str();
// 			const QString qformat = format.c_str();
// 			cli.delay(boost::bind(&gdrive::get, &cli, qpath, output, qformat));
//         }        
//         else
//         {
//             show_help(main_desc);
//             return EXIT_SUCCESS;
//         }
        
        return app.exec();

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}








