#include <iostream>
#include <string>
#include <getopt.h>
#include "../../System/include/ServerDaemon.h"

struct CommandLineOptions {
    bool start = false;
    bool stop = false;
    bool status = false;
    bool help = false;
};

void showUsage(const char* programName) {
    std::cout << "Radio Control Server Daemon" << std::endl;
    std::cout << "Usage: " << programName << " [OPTION]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -s, --start    Start the daemon" << std::endl;
    std::cout << "  -t, --stop     Stop the daemon" << std::endl;
    std::cout << "  -S, --status   Check daemon status" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
}

bool parseCommandLine(int argc, char* argv[], CommandLineOptions& options) {
    static struct option longOptions[] = {
        {"start", no_argument, 0, 's'},
        {"stop", no_argument, 0, 't'},
        {"status", no_argument, 0, 'S'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    const char* shortOptions = "stSh";
    
    int optionIndex = 0;
    int c;
    
    optind = 0;
    opterr = 0;
    
    while ((c = getopt_long(argc, argv, shortOptions, longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 's':
                options.start = true;
                break;
            case 't':
                options.stop = true;
                break;
            case 'S':
                options.status = true;
                break;
            case 'h':
                options.help = true;
                break;
            case '?':
                std::cerr << "Unknown option: " << argv[optind - 1] << std::endl;
                return false;
            default:
                std::cerr << "Failed to parse command line arguments" << std::endl;
                return false;
        }
    }
    
    if (optind < argc) {
        std::cerr << "Unexpected argument: " << argv[optind] << std::endl;
        return false;
    }
    
    int actionCount = options.start + options.stop + options.status;
    if (actionCount > 1) {
        std::cerr << "Error: Multiple actions specified. Choose only one of --start, --stop, or --status." << std::endl;
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    CommandLineOptions options;
    
    if (!parseCommandLine(argc, argv, options)) {
        showUsage(argv[0]);
        return -1;
    }
    
    if (options.help) {
        showUsage(argv[0]);
        return 0;
    }
    
    if (!options.start && !options.stop && !options.status) {
        std::cerr << "Error: No action specified." << std::endl;
        showUsage(argv[0]);
        return -1;
    }
    
    ServerDaemon daemon;
    
    if (options.start) {
        std::cout << "Starting Radio Control Server Daemon..." << std::endl;
        if (daemon.start()) {
            std::cout << "Daemon started successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to start daemon" << std::endl;
            return -1;
        }
    }
    else if (options.stop) {
        std::cout << "Stopping Radio Control Server Daemon..." << std::endl;
        if (daemon.stop()) {
            std::cout << "Daemon stopped successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to stop daemon" << std::endl;
            return -1;
        }
    }
    else if (options.status) {
        if (daemon.status()) {
            return 0;
        } else {
            return -1;
        }
    }
    
    return -1;
}