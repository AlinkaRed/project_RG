/**
 * @file Client.cpp
 * @brief Implementation of Client class for radio control system
 */

#include "../include/Client.h"
#include <thread>
#include <chrono>

/**
 * @brief Constructs a new Client object
 */
Client::Client() : sem_client(nullptr), sem_server(nullptr), shm_fd(-1), data(nullptr) {
}

/**
 * @brief Destroys the Client object and cleans up resources
 */
Client::~Client() {
    cleanup();
}

/**
 * @brief Initializes shared memory connection to server
 * @return true if initialization successful, false otherwise
 */
bool Client::initializeSharedMemory() {
    sem_client = sem_open(SEM_CLIENT_NAME, 0);
    sem_server = sem_open(SEM_SERVER_NAME, 0);

    if (sem_client == SEM_FAILED || sem_server == SEM_FAILED) {
        std::cerr << "Error: Server is not running!" << std::endl;
        return false;
    }

    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error: Cannot open shared memory!" << std::endl;
        return false;
    }

    data = static_cast<SharedData*>(
        mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        
    if (data == MAP_FAILED) {
        std::cerr << "Error: Cannot map shared memory!" << std::endl;
        return false;
    }
    
    return true;
}

/**
 * @brief Displays help information about available commands
 */
void Client::showHelp() {
    std::cout << "\n=== Radio Control System Commands ===" << std::endl;
    std::cout << "Note: Server has 90-second inactivity timeout" << std::endl;
    std::cout << "\nSET commands:" << std::endl;
    std::cout << "  SET nominal_output_power <0-10>    - Set output power in dBm" << std::endl;
    std::cout << "  SET frequency <25.0-26.0>          - Set frequency in MHz (step 0.1)" << std::endl;
    std::cout << "  SET automatic_modulation <on/off>  - Enable/disable auto modulation" << std::endl;
    std::cout << "  SET modulation <on/off>            - Set modulation (only if auto is off)" << std::endl;
    std::cout << "\nGET commands:" << std::endl;
    std::cout << "  GET nominal_output_power           - Get nominal power" << std::endl;
    std::cout << "  GET frequency                      - Get frequency" << std::endl;
    std::cout << "  GET automatic_modulation           - Get auto modulation status" << std::endl;
    std::cout << "  GET modulation                     - Get modulation status" << std::endl;
    std::cout << "  GET temp                           - Get temperature" << std::endl;
    std::cout << "  GET real_output_power              - Get real output power" << std::endl;
    std::cout << "  GET input_power                    - Get input power" << std::endl;
    std::cout << "\nOther commands:" << std::endl;
    std::cout << "  ALARM                              - Check system alarms" << std::endl;
    std::cout << "  STATUS                             - Get full system status" << std::endl;
    std::cout << "  HELP                               - Show this help message" << std::endl;
    std::cout << "  EXIT                               - Exit client" << std::endl;
    std::cout << "=====================================" << std::endl;
}

/**
 * @brief Main client execution loop
 * 
 * Provides interactive command interface for communicating with
 * the radio control server.
 */
void Client::run() {
    if (!initializeSharedMemory()) {
        return;
    }

    std::cout << "Radio Control Client connected to server." << std::endl;
    std::cout << "Server will timeout after 90 seconds of inactivity." << std::endl;
    showHelp();

    while (true) {
        std::cout << "\nEnter command: ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "EXIT" || cmd == "exit" || cmd == "quit") {
            break;
        }
        
        if (cmd == "HELP" || cmd == "help") {
            showHelp();
            continue;
        }

        if (cmd.empty()) {
            continue;
        }

        strncpy(data->command, cmd.c_str(), sizeof(data->command) - 1);
        data->command[sizeof(data->command) - 1] = '\0';

        sem_post(sem_client);

        std::cout << "Waiting for server response..." << std::endl;
        
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
            std::cerr << "Error getting current time" << std::endl;
            continue;
        }
        ts.tv_sec += 5;
        
        if (sem_timedwait(sem_server, &ts) == 0) {
            std::cout << "\n=== Server Response ===" << std::endl;
            std::cout << data->response << std::endl;
            std::cout << "=======================" << std::endl;
        } else {
            std::cout << "Error: Server response timeout (server may be inactive)" << std::endl;
            break;
        }
    }

    std::cout << "Client disconnected." << std::endl;
}

/**
 * @brief Cleans up shared memory and semaphores
 */
void Client::cleanup() {
    if (data != MAP_FAILED) {
        munmap(data, sizeof(SharedData));
    }
    if (shm_fd != -1) {
        close(shm_fd);
    }
    if (sem_client != SEM_FAILED) {
        sem_close(sem_client);
    }
    if (sem_server != SEM_FAILED) {
        sem_close(sem_server);
    }
}