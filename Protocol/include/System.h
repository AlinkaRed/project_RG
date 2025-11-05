#pragma once
#include <string>
#include <functional>
#include <random>
#include "SystemData.h"

/**
 * @brief Base class for system management
 * 
 * @ingroup CoreClasses
 * 
 * System provides common functionality for all system classes:
 * - Input data validation
 * - Parameter simulation updates
 * - Alarm handling through callback functions
 * - Access to shared system data
 * 
 * All specialized classes (SET, GET, ALARM) inherit from System
 * and use its basic functionality.
 */
class System {
protected:
    SystemData& data;  ///< Reference to shared system data
    
    /**
     * @brief Callback function for alarm handling
     * 
     * Function called when alarms of any level occur.
     * Can be set by user for custom notification handling.
     */
    std::function<void(const std::string&)> alarm_callback_;

    /**
     * @brief Checks validity of numeric string
     * 
     * @param str String to check
     * @return true if string contains valid number (integer or decimal)
     * @return false if string contains invalid characters
     * 
     * Allowed:
     * - Digits (0-9)
     * - Minus sign at the beginning
     * - One decimal point
     */
    bool isValidNumber(const std::string& str);
    
    /**
     * @brief Checks validity of boolean string
     * 
     * @param str String to check
     * @return true if string contains "true", "false", "1" or "0"
     * @return false for any other values
     */
    bool isValidBoolean(const std::string& str);

    /**
     * @brief Updates system simulation data
     * 
     * Generates random values for:
     * - Temperature (-50°C to +120°C)
     * - Real output power (-5 dBm to 15 dBm) 
     * - Input power (-35 dBm to 5 dBm)
     * 
     * Used to emulate real equipment operation.
     */
    void updateSimulation();

public:
    /**
     * @brief System constructor
     * 
     * @param system_data Reference to shared system data
     */
    System(SystemData& system_data);

    /**
     * @brief Sets callback function for alarm handling
     * 
     * @param callback Function to be called when alarms occur
     * 
     * @note Callback should accept std::string (alarm message)
     * and return void
     */
    void setAlarmCallback(const std::function<void(const std::string&)>& callback);

protected:
    /**
     * @brief Calls alarm callback function
     * 
     * @param message Alarm message text
     * 
     * If callback is not set, message is ignored.
     */
    void triggerAlarm(const std::string& message);
};