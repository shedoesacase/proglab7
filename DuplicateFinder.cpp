#include "DuplicateFinder.hpp"


DuplicateFinder::DuplicateFinder(const CommandLineParser::ProgramOptions& config) : config_(config), hasher_(config.hash){};

std::vector<uint8_t> DuplicateFinder::get_block_hash(const FileScanner::FileInfo& info,size_t blockIndex) {
    auto& entry = cache_[info.path.string()];
    const size_t blockSize = config_.blockSize;

    if (entry.blocksCount == 0) {
        entry.blocksCount = (info.size + blockSize - 1) / blockSize;
        entry.blockHashes.resize(entry.blocksCount);
    }

    if (entry.blockHashes[blockIndex].has_value())
        return entry.blockHashes[blockIndex].value();

    std::ifstream file(info.path, std::ios::binary);
    file.seekg(static_cast<std::streamoff>(blockIndex * blockSize));

    std::vector<char> buf(blockSize, 0);
    file.read(buf.data(), blockSize);
    std::streamsize n = file.gcount();

    if (n < static_cast<std::streamsize>(blockSize))
        std::fill(buf.begin() + n, buf.end(), 0);

    std::vector<uint8_t> h = hasher_.hash_block(buf.data(), blockSize);

    entry.blockHashes[blockIndex] = h;
    return h;
}


bool DuplicateFinder::Comparison_in_pairs(
    const FileScanner::FileInfo& f1,
    const FileScanner::FileInfo& f2)
{
    const size_t blockSize = config_.blockSize;

    size_t blocksCount =
        (f1.size + blockSize - 1) / blockSize;

    for (size_t block = 0; block < blocksCount; block++) {

        std::vector<uint8_t> h1 = get_block_hash(f1, block);
        std::vector<uint8_t> h2 = get_block_hash(f2, block);

        if (h1 != h2)
            return false;
    }

    return true;
}






std::vector<std::vector<fs::path>> DuplicateFinder::DupFinder(std::vector<FileScanner::FileInfo> files){
    std::vector<std::vector<fs::path>> result;
    if (files.size() < 2) return result;
    size_t start = 0;
    for (size_t i = 1; i <= files.size(); ++i) {
        bool group_end = (i == files.size()) || (files[i].size != files[start].size);
        if (group_end) {
            size_t group_size = i - start;
            if (group_size >= 2) {
                std::vector<bool> used(group_size, false);
                const auto* group_ptr = files.data() + start;
                for (size_t a = 0; a < group_size; ++a) {
                    if (used[a]) continue;
                    std::vector<fs::path> dup_group;
                    dup_group.push_back(group_ptr[a].path);
                    used[a] = true;
                    for (size_t b = a + 1; b < group_size; ++b) {
                        if (!used[b] && Comparison_in_pairs(group_ptr[a], group_ptr[b])) {
                            dup_group.push_back(group_ptr[b].path);
                            used[b] = true;
                        }
                    }
                    if (dup_group.size() > 1) {
                        result.push_back(std::move(dup_group));
                    }
                }
            }
            start = i;
        }
    }

    return result;
}