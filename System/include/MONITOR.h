#pragma once

#include "../../Protocol/include/System.h"

/**
 * @brief Class for monitoring system commands
 * 
 * @ingroup CommandClasses
 * 
 * MONITOR provides functionality for managing the monitoring system:
 * - Get monitoring status and statistics
 * - View sensor values
 * - Manage alarms
 * - Configure monitoring parameters
 * - Control monitoring service
 * 
 * Supported commands:
 * - MONITOR STATUS - Get monitoring system status
 * - MONITOR SENSORS - Get current sensor values
 * - MONITOR ALARMS - Get active alarms
 * - MONITOR CONFIG GET <param> - Get monitoring parameter
 * - MONITOR CONFIG SET <param> <value> - Set monitoring parameter
 * - MONITOR ALARM ACK <id> - Acknowledge alarm
 * - MONITOR SERVICE <on/off> - Enable/disable monitoring service
 * - MONITOR UPDATE - Force sensor update
 * - MONITOR CHECK - Check thresholds
 * - MONITOR CLEAR - Clear acknowledged alarms
 */
class MONITOR : public System {
public:
    /**
     * @brief MONITOR constructor
     * 
     * @param system_data Reference to shared system data
     */
    MONITOR(SystemData& system_data);

    /**
     * @brief Executes monitoring command
     * 
     * @param command Full monitoring command string
     * @return std::string Command response
     */
    std::string execute(const std::string& command);
    
private:
    /**
     * @brief Handle STATUS command
     */
    std::string handleStatus() const;
    
    /**
     * @brief Handle SENSORS command
     */
    std::string handleSensors() const;
    
    /**
     * @brief Handle ALARMS command
     */
    std::string handleAlarms() const;
    
    /**
     * @brief Handle CONFIG GET command
     */
    std::string handleConfigGet(const std::string& param) const;
    
    /**
     * @brief Handle CONFIG SET command
     */
    bool handleConfigSet(const std::string& param, const std::string& value);
    
    /**
     * @brief Handle ALARM ACK command
     */
    bool handleAlarmAck(const std::string& alarm_id);
    
    /**
     * @brief Handle SERVICE command
     */
    bool handleService(const std::string& state);
    
    /**
     * @brief Handle UPDATE command
     */
    std::string handleUpdate();
    
    /**
     * @brief Handle CHECK command
     */
    std::string handleCheck();
    
    /**
     * @brief Handle CLEAR command
     */
    std::string handleClear();
};