#pragma once

#include <iostream>
#include <string>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <thread>
#include <atomic>
#include <chrono>

#include "SharedData.h"
#include "Alarm.h"
#include "MONITOR.h"

#include "../../Protocol/include/Set.h"
#include "../../Protocol/include/Get.h"

class Server {
private:
    SystemData shared_data;
    SET set_system;
    GET get_system;
    ALARM alarm_system;
    MONITOR monitor_system;
    
    sem_t* sem_client;
    sem_t* sem_server;
    int shm_fd;
    SharedData* data;
    
    // Monitoring thread
    std::thread monitoring_thread;
    std::atomic<bool> monitoring_running;
    
    void initializeSharedMemory();
    std::string executeSET(const std::string& parameter, const std::string& value);
    std::string executeGET(const std::string& parameter);
    std::string executeALARM();
    std::string executeMONITOR(const std::string& command);
    std::string executeSTATUS();
    
    // Monitoring thread function
    void monitoringLoop();
    
public:
    Server();
    ~Server();
    
    void processCommand(const std::string& command);
    void run();
    void cleanup();
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoringRunning() const { return monitoring_running; }
};