#include "../include/Server.h"
#include <thread>
#include <chrono>
#include <syslog.h>

Server::Server() 
    : set_system(shared_data), get_system(shared_data), 
      alarm_system(shared_data), monitor_system(shared_data),
      monitoring_running(false) { 
    
    std::cout << "Server constructor called" << std::endl;
    
    auto alarm_callback = [](const std::string& alarm_msg) {
        std::cout << "[ALARM] " << alarm_msg << std::endl;
    };
    
    set_system.setAlarmCallback(alarm_callback);
    get_system.setAlarmCallback(alarm_callback);
    alarm_system.setAlarmCallback(alarm_callback);
    monitor_system.setAlarmCallback(alarm_callback);
    
    initializeSharedMemory();
    
    startMonitoring();
}

Server::~Server() {
    stopMonitoring();
    cleanup();
}

/**
 * @brief Initializes shared memory and semaphores for IPC
 */
void Server::initializeSharedMemory() {
    std::cout << "Initializing shared memory..." << std::endl;
    
    sem_unlink(SEM_CLIENT_NAME);
    sem_unlink(SEM_SERVER_NAME);
    shm_unlink(SHM_NAME);
    
    std::cout << "Creating semaphores..." << std::endl;
    sem_client = sem_open(SEM_CLIENT_NAME, O_CREAT | O_EXCL, 0644, 0);
    sem_server = sem_open(SEM_SERVER_NAME, O_CREAT | O_EXCL, 0644, 0);
    
    if (sem_client == SEM_FAILED) {
        std::cerr << "Failed to create sem_client: " << strerror(errno) << std::endl;
        return;
    } else {
        std::cout << "sem_client created successfully" << std::endl;
    }
    
    if (sem_server == SEM_FAILED) {
        std::cerr << "Failed to create sem_server: " << strerror(errno) << std::endl;
        return;
    } else {
        std::cout << "sem_server created successfully" << std::endl;
    }

    std::cout << "Creating shared memory..." << std::endl;
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory: " << strerror(errno) << std::endl;
        return;
    } else {
        std::cout << "Shared memory created successfully" << std::endl;
    }
    
    ftruncate(shm_fd, sizeof(SharedData));
    data = static_cast<SharedData*>(
        mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        
    if (data == MAP_FAILED) {
        std::cerr << "Failed to map shared memory: " << strerror(errno) << std::endl;
        return;
    } else {
        std::cout << "Shared memory mapped successfully" << std::endl;
    }
    
    memset(data, 0, sizeof(SharedData));
    std::cout << "Shared memory initialized successfully" << std::endl;
}

/**
 * @brief Executes SET command with parameter validation
 */
std::string Server::executeSET(const std::string& parameter, const std::string& value) {
    bool success = set_system.execute(parameter, value);
    if (success) {
        return "SUCCESS: Parameter " + parameter + " set to " + value;
    } else {
        return "ERROR: Failed to set " + parameter + " to " + value;
    }
}

/**
 * @brief Executes GET command to retrieve parameter value
 */
std::string Server::executeGET(const std::string& parameter) {
    std::string result = get_system.execute(parameter);
    if (result.find("Error: Unknown parameter") != std::string::npos) {
        return "ERROR: Unknown parameter " + parameter;
    }
    return "SUCCESS: " + parameter + " = " + result;
}

/**
 * @brief Executes ALARM command to check system status
 */
std::string Server::executeALARM() {
    alarm_system.updateAndCheckAlarms();
    return "SUCCESS: Alarm check completed";
}

/**
 * @brief Executes MONITOR command
 */
std::string Server::executeMONITOR(const std::string& command) {
    return monitor_system.execute(command);
}

/**
 * @brief Executes STATUS command
 */
std::string Server::executeSTATUS() {
    std::stringstream status;
    status << "SYSTEM STATUS:\n"
           << "================\n"
           << "Radio System:\n"
           << "  Nominal Power: " << get_system.execute("nominal_output_power") << " dBm\n"
           << "  Frequency: " << get_system.execute("frequency") << " MHz\n"
           << "  Auto Modulation: " << get_system.execute("automatic_modulation") << "\n"
           << "  Modulation: " << get_system.execute("modulation") << "\n"
           << "  Temperature: " << get_system.execute("temp") << " C\n"
           << "  Real Power: " << get_system.execute("real_output_power") << " dBm\n"
           << "  Input Power: " << get_system.execute("input_power") << " dBm\n"
           << "\nMonitoring System:\n"
           << "  Service: " << (monitoring_running ? "RUNNING" : "STOPPED") << "\n"
           << "  Enabled: " << (shared_data.monitoring.service_enabled ? "YES" : "NO") << "\n"
           << "  Active Alarms: " << shared_data.monitoring.active_alarms.size() << "\n"
           << "  Last Update: " << shared_data.monitoring.total_sensor_updates << " updates";
    
    return status.str();
}

/**
 * @brief Processes incoming command from client
 */
void Server::processCommand(const std::string& command) {
    std::cout << "Processing command: " << command << std::endl;
    
    std::stringstream ss(command);
    std::string action, parameter, value;
    ss >> action;
    
    // Handle MONITOR commands (they can have multiple words)
    if (action == "MONITOR") {
        std::string monitor_cmd;
        std::getline(ss, monitor_cmd);
        if (!monitor_cmd.empty() && monitor_cmd[0] == ' ') {
            monitor_cmd = monitor_cmd.substr(1);
        }
        
        std::string response = executeMONITOR(monitor_cmd);
        strncpy(data->response, response.c_str(), sizeof(data->response) - 1);
        data->response[sizeof(data->response) - 1] = '\0';
        return;
    }
    
    // For other commands, parse normally
    ss >> parameter;
    std::getline(ss, value);
    if (!value.empty() && value[0] == ' ') value = value.substr(1);
    
    std::string response;
    
    if (action == "SET" && !parameter.empty() && !value.empty()) {
        response = executeSET(parameter, value);
    }
    else if (action == "GET" && !parameter.empty()) {
        response = executeGET(parameter);
    }
    else if (action == "ALARM") {
        response = executeALARM();
    }
    else if (action == "STATUS") {
        response = executeSTATUS();
    }
    else {
        response = "ERROR: Invalid command format. Use: SET <param> <value>, GET <param>, ALARM, MONITOR <command>, or STATUS";
    }
    
    strncpy(data->response, response.c_str(), sizeof(data->response) - 1);
    data->response[sizeof(data->response) - 1] = '\0';
}

/**
 * @brief Monitoring thread function
 */
void Server::monitoringLoop() {
    syslog(LOG_INFO, "Monitoring thread started");
    
    while (monitoring_running) {
        if (shared_data.monitoring.service_enabled) {
            // Update sensors
            shared_data.updateMonitoringSensors();
            
            // Check thresholds
            shared_data.checkMonitoringThresholds();
            
            // Update shared memory
            data->monitoring.temperature = shared_data.monitoring.temperature;
            data->monitoring.current = shared_data.monitoring.current;
            data->monitoring.power = shared_data.monitoring.power;
            data->monitoring.voltage = shared_data.monitoring.voltage;
            data->monitoring.active_alarms_count = shared_data.monitoring.active_alarms.size();
            data->monitoring.service_enabled = shared_data.monitoring.service_enabled;
            
            // Update last update time
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::strftime(data->monitoring.last_update, sizeof(data->monitoring.last_update),
                         "%H:%M:%S", std::localtime(&now_time));
        }
        
        // Sleep for polling interval
        std::this_thread::sleep_for(
            std::chrono::milliseconds(shared_data.monitoring.polling_interval_ms)
        );
    }
    
    syslog(LOG_INFO, "Monitoring thread stopped");
}

void Server::startMonitoring() {
    if (!monitoring_running) {
        monitoring_running = true;
        monitoring_thread = std::thread(&Server::monitoringLoop, this);
        std::cout << "Monitoring service started" << std::endl;
    }
}

void Server::stopMonitoring() {
    if (monitoring_running) {
        monitoring_running = false;
        if (monitoring_thread.joinable()) {
            monitoring_thread.join();
        }
        std::cout << "Monitoring service stopped" << std::endl;
    }
}

/**
 * @brief Main server loop - waits for client commands with 90-second timeout
 */
void Server::run() {
    syslog(LOG_INFO, "Server run method started");

    std::cout << "Radio Control Server started..." << std::endl;
    std::cout << "Monitoring service: " << (monitoring_running ? "RUNNING" : "STOPPED") << std::endl;
    std::cout << "Server will automatically shutdown after 90 seconds of inactivity." << std::endl;

    time_t startTime = time(nullptr);
    bool commandReceived = false;

    while (time(nullptr) - startTime < 90) {
        if (sem_trywait(sem_client) == 0) {
            commandReceived = true;
            
            processCommand(data->command);
            
            sem_post(sem_server);

            std::cout << "Response sent to client. Waiting for next command..." << std::endl;
            
            startTime = time(nullptr);
            
            sleep(1);
        } 
        else 
        {
            sleep(1);
        }
    }

    if (!commandReceived) {
        std::cout << "90-second timeout! No commands received." << std::endl;
    } else {
        std::cout << "90-second inactivity timeout reached. Server shutting down." << std::endl;
    }

    std::cout << "Server shutdown completed." << std::endl;
    syslog(LOG_INFO, "Server run method completed");
}

/**
 * @brief Cleans up shared memory and semaphores
 */
void Server::cleanup() {
    if (data != MAP_FAILED) {
        munmap(data, sizeof(SharedData));
    }
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
    
    if (sem_client != SEM_FAILED) {
        sem_close(sem_client);
        sem_unlink(SEM_CLIENT_NAME);
    }
    if (sem_server != SEM_FAILED) {
        sem_close(sem_server);
        sem_unlink(SEM_SERVER_NAME);
    }
}