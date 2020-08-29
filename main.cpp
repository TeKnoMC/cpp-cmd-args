#include <iostream>
#include <string>
#include <map>

#include "args.hpp"

int main(int argc, char* argv[])
{
    Command cmd = Command("test");
    cmd.AddArgument("-t", "--test", "def", "A test parameter");

    ArgumentsParser parser = ArgumentsParser();
    parser.AddCommand(cmd);

    auto nameMapPair {parser.ParseArguments(argc, argv)};

    std::string name {std::get<0>(nameMapPair)};
    std::map<std::string, std::string> arguments {std::get<1>(nameMapPair)};

    if (name == "test")
    {
        std::cout << arguments["-t"] << std::endl;
    }

    return 0;
}