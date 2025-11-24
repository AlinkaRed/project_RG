#include <iostream>
#include <string>
#include "../../System/include/ServerDaemon.h"

void showUsage(const char* programName) {
    std::cout << "Radio Control Server Daemon" << std::endl;
    std::cout << "Usage: " << programName << " [OPTION]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --start    Start the daemon" << std::endl;
    std::cout << "  --stop     Stop the daemon" << std::endl;
    std::cout << "  --status   Check daemon status" << std::endl;
    std::cout << "  --help     Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        showUsage(argv[0]);
        return 1;
    }
    
    std::string option = argv[1];
    ServerDaemon daemon;
    
    if (option == "--start") {
        std::cout << "Starting Radio Control Server Daemon..." << std::endl;
        if (daemon.start()) {
            std::cout << "Daemon started successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to start daemon" << std::endl;
            return 1;
        }
    }
    else if (option == "--stop") {
        std::cout << "Stopping Radio Control Server Daemon..." << std::endl;
        if (daemon.stop()) {
            std::cout << "Daemon stopped successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to stop daemon" << std::endl;
            return 1;
        }
    }
    else if (option == "--status") {
        if (daemon.status()) {
            return 0;
        } else {
            return 1;
        }
    }
    else if (option == "--help") {
        showUsage(argv[0]);
        return 0;
    }
    else {
        std::cerr << "Unknown option: " << option << std::endl;
        showUsage(argv[0]);
        return 1;
    }
}