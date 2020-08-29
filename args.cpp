#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

#include "args.hpp"

class ParsingException : public std::exception
{
    protected:
        std::string suspectArg;

    public:
        ParsingException(std::string arg)
        {
            suspectArg = arg;
        }

        virtual const std::string getMsg() const throw()
        {
            return "Argument parsing error: " + suspectArg;
        }

        const char* what() const throw()
        {
            return "Error occured whilst parsing argument";
        }
};

class DuplicateArgument : public ParsingException
{
    public:
        DuplicateArgument(std::string arg)
        : ParsingException(arg)
        {
        }

        const std::string getMsg() const throw()
        {
            return "Duplicate argument: " + suspectArg;
        }
};

class MissingArgument : public ParsingException
{
    public:
        MissingArgument(std::string arg)
        : ParsingException(arg)
        {
        }
        
        const std::string getMsg() const throw()
        {
            return "Missing argument: " + suspectArg;
        }
};

class MissingValue : public ParsingException
{
    public:
        MissingValue(std::string arg)
        : ParsingException(arg)
        {
        }
        
        const std::string getMsg() const throw()
        {
            return "Missing value for argument: " + suspectArg;
        }
};

// ------------ Command object ------------
Command::Command(std::string name)
{
    Name = name;
}

void Command::AddArgument(std::string shortName, std::string longName, std::string defaultValue, std::string description)
{
    Argument arg {shortName, longName, defaultValue, description};
    
    arguments.push_back(arg);
}

std::map<std::string, std::string> Command::Parse(int argc, char* userArgsArr[])
{
    std::map<std::string, std::string> valueMap;
    int idx;

    for (Argument expectedArg : arguments)
    {
        idx = -1;

        // go through all args and look for expected argument: set idx
        for (int i {0}; i < argc; i++)
        {
            if (userArgsArr[i] == expectedArg.shortName || userArgsArr[i] == expectedArg.longName)
            {
                if (idx != -1)   // if already located (duplicate)
                {
                    throw DuplicateArgument(userArgsArr[i]);
                }
                else
                {
                    idx = i;
                }
            }
        }

        if (idx == -1)              // if not found
        {
            if (expectedArg.defaultValue == "")
            {
                throw MissingArgument(expectedArg.shortName);
            }
            else
            {
                valueMap.insert(std::make_pair(expectedArg.shortName, expectedArg.defaultValue));
            }
        }
        else if (idx + 1 >= argc)   // if no data (overruns argv size)
        {
            throw MissingValue(userArgsArr[idx]);
        }
        else
        {
            valueMap.insert(std::make_pair(expectedArg.shortName, userArgsArr[idx + 1]));
        }
        
    }

    return valueMap;
}

// ------------ ArgumentsParser object ------------

void ArgumentsParser::usage(std::string errMsg, std::string progName)
{
    std::cout << "error: " << errMsg << std::endl << std::endl;
    std::cout << "Usage: " << std::endl << progName << " [command] <arguments>" << std::endl << std::endl;

    std::cout << "Available commands:" << std::endl;
    for (Command cmd : commands)
    {
        std::cout << cmd.Name << std::endl;
        for (Argument arg : cmd.arguments)
        {
            std::cout << "\t" << std::setw(20) << std::left << arg.shortName + "/" + arg.longName << arg.description << std::endl;
        }
    }

    std::exit(1);
}

void ArgumentsParser::AddCommand(Command cmd)
{
    commands.push_back(cmd);
}

std::tuple<std::string, std::map<std::string, std::string>> ArgumentsParser::ParseArguments(int argc, char* argv[])
{
    std::string progName {argv[0]};

    if (argc == 1)
    {
        usage("Missing a command", progName);
    }

    std::string name;
    bool isParsed {false};
    std::map<std::string, std::string> valueMap;

    for (Command cmd : commands)
    {
        if (argv[1] == cmd.Name)
        {
            name = cmd.Name;
            try
            {
                valueMap = cmd.Parse(argc - 2, argv + 2);
            }
            catch(const ParsingException& e)
            {
                usage(e.getMsg(), progName);
            }
            
            isParsed = true;
            break;
        }
    }

    if (!isParsed)
    {
        std::string msg {"Unknown command: "};
        msg.append(argv[1]);
        usage(msg, progName);
    }

    return {name, valueMap};
}
