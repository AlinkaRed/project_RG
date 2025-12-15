#include "../include/MONITOR.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

/**
 * @brief MONITOR constructor
 */
MONITOR::MONITOR(SystemData& system_data) : System(system_data) {}

/**
 * @brief Executes monitoring command
 */
std::string MONITOR::execute(const std::string& command) {
    std::stringstream ss(command);
    std::string action, param1, param2;
    ss >> action >> param1 >> param2;
    
    if (action == "STATUS") {
        return handleStatus();
    }
    else if (action == "SENSORS") {
        return handleSensors();
    }
    else if (action == "ALARMS") {
        return handleAlarms();
    }
    else if (action == "CONFIG") {
        if (param1 == "GET") {
            return handleConfigGet(param2);
        }
        else if (param1 == "SET") {
            std::string value;
            std::getline(ss, value);
            if (!value.empty() && value[0] == ' ') value = value.substr(1);
            return handleConfigSet(param2, value) ? 
                   "SUCCESS: Parameter set" : "ERROR: Failed to set parameter";
        }
    }
    else if (action == "ALARM") {
        if (param1 == "ACK") {
            return handleAlarmAck(param2) ? 
                   "SUCCESS: Alarm acknowledged" : "ERROR: Alarm not found";
        }
    }
    else if (action == "SERVICE") {
        return handleService(param1) ? 
               "SUCCESS: Service state changed" : "ERROR: Invalid state (use on/off)";
    }
    else if (action == "UPDATE") {
        return handleUpdate();
    }
    else if (action == "CHECK") {
        return handleCheck();
    }
    else if (action == "CLEAR") {
        return handleClear();
    }
    
    return "ERROR: Unknown MONITOR command. Use: STATUS, SENSORS, ALARMS, CONFIG, ALARM ACK, SERVICE, UPDATE, CHECK, CLEAR";
}

std::string MONITOR::handleStatus() const {
    std::stringstream ss;
    
    auto now = std::chrono::system_clock::now();
    auto last_update_duration = std::chrono::duration_cast<std::chrono::seconds>(
        now - data.monitoring.last_update).count();
    
    ss << "Monitoring System Status:\n"
       << "=======================\n"
       << "Service: " << (data.monitoring.service_enabled ? "ENABLED" : "DISABLED") << "\n"
       << "Polling Interval: " << data.monitoring.polling_interval_ms << " ms\n"
       << "Last Update: " << last_update_duration << " seconds ago\n"
       << "Total Updates: " << data.monitoring.total_sensor_updates << "\n"
       << "Total Alarms: " << data.monitoring.total_alarms_triggered << "\n"
       << "Active Alarms: " << data.monitoring.active_alarms.size() << "\n"
       << "\nSensor Monitoring:\n"
       << "Temperature: " << (data.monitoring.temp_config.monitor ? "ON" : "OFF") << "\n"
       << "Current: " << (data.monitoring.current_config.monitor ? "ON" : "OFF") << "\n"
       << "Power: " << (data.monitoring.power_config.monitor ? "ON" : "OFF") << "\n"
       << "Voltage: " << (data.monitoring.voltage_config.monitor ? "ON" : "OFF");
    
    return ss.str();
}

std::string MONITOR::handleSensors() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "Current Sensor Values:\n"
       << "======================\n"
       << "Temperature: " << data.monitoring.temperature << " °C\n"
       << "  Range: [" << data.monitoring.temp_config.min_value 
       << ", " << data.monitoring.temp_config.max_value << "]\n"
       << "  Monitoring: " << (data.monitoring.temp_config.monitor ? "ON" : "OFF") << "\n"
       << "\nCurrent: " << data.monitoring.current << " A\n"
       << "  Range: [" << data.monitoring.current_config.min_value 
       << ", " << data.monitoring.current_config.max_value << "]\n"
       << "  Monitoring: " << (data.monitoring.current_config.monitor ? "ON" : "OFF") << "\n"
       << "\nPower: " << data.monitoring.power << " W\n"
       << "  Range: [" << data.monitoring.power_config.min_value 
       << ", " << data.monitoring.power_config.max_value << "]\n"
       << "  Monitoring: " << (data.monitoring.power_config.monitor ? "ON" : "OFF") << "\n"
       << "\nVoltage: " << data.monitoring.voltage << " V\n"
       << "  Range: [" << data.monitoring.voltage_config.min_value 
       << ", " << data.monitoring.voltage_config.max_value << "]\n"
       << "  Monitoring: " << (data.monitoring.voltage_config.monitor ? "ON" : "OFF");
    
    return ss.str();
}

std::string MONITOR::handleAlarms() const {
    auto alarms = data.getActiveAlarms();
    
    if (alarms.empty()) {
        return "No active alarms";
    }
    
    std::stringstream ss;
    ss << "Active Alarms (" << alarms.size() << "):\n"
       << "================\n";
    
    for (const auto& alarm : alarms) {
        auto time_t = std::chrono::system_clock::to_time_t(alarm.timestamp);
        std::tm tm = *std::localtime(&time_t);
        char time_buf[64];
        std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm);
        
        ss << "ID: " << alarm.id << "\n"
           << "  Sensor: " << alarm.sensor << "\n"
           << "  Severity: " << alarm.severity << "\n"
           << "  Message: " << alarm.message << "\n"
           << "  Value: " << alarm.value << " (Threshold: " << alarm.threshold << ")\n"
           << "  Time: " << time_buf << "\n"
           << "  Acknowledged: " << (alarm.acknowledged ? "YES" : "NO") << "\n"
           << "  ------------------\n";
    }
    
    return ss.str();
}

std::string MONITOR::handleConfigGet(const std::string& param) const {
    if (param.empty()) {
        return "ERROR: No parameter specified";
    }
    
    std::stringstream ss;
    
    if (param == "service_enabled") {
        ss << data.monitoring.service_enabled;
    }
    else if (param == "polling_interval") {
        ss << data.monitoring.polling_interval_ms;
    }
    else if (param == "monitor_temperature") {
        ss << data.monitoring.temp_config.monitor;
    }
    else if (param == "monitor_current") {
        ss << data.monitoring.current_config.monitor;
    }
    else if (param == "monitor_power") {
        ss << data.monitoring.power_config.monitor;
    }
    else if (param == "monitor_voltage") {
        ss << data.monitoring.voltage_config.monitor;
    }
    else if (param == "temperature_range") {
        ss << data.monitoring.temp_config.min_value << "," << data.monitoring.temp_config.max_value;
    }
    else if (param == "current_range") {
        ss << data.monitoring.current_config.min_value << "," << data.monitoring.current_config.max_value;
    }
    else if (param == "power_range") {
        ss << data.monitoring.power_config.min_value << "," << data.monitoring.power_config.max_value;
    }
    else if (param == "voltage_range") {
        ss << data.monitoring.voltage_config.min_value << "," << data.monitoring.voltage_config.max_value;
    }
    else {
        return "ERROR: Unknown parameter";
    }
    
    return "SUCCESS: " + param + " = " + ss.str();
}

bool MONITOR::handleConfigSet(const std::string& param, const std::string& value_str) {
    if (param.empty() || value_str.empty()) {
        return false;
    }
    
    // Handle different parameter types
    if (param == "service_enabled") {
        if (value_str == "true" || value_str == "1" || value_str == "on") {
            data.monitoring.service_enabled = true;
            return true;
        } else if (value_str == "false" || value_str == "0" || value_str == "off") {
            data.monitoring.service_enabled = false;
            return true;
        }
        return false;
    }
    else if (param == "polling_interval") {
        try {
            int value = std::stoi(value_str);
            if (value > 0) {
                data.monitoring.polling_interval_ms = value;
                return true;
            }
        } catch (...) {
            return false;
        }
    }
    else if (param == "monitor_temperature") {
        if (value_str == "true" || value_str == "1" || value_str == "on") {
            data.monitoring.temp_config.monitor = true;
            return true;
        } else if (value_str == "false" || value_str == "0" || value_str == "off") {
            data.monitoring.temp_config.monitor = false;
            return true;
        }
        return false;
    }
    else if (param == "monitor_current") {
        if (value_str == "true" || value_str == "1" || value_str == "on") {
            data.monitoring.current_config.monitor = true;
            return true;
        } else if (value_str == "false" || value_str == "0" || value_str == "off") {
            data.monitoring.current_config.monitor = false;
            return true;
        }
        return false;
    }
    else if (param == "monitor_power") {
        if (value_str == "true" || value_str == "1" || value_str == "on") {
            data.monitoring.power_config.monitor = true;
            return true;
        } else if (value_str == "false" || value_str == "0" || value_str == "off") {
            data.monitoring.power_config.monitor = false;
            return true;
        }
        return false;
    }
    else if (param == "monitor_voltage") {
        if (value_str == "true" || value_str == "1" || value_str == "on") {
            data.monitoring.voltage_config.monitor = true;
            return true;
        } else if (value_str == "false" || value_str == "0" || value_str == "off") {
            data.monitoring.voltage_config.monitor = false;
            return true;
        }
        return false;
    }
    
    return false;
}

bool MONITOR::handleAlarmAck(const std::string& alarm_id) {
    if (alarm_id.empty()) {
        return false;
    }
    
    return data.acknowledgeAlarm(alarm_id);
}

bool MONITOR::handleService(const std::string& state) {
    if (state == "on" || state == "true" || state == "1") {
        data.monitoring.service_enabled = true;
        return true;
    }
    else if (state == "off" || state == "false" || state == "0") {
        data.monitoring.service_enabled = false;
        return true;
    }
    
    return false;
}

std::string MONITOR::handleUpdate() {
    data.updateMonitoringSensors();
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Sensors updated:\n"
       << "  Temperature: " << data.monitoring.temperature << " °C\n"
       << "  Current: " << data.monitoring.current << " A\n"
       << "  Power: " << data.monitoring.power << " W\n"
       << "  Voltage: " << data.monitoring.voltage << " V";
    
    return ss.str();
}

std::string MONITOR::handleCheck() {
    data.checkMonitoringThresholds();
    
    auto alarms = data.getActiveAlarms();
    std::stringstream ss;
    ss << "Threshold check completed.\n"
       << "Active alarms: " << alarms.size();
    
    return ss.str();
}

std::string MONITOR::handleClear() {
    data.clearAcknowledgedAlarms();
    
    auto alarms = data.getActiveAlarms();
    return "Acknowledged alarms cleared. Remaining active alarms: " + 
           std::to_string(alarms.size());
}