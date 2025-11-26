#include "FileScanner.hpp"

FileScanner::FileScanner(const CommandLineParser::ProgramOptions& config) : config_(config) {
    exclude_paths_ = convert_to_absolute_paths(config.exclude);
}

std::vector<fs::path> FileScanner::convert_to_absolute_paths(const std::vector<std::string>& dir_strings){
std::vector<fs::path> result;
for(const std::string& dir_dir : dir_strings){
    if(dir_dir.empty()) continue;
    try{
        fs::path input_path(dir_dir);
        fs::path absolute_path = fs::absolute(input_path);
        result.push_back(absolute_path); 
    } 
    catch (const fs::filesystem_error& e){
        std::cerr << "Filesystem error: " << dir_dir << e.what() << std::endl;
    } 
    catch (const std::exception& e){
        std::cerr << "Error processing path " << e.what() << std::endl;
    }
}

return result;
}

bool match_mask(const std::string& filename, const std::string& mask) {
    size_t i = 0;
    size_t j = 0;
    size_t star_pos = std::string::npos;
    size_t i_backup = 0;

    while (i < filename.size()) {
        if (j < mask.size() && (mask[j] == filename[i] || mask[j] == '?')) {
            i++;
            j++;
        } else if (j < mask.size() && mask[j] == '*') {
            star_pos = j;
            i_backup = i;
            j++;
        } else if (star_pos != std::string::npos) {
            j = star_pos + 1;
            i_backup++;
            i = i_backup;
        } else {
            return false;
        }
    }
    while (j < mask.size() && mask[j] == '*') j++;

    return j == mask.size();
}

bool FileScanner::maskcheck(const std::string& filename) const {
    if (config_.masks.empty()) return true;
        std::string lower_filename = filename;
        std::transform(lower_filename.begin(), lower_filename.end(), lower_filename.begin(),
        [](unsigned char c) { return std::tolower(c); });
    for (const auto& mask : config_.masks) {
        if (match_mask(lower_filename, mask)) return true;
    }
    return false;
}

bool FileScanner::is_excluded(const fs::path& path) const {
    auto abs = fs::absolute(path).lexically_normal();
    for (const auto &ex : exclude_paths_) {
        if (ex.lexically_normal() == abs) return true;
    }
    return false;
}

void FileScanner::scan_directory(const fs::path& cur_path, int cur_depth, std::vector<FileInfo>& result) {
    if(cur_depth > config_.depth){
        return;
    }

    try {
        for(auto& entry : fs::directory_iterator(cur_path)){
            try {
                if(is_excluded(entry.path())) continue;

                if (fs::is_directory(entry)){
                    scan_directory(entry.path(), cur_depth + 1, result);
                    continue;
                }

                uint64_t filesize = 0;
                try {
                    filesize = fs::file_size(entry.path());
                } catch (const fs::filesystem_error&) {
                    continue; 
                }

                if(filesize < config_.minSize) continue;

                fs::path entrypath = entry.path();
                std::string filename = entrypath.filename().string();
                if(maskcheck(filename)) 
                    result.push_back(FileInfo{entry.path(), filesize});
            } catch (const fs::filesystem_error&) {
                continue;
            }
        }
    } catch (const fs::filesystem_error&) {
        return;
    }
}

std::vector<FileScanner::FileInfo> FileScanner::scan(){
    auto dirs = convert_to_absolute_paths(config_.dirs);
    std::vector<FileInfo> result;

    for(auto base_path : dirs){
        int cur_depth = 0;
        scan_directory(base_path, cur_depth, result);
    }

    std::sort(result.begin(), result.end(),
    [](const FileScanner::FileInfo& a, const FileScanner::FileInfo& b) {
        return a.size < b.size;});

    std::unordered_map<uint64_t, int> count;

    for (const auto& x : result)
        count[x.size]++;

    result.erase(
        std::remove_if(result.begin(), result.end(),
        [&](const FileInfo& x) { return count[x.size] < 2; }),
        result.end());
    return result;
}