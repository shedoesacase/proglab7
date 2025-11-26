#include "DuplicateFinder.hpp"
#include <locale>


int main(int argc, char** argv) {
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    try {
        CommandLineParser parser;
        auto config = parser.parse(argc, argv);
        
        FileScanner scanner(config);
        auto files = scanner.scan();
    
        
        DuplicateFinder finder(config);
        auto duplicate_groups = finder.DupFinder(files);
        long long sum = 0;
        for (size_t i = 0; i < duplicate_groups.size(); ++i) {
            const auto& group = duplicate_groups[i];
            for (const auto& path : group) {
                std::cout << fs::absolute(path).string() << std::endl;
                sum++;
            }
            
            if (i < duplicate_groups.size() - 1) {
                std::cout << std::endl;
            }
        }
        std::cout << sum;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}