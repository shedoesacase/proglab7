#pragma once
#include <fstream>
#include <filesystem>
#include "FileScanner.hpp"
#include "Hash.hpp"
#include <optional>

struct CachedFileData {
    std::vector<std::optional<std::vector<std::uint8_t>>> blockHashes;
    size_t blocksCount = 0;
};

class DuplicateFinder{
    public:
        DuplicateFinder(const CommandLineParser::ProgramOptions& config);
        std::vector<std::vector<fs::path>> DupFinder(std::vector<FileScanner::FileInfo> scan);
        bool Comparison_in_pairs(const FileScanner::FileInfo& f1, const FileScanner::FileInfo& f2);
        std::vector<uint8_t> get_block_hash(const FileScanner::FileInfo& info,size_t blockIndex);
    private:
        const CommandLineParser::ProgramOptions& config_;
        FileHasher hasher_;
        std::unordered_map<std::string, CachedFileData> cache_;
};