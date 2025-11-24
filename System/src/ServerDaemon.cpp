#include "../include/ServerDaemon.h"
#include <chrono>
#include <thread>

ServerDaemon::ServerDaemon() 
    : DaemonBase("/tmp/radio_server.pid", "radio_server") {
}

ServerDaemon::~ServerDaemon() {
    cleanup();
}

void ServerDaemon::mainLoop() {
    syslog(LOG_INFO, "Radio Control Server Daemon starting main loop");
    
    int fd_stdout = open("/tmp/radio_server_stdout.log", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    int fd_stderr = open("/tmp/radio_server_stderr.log", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    
    if (fd_stdout != -1 && fd_stderr != -1) {
        dup2(fd_stdout, STDOUT_FILENO);
        dup2(fd_stderr, STDERR_FILENO);
    }
    
    std::cout << "=== Radio Server Starting ===" << std::endl;
    
    while (isRunning) {
        try {
            std::cout << "Creating new Server instance..." << std::endl;
            Server server;
            std::cout << "Server instance created, calling run()..." << std::endl;
            server.run();
            std::cout << "Server run() completed" << std::endl;
            
            if (isRunning) {
                std::cout << "Restarting server in 5 seconds..." << std::endl;
                for (int i = 0; i < 5 && isRunning; i++) {
                    sleep(1);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Server exception: " << e.what() << std::endl;
            syslog(LOG_ERR, "Server exception: %s", e.what());
            if (isRunning) {
                std::cout << "Restarting server after exception in 5 seconds..." << std::endl;
                for (int i = 0; i < 5 && isRunning; i++) {
                    sleep(1);
                }
            }
        }
    }
    
    std::cout << "=== Radio Server Stopping ===" << std::endl;
    
    if (fd_stdout != -1) close(fd_stdout);
    if (fd_stderr != -1) close(fd_stderr);
    
    syslog(LOG_INFO, "Radio Control Server Daemon main loop finished");
}

void ServerDaemon::cleanup() {
    syslog(LOG_INFO, "Radio Control Server Daemon cleaning up");
}