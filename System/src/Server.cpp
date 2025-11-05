#include "../include/Server.h"
#include <thread>
#include <chrono>

Server::Server() 
    : set_system(shared_data), get_system(shared_data), alarm_system(shared_data) {
    
    auto alarm_callback = [](const std::string& alarm_msg) {
        std::cout << "[ALARM] " << alarm_msg << std::endl;
    };
    
    set_system.setAlarmCallback(alarm_callback);
    get_system.setAlarmCallback(alarm_callback);
    alarm_system.setAlarmCallback(alarm_callback);
    
    initializeSharedMemory();
}

Server::~Server() {
    cleanup();
}

/**
 * @brief Initializes shared memory and semaphores for IPC
 */
void Server::initializeSharedMemory() {
    sem_client = sem_open(SEM_CLIENT_NAME, O_CREAT, 0644, 0);
    sem_server = sem_open(SEM_SERVER_NAME, O_CREAT, 0644, 0);
    
    if (sem_client == SEM_FAILED || sem_server == SEM_FAILED) {
        std::cerr << "Failed to create semaphores" << std::endl;
        return;
    }

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory" << std::endl;
        return;
    }
    
    ftruncate(shm_fd, sizeof(SharedData));
    data = static_cast<SharedData*>(
        mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        
    if (data == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        return;
    }
}

/**
 * @brief Executes SET command with parameter validation
 * @param parameter Parameter name to set
 * @param value Value to set
 * @return Response message
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
 * @param parameter Parameter name to get
 * @return Response message with parameter value
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
 * @return Response message
 */
std::string Server::executeALARM() {
    alarm_system.updateAndCheckAlarms();
    return "SUCCESS: Alarm check completed";
}

/**
 * @brief Processes incoming command from client
 * @param command Command string to process
 */
void Server::processCommand(const std::string& command) {
    std::cout << "Processing command: " << command << std::endl;
    
    std::stringstream ss(command);
    std::string action, parameter, value;
    ss >> action >> parameter >> value;
    
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
        std::stringstream status;
        status << "SYSTEM STATUS:\n"
               << "Nominal Power: " << get_system.execute("nominal_output_power") << " dBm\n"
               << "Frequency: " << get_system.execute("frequency") << " MHz\n"
               << "Auto Modulation: " << get_system.execute("automatic_modulation") << "\n"
               << "Modulation: " << get_system.execute("modulation") << "\n"
               << "Temperature: " << get_system.execute("temp") << " C\n"
               << "Real Power: " << get_system.execute("real_output_power") << " dBm\n"
               << "Input Power: " << get_system.execute("input_power") << " dBm";
        response = status.str();
    }
    else {
        response = "ERROR: Invalid command format. Use: SET <param> <value>, GET <param>, ALARM, or STATUS";
    }
    
    strncpy(data->response, response.c_str(), sizeof(data->response) - 1);
    data->response[sizeof(data->response) - 1] = '\0';
}

/**
 * @brief Main server loop - waits for client commands with 90-second timeout
 * @details The server resets the 90-second timer after each client command,
 * allowing continuous operation as long as commands are received regularly
 */
void Server::run() {
    std::cout << "Radio Control Server started..." << std::endl;
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