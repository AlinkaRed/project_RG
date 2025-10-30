/**
 * @file Server.h
 * @brief Server class for handling radio control system commands
 * 
 * @ingroup CommunicationClasses
 */

#pragma once
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>
#include <ctime>
#include <sstream>
#include "Protocol/Set.h"
#include "Protocol/Get.h"
#include "Alarm.h"
#include "SharedData.h"

/**
 * @brief Server class for processing radio control commands via shared memory
 * 
 * The Server class handles incoming commands from clients through shared memory
 * and semaphores. It processes SET, GET, ALARM, and STATUS commands for the
 * radio control system with a 90-second inactivity timeout.
 */
class Server {
private:
    SystemData shared_data;   ///< Shared system data storage
    SET set_system;           ///< SET command processor
    GET get_system;           ///< GET command processor  
    ALARM alarm_system;       ///< ALARM monitoring system
    
    sem_t *sem_client;        ///< Client notification semaphore
    sem_t *sem_server;        ///< Server response semaphore
    int shm_fd;               ///< Shared memory file descriptor
    SharedData* data;         ///< Pointer to shared memory data

public:
    /**
     * @brief Constructs a new Server object
     */
    Server();
    
    /**
     * @brief Destroys the Server object and cleans up resources
     */
    ~Server();
    
    /**
     * @brief Main server execution loop
     * 
     * Starts the server to listen for client commands with a 90-second
     * inactivity timeout that resets after each received command.
     */
    void run();
    
private:
    /**
     * @brief Initializes shared memory and semaphores for IPC
     */
    void initializeSharedMemory();
    
    /**
     * @brief Cleans up shared memory and semaphores
     */
    void cleanup();
    
    /**
     * @brief Processes incoming command from client
     * @param command Command string to process
     */
    void processCommand(const std::string& command);
    
    /**
     * @brief Executes SET command with parameter validation
     * @param parameter Parameter name to set
     * @param value Value to set
     * @return Response message
     */
    std::string executeSET(const std::string& parameter, const std::string& value);
    
    /**
     * @brief Executes GET command to retrieve parameter value
     * @param parameter Parameter name to get
     * @return Response message with parameter value
     */
    std::string executeGET(const std::string& parameter);
    
    /**
     * @brief Executes ALARM command to check system status
     * @return Response message
     */
    std::string executeALARM();
};