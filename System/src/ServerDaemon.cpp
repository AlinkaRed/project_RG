#include "../include/ServerDaemon.h"
#include <chrono>
#include <thread>
#include <csignal>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

static ServerDaemon* currentDaemonInstance = nullptr;

ServerDaemon::ServerDaemon() 
    : DaemonBase("/tmp/radio_server.pid", "radio_server") {
    currentDaemonInstance = this;
}

ServerDaemon::~ServerDaemon() {
    cleanup();
    currentDaemonInstance = nullptr;
}

void ServerDaemon::signalHandlerWrapper(int signum) {
    switch(signum) {
        case SIGTERM:
        case SIGINT:
            if (currentDaemonInstance) {
                syslog(LOG_INFO, "ServerDaemon stopping via signal %d", signum);
                currentDaemonInstance->requestStop();
            }
            break;
        case SIGHUP:
            syslog(LOG_INFO, "ServerDaemon received SIGHUP");
            break;
    }
}

void ServerDaemon::mainLoop() {
    syslog(LOG_INFO, "Radio Control Server Daemon starting main loop");
    
    struct sigaction sa;
    sa.sa_handler = ServerDaemon::signalHandlerWrapper;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGHUP, &sa, nullptr);
    
    int fd_stdout = open("/tmp/radio_server_stdout.log", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    int fd_stderr = open("/tmp/radio_server_stderr.log", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    
    if (fd_stdout != -1 && fd_stderr != -1) {
        dup2(fd_stdout, STDOUT_FILENO);
        dup2(fd_stderr, STDERR_FILENO);
    }
    
    std::cout << "=== Radio Server Starting ===" << std::endl;
    
    while (isDaemonRunning()) {
        try {
            std::cout << "Creating new Server instance..." << std::endl;
            Server server;
            std::cout << "Server instance created, calling run()..." << std::endl;
            server.run();
            std::cout << "Server run() completed" << std::endl;
            
            if (isDaemonRunning()) {
                std::cout << "Restarting server in 5 seconds..." << std::endl;
                
                if (waitWithInterrupt(5)) {
                    std::cout << "Restart cancelled - daemon is stopping" << std::endl;
                    break;
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Server exception: " << e.what() << std::endl;
            syslog(LOG_ERR, "Server exception: %s", e.what());
            
            if (isDaemonRunning()) {
                std::cout << "Restarting server after exception in 5 seconds..." << std::endl;
                
                if (waitWithInterrupt(5)) {
                    std::cout << "Restart cancelled after exception - daemon is stopping" << std::endl;
                    break;
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