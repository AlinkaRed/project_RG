#pragma once
#include "System.h"

/**
 * @brief Class for reading system parameters
 * 
 * @ingroup CommandClasses
 * 
 * GET provides functionality for reading current values
 * of all system parameters. Supports reading both configurable
 * parameters and real-time status parameters.
 * 
 * Return values are formatted as strings for a unified interface.
 */
class GET : public System {
public:
    /**
     * @brief GET constructor
     * 
     * @param system_data Reference to shared system data
     */
    GET(SystemData& system_data);

    /**
     * @brief Executes system parameter reading
     * 
     * @param parameter Name of parameter to read
     * @return std::string Parameter value in string format
     * 
     * @retval "on"/"off" for boolean parameters
     * @retval "auto" for modulation in automatic mode  
     * @retval numeric values in string format
     * @retval "Error: Unknown parameter" for incorrect requests
     */
    std::string execute(const std::string& parameter) const;
};