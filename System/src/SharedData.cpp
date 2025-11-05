/**
 * @file SharedData.cpp
 * @brief Definitions of shared memory constants for client-server communication
 */

#include "../include/SharedData.h"

// Define the constants here to avoid multiple definitions
const char* SHM_NAME = "/radio_control_memory";
const char* SEM_CLIENT_NAME = "/sem_radio_client";
const char* SEM_SERVER_NAME = "/sem_radio_server";