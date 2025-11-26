#pragma once
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <iostream>
#include <cctype>
#include <algorithm>

class CommandLineParser{
    public:
        struct ProgramOptions{
            std::vector<std::string> dirs;
            std::vector<std::string> exclude;
            int depth = 0;
            size_t minSize = 2;
            std::vector<std::string> masks;
            size_t blockSize = 4096;
            std::string hash;
        };
        
        ProgramOptions parse(int argc, char** argv);

};
