/**
 * @file main.cpp
 * @brief Main entry point for Radio Control System (Server/Client)
 * 
 * @mainpage Control System Documentation
 *
 * @section intro_sec Introduction
 * 
 * The Radio Control System is a client-server application for managing
 * radio equipment parameters with real-time monitoring and alarm capabilities.
 * 
 * The system includes functionality for:
 * - Parameter setting (SET) with validation
 * - Parameter reading (GET) in real-time
 * - Status monitoring and alarm generation (ALARM)
 * - Client-server communication via shared memory
 * 
 * @section features_sec Main Features
 *
 * - Output power control (0-10 dBm)
 * - Frequency setting (25.0-26.0 MHz with 0.1 MHz step)
 * - Automatic and manual modulation control
 * - Temperature, input and output power monitoring
 * - Multi-level alarm system (Warning, Error, Critical)
 * - 90-second server inactivity timeout
 * - Shared memory IPC for client-server communication
 *
 * @section arch_sec Architecture
 *
 * The system consists of the following main components:
 *
 * @subsection data_sec SystemData
 * Central class for storing all system parameters and status data.
 *
 * @subsection system_sec System  
 * Base class providing common functionality for all components.
 *
 * @subsection set_sec SET
 * Class for setting system parameters with input data validation.
 *
 * @subsection get_sec GET
 * Class for reading current system parameters.
 *
 * @subsection alarm_sec ALARM
 * Class for system status monitoring and alarm generation.
 *
 * @subsection server_sec Server
 * Server process that handles client commands via shared memory.
 *
 * @subsection client_sec Client  
 * Client process for sending commands to the server.
 *
 * @section usage_sec Usage
 *
 * Start the server:
 * @code{.bash}
 * ./RadioControlSystem server
 * @endcode
 *
 * Start the client:
 * @code{.bash}
 * ./RadioControlSystem client
 * @endcode
 *
 * @section comm_sec Communication Protocol
 *
 * Client and server communicate using:
 * - Shared memory for data exchange
 * - POSIX semaphores for synchronization
 * - 90-second inactivity timeout on server
 * - Automatic resource cleanup on shutdown
 *
 * @section alarm_levels_sec Alarm Levels
 *
 * - Warning: Warnings about approaching critical values
 * - Error: Errors when exceeding safe ranges  
 * - Critical: Critical errors requiring immediate intervention
 */

#include <iostream>
#include <string>
#include "System/include/Server.h"
#include "System/include/Client.h"

/**
 * @brief Main function for Radio Control System
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Exit status (0 = success, 1 = error)
 * 
 * @par Usage:
 * - To start server: `./RadioControlSystem server`
 * - To start client: `./RadioControlSystem client`
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Radio Control System - Server/Client Application" << std::endl;
        std::cout << "Usage: " << argv[0] << " [server|client]" << std::endl;
        std::cout << "\nExamples:" << std::endl;
        std::cout << "  " << argv[0] << " server    - Start the control server" << std::endl;
        std::cout << "  " << argv[0] << " client    - Start the control client" << std::endl;
        std::cout << "\nNote: Server has 90-second inactivity timeout" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    
    if (mode == "server") {
        Server server;
        server.run();
    }
    else if (mode == "client") {
        Client client;
        client.run();
    }
    else {
        std::cout << "Error: Invalid mode. Use 'server' or 'client'" << std::endl;
        return 1;
    }

    return 0;
}