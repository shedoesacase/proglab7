#include "CommandLineParser.hpp"


namespace po = boost::program_options;

CommandLineParser::ProgramOptions CommandLineParser::parse(int argc, char** argv){
    CommandLineParser::ProgramOptions config;
    po::variables_map vm;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "show help")
        ("dirs", po::value<std::vector<std::string>>()->multitoken(), "select dirs")
        ("exclude", po::value<std::vector<std::string>>()->multitoken(), "exclude dirs")
        ("depth", po::value<int>()->default_value(0), "max depth")
        ("min-size", po::value<size_t>()->default_value(2), "min size of file")
        ("mask", po::value<std::vector<std::string>>()->multitoken(), "masks")
        ("block-size", po::value<size_t>()->default_value(4096), "block size")
        ("hash", po::value<std::string>()->default_value("crc32"), "hash algorithm");
    
    try{
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);
    } catch (const std::exception& e){
        std::cerr << "parsing error: " << e.what() << std::endl;
        throw;
    }

    if(vm.count("help")){
        std::cout << desc << std::endl;
        exit(0);
    }

    if(vm.count("dirs")){
        config.dirs = vm["dirs"].as<std::vector<std::string>>();
    }

    if(config.dirs.empty()){
        throw std::invalid_argument("dir is empty");
    }

    if(vm.count("exclude")){
        config.exclude = vm["exclude"].as<std::vector<std::string>>();
    }

    if(vm.count("depth")){
        config.depth = vm["depth"].as<int>();
    }

    if(config.depth < 0){
        throw std::invalid_argument("depth < 0");
    }

    if(vm.count("mask")){
        config.masks = vm["mask"].as<std::vector<std::string>>();
        for(auto& mask : config.masks){
            std::transform(mask.begin(), mask.end(), mask.begin(),
             [](unsigned char c) {
                return std::tolower(c);
             });
        }
    }

    config.minSize = vm["min-size"].as<size_t>();
    if(config.minSize < 2){
        throw std::invalid_argument("min size < 2");
    }

    config.blockSize = vm["block-size"].as<size_t>();
    if(config.blockSize <= 0){
        throw std::invalid_argument("blockSize > 0");
    }
    config.hash = vm["hash"].as<std::string>();
    if(config.hash != "crc32" && config.hash != "md5"){
        throw std::invalid_argument("crc32 or md5 only");
    }

    return config;
}