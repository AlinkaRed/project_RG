/**
 * @file SharedData.h
 * @brief Shared memory structure and constants for client-server communication
 * 
 * @ingroup CommunicationClasses
 */

#pragma once

/**
 * @brief Structure for shared memory data exchange between client and server
 */
struct SharedData {
    char command[256];    ///< Command from client to server
    char response[256];   ///< Response from server to client
};

// External declarations for shared memory and semaphore constants
extern const char* SHM_NAME;         ///< Shared memory name
extern const char* SEM_CLIENT_NAME;  ///< Client semaphore name  
extern const char* SEM_SERVER_NAME;  ///< Server semaphore name