#pragma once
#include <string>
#include <functional>
#include <csignal>
#include <syslog.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

class DaemonBase {
private:
    std::atomic<bool> isRunning_;
    std::mutex stopMutex_;
    std::condition_variable stopCondition_;
    
    std::string pidFile;
    std::string logIdent;
    
    static void signalHandler(int signum);
    
protected:
    virtual void mainLoop() = 0;
    virtual void cleanup() {}
    
    bool shouldStop() const { return !isRunning_.load(); }
    bool isDaemonRunning() const { return isRunning_.load(); }
    void requestStop() { 
        isRunning_.store(false);
        stopCondition_.notify_all();
    }
    
    bool waitWithInterrupt(unsigned int seconds) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        return !stopCondition_.wait_for(lock, 
            std::chrono::seconds(seconds),
            [this]() { return shouldStop(); }
        );
    }
    
public:
    DaemonBase(const std::string& pidFilePath, const std::string& logIdentifier);
    virtual ~DaemonBase();
    
    bool start();
    bool stop();
    bool status() const;
};