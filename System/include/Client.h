/**
 * @file Client.h
 * @brief Client class for sending commands to radio control server
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
#include "SharedData.h"

/**
 * @brief Client class for communicating with radio control server
 * 
 * The Client class provides an interactive interface for sending commands
 * to the radio control server via shared memory and semaphores.
 */
class Client {
private:
    sem_t *sem_client;    ///< Client notification semaphore
    sem_t *sem_server;    ///< Server response semaphore
    int shm_fd;           ///< Shared memory file descriptor
    SharedData* data;     ///< Pointer to shared memory data

public:
    /**
     * @brief Constructs a new Client object
     */
    Client();
    
    /**
     * @brief Destroys the Client object and cleans up resources
     */
    ~Client();
    
    /**
     * @brief Main client execution loop
     * 
     * Provides interactive command interface for communicating with
     * the radio control server.
     */
    void run();
    
private:
    /**
     * @brief Initializes shared memory connection to server
     * @return true if initialization successful, false otherwise
     */
    bool initializeSharedMemory();
    
    /**
     * @brief Cleans up shared memory and semaphores
     */
    void cleanup();
    
    /**
     * @brief Displays help information about available commands
     */
    void showHelp();
};