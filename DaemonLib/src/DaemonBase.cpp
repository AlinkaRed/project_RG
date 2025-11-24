#include "../include/DaemonBase.h"
#include <sys/stat.h>
#include <signal.h>
#include <cstring>

volatile sig_atomic_t DaemonBase::isRunning = 0;

DaemonBase::DaemonBase(const std::string& pidFilePath, const std::string& logIdentifier) 
    : pidFile(pidFilePath), logIdent(logIdentifier) {
}

DaemonBase::~DaemonBase() {
}

bool DaemonBase::start() {
    std::ifstream pidf(pidFile);
    if (pidf.good()) {
        pid_t existingPid;
        pidf >> existingPid;
        pidf.close();
        
        if (kill(existingPid, 0) == 0) {
            std::cerr << "Daemon is already running with PID: " << existingPid << std::endl;
            return false;
        } else {
            std::cout << "Removing stale PID file for process " << existingPid << std::endl;
            remove(pidFile.c_str());
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "First fork failed" << std::endl;
        return false;
    }
    if (pid > 0) {
        return true;
    }

    if (setsid() < 0) {
        std::cerr << "setsid failed" << std::endl;
        return false;
    }

    pid = fork();
    if (pid < 0) {
        std::cerr << "Second fork failed" << std::endl;
        return false;
    }
    if (pid > 0) {
        return true;
    }

    std::ofstream pidFileStream(pidFile);
    if (!pidFileStream) {
        std::cerr << "Cannot create PID file: " << pidFile << std::endl;
        return false;
    }
    pidFileStream << getpid();
    pidFileStream.close();

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalHandler);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    openlog(logIdent.c_str(), LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started successfully");

    isRunning = 1;

    sleep(2);

    mainLoop();

    remove(pidFile.c_str());
    syslog(LOG_INFO, "Daemon stopped");
    closelog();

    return true;
}

bool DaemonBase::stop() {
    std::ifstream pidf(pidFile);
    if (!pidf.good()) {
        std::cerr << "PID file not found. Is daemon running?" << std::endl;
        return false;
    }
    
    pid_t pid;
    pidf >> pid;
    pidf.close();
    
    if (kill(pid, SIGTERM) == 0) {
        std::cout << "Stop signal sent to process " << pid << std::endl;
        
        int count = 0;
        while (kill(pid, 0) == 0 && count < 10) {
            sleep(1);
            count++;
        }
        
        if (kill(pid, 0) != 0) {
            remove(pidFile.c_str());
            std::cout << "Daemon stopped successfully" << std::endl;
            return true;
        } else {
            std::cerr << "Daemon did not stop gracefully, forcing..." << std::endl;
            kill(pid, SIGKILL);
            remove(pidFile.c_str());
            return true;
        }
    } else {
        std::cerr << "Failed to send stop signal to process " << pid << std::endl;
        if (kill(pid, 0) != 0) {
            remove(pidFile.c_str());
        }
        return false;
    }
}

bool DaemonBase::status() const {
    std::ifstream pidf(pidFile);
    if (!pidf.good()) {
        std::cout << "Daemon is not running" << std::endl;
        return false;
    }
    
    pid_t pid;
    pidf >> pid;
    pidf.close();
    
    if (kill(pid, 0) == 0) {
        std::cout << "Daemon is running with PID: " << pid << std::endl;
        return true;
    } else {
        std::cout << "PID file exists but process is not running" << std::endl;
        remove(pidFile.c_str());
        return false;
    }
}

void DaemonBase::signalHandler(int signum) {
    switch(signum) {
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "Daemon stopping via signal %d", signum);
            isRunning = 0;
            break;
        case SIGHUP:
            syslog(LOG_INFO, "Daemon received SIGHUP");
            break;
    }
}