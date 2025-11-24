#pragma once
#include <string>
#include <functional>
#include <csignal>
#include <syslog.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

class DaemonBase {
protected:
    static volatile sig_atomic_t isRunning;
    std::string pidFile;
    std::string logIdent;
    
    virtual void mainLoop() = 0;
    virtual void cleanup() = 0;
    
public:
    DaemonBase(const std::string& pidFilePath, const std::string& logIdentifier);
    virtual ~DaemonBase();
    
    bool start();
    bool stop();
    bool status() const;
    
    static void signalHandler(int signum);
};