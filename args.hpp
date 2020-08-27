#ifndef ARGS_H
#define ARGS_H

#include <vector>
#include <string>
#include <map>

struct Argument
{
    std::string shortName;
    std::string longName;
    std::string defaultValue;
    std::string description;
};

class Command
{
    private:
        std::vector<Argument> arguments;

    public:
        std::string Name;

        Command(std::string name);
        void AddArgument(std::string shortName, std::string longName, std::string defaultValue, std::string description);
        std::map<std::string, std::string> Parse(int argc, char* userArgsArr[]);

};

class ArgumentsParser
{
    private:
        std::vector<Command> commands;

        void usage(std::string errMsg);

    public:
        void AddCommand(Command cmd);
        std::tuple<std::string, std::map<std::string, std::string>> ParseArguments(int argc, char* argv[]);
};

#endif