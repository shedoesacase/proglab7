#pragma once
#include <boost/filesystem.hpp>
#include "CommandLineParser.hpp"
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace fs = boost::filesystem;

class FileScanner{
    public:
        struct FileInfo{
            fs::path path;
            uint64_t size;
        };

        FileScanner(const CommandLineParser::ProgramOptions& config);
        std::vector<FileInfo> scan();
    private:
        const CommandLineParser::ProgramOptions& config_;
        std::vector<fs::path> exclude_paths_;
        bool maskcheck(const std::string& filename) const;
        std::vector<fs::path> convert_to_absolute_paths(const std::vector<std::string>& dir_strings);
        bool is_excluded(const fs::path& path) const;
        void scan_directory(const fs::path& cur_path, int cur_depth, std::vector<FileInfo>& result);
};