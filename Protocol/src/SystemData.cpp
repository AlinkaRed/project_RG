#include "../include/SystemData.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

/**
 * @brief SystemData constructor
 * 
 * Initializes random number generator and randomly sets initial modulation state.
 */
SystemData::SystemData() : gen(rd()) {
    std::uniform_int_distribution<> dis(0, 1);
    modulation = dis(gen);
}

void SystemData::updateMonitoringSensors() {
    std::uniform_real_distribution<> uniform_dist(0.0, 1.0);
    
    // Update temperature
    if (monitoring.temp_config.enabled && monitoring.temp_config.monitor) {
        std::uniform_real_distribution<> temp_dist(monitoring.temp_config.min_value, 
                                                   monitoring.temp_config.max_value);
        monitoring.temperature = temp_dist(gen);
        
        if (uniform_dist(gen) < monitoring.temp_config.anomaly_probability) {
            if (uniform_dist(gen) < 0.5) {
                monitoring.temperature = monitoring.temp_config.min_value - 
                    (monitoring.temp_config.max_value - monitoring.temp_config.min_value) * 
                    monitoring.temp_config.anomaly_scale;
            } else {
                monitoring.temperature = monitoring.temp_config.max_value + 
                    (monitoring.temp_config.max_value - monitoring.temp_config.min_value) * 
                    monitoring.temp_config.anomaly_scale;
            }
        }
    }
    
    // Update current
    if (monitoring.current_config.enabled && monitoring.current_config.monitor) {
        std::uniform_real_distribution<> current_dist(monitoring.current_config.min_value, 
                                                      monitoring.current_config.max_value);
        monitoring.current = current_dist(gen);
        
        if (uniform_dist(gen) < monitoring.current_config.anomaly_probability) {
            if (uniform_dist(gen) < 0.5) {
                monitoring.current = monitoring.current_config.min_value - 
                    (monitoring.current_config.max_value - monitoring.current_config.min_value) * 
                    monitoring.current_config.anomaly_scale;
            } else {
                monitoring.current = monitoring.current_config.max_value + 
                    (monitoring.current_config.max_value - monitoring.current_config.min_value) * 
                    monitoring.current_config.anomaly_scale;
            }
        }
    }
    
    // Update power
    if (monitoring.power_config.enabled && monitoring.power_config.monitor) {
        std::uniform_real_distribution<> power_dist(monitoring.power_config.min_value, 
                                                    monitoring.power_config.max_value);
        monitoring.power = power_dist(gen);
        
        if (uniform_dist(gen) < monitoring.power_config.anomaly_probability) {
            if (uniform_dist(gen) < 0.5) {
                monitoring.power = monitoring.power_config.min_value - 
                    (monitoring.power_config.max_value - monitoring.power_config.min_value) * 
                    monitoring.power_config.anomaly_scale;
            } else {
                monitoring.power = monitoring.power_config.max_value + 
                    (monitoring.power_config.max_value - monitoring.power_config.min_value) * 
                    monitoring.power_config.anomaly_scale;
            }
        }
    }
    
    // Update voltage
    if (monitoring.voltage_config.enabled && monitoring.voltage_config.monitor) {
        std::uniform_real_distribution<> voltage_dist(monitoring.voltage_config.min_value, 
                                                      monitoring.voltage_config.max_value);
        monitoring.voltage = voltage_dist(gen);
        
        if (uniform_dist(gen) < monitoring.voltage_config.anomaly_probability) {
            if (uniform_dist(gen) < 0.5) {
                monitoring.voltage = monitoring.voltage_config.min_value - 
                    (monitoring.voltage_config.max_value - monitoring.voltage_config.min_value) * 
                    monitoring.voltage_config.anomaly_scale;
            } else {
                monitoring.voltage = monitoring.voltage_config.max_value + 
                    (monitoring.voltage_config.max_value - monitoring.voltage_config.min_value) * 
                    monitoring.voltage_config.anomaly_scale;
            }
        }
    }
    
    monitoring.last_update = std::chrono::system_clock::now();
    monitoring.total_sensor_updates++;
}

std::string SystemData::addAlarm(const std::string& sensor, const std::string& message, 
                                const std::string& severity, double value, double threshold) {
    std::lock_guard<std::mutex> lock(monitoring.alarms_mutex);
    
    MonitoringData::Alarm alarm;
    alarm.id = "ALM" + std::to_string(monitoring.alarm_id_counter++);
    alarm.sensor = sensor;
    alarm.message = message;
    alarm.severity = severity;
    alarm.value = value;
    alarm.threshold = threshold;
    alarm.timestamp = std::chrono::system_clock::now();
    alarm.acknowledged = false;
    alarm.active = true;
    
    monitoring.active_alarms.push_back(alarm);
    monitoring.total_alarms_triggered++;
    
    return alarm.id;
}

std::vector<SystemData::MonitoringData::Alarm> SystemData::getActiveAlarms() const {
    std::lock_guard<std::mutex> lock(monitoring.alarms_mutex);
    return monitoring.active_alarms;
}

bool SystemData::acknowledgeAlarm(const std::string& alarm_id) {
    std::lock_guard<std::mutex> lock(monitoring.alarms_mutex);
    
    for (auto& alarm : monitoring.active_alarms) {
        if (alarm.id == alarm_id) {
            alarm.acknowledged = true;
            return true;
        }
    }
    
    return false;
}

void SystemData::clearAcknowledgedAlarms() {
    std::lock_guard<std::mutex> lock(monitoring.alarms_mutex);
    
    auto it = std::remove_if(monitoring.active_alarms.begin(), monitoring.active_alarms.end(),
        [](const MonitoringData::Alarm& alarm) {
            return alarm.acknowledged;
        });
    
    monitoring.active_alarms.erase(it, monitoring.active_alarms.end());
}

void SystemData::clearAllAlarms() {
    std::lock_guard<std::mutex> lock(monitoring.alarms_mutex);
    monitoring.active_alarms.clear();
}

void SystemData::checkMonitoringThresholds() {
    // Check temperature thresholds
    if (monitoring.temp_config.monitor) {
        if (monitoring.temperature <= monitoring.thresholds.temp_error_min) {
            addAlarm("temperature", "Temperature below error threshold", 
                    "ERROR", monitoring.temperature, monitoring.thresholds.temp_error_min);
        } else if (monitoring.temperature >= monitoring.thresholds.temp_error_max) {
            addAlarm("temperature", "Temperature above error threshold", 
                    "ERROR", monitoring.temperature, monitoring.thresholds.temp_error_max);
        } else if (monitoring.temperature <= monitoring.thresholds.temp_warning_min) {
            addAlarm("temperature", "Temperature below warning threshold", 
                    "WARNING", monitoring.temperature, monitoring.thresholds.temp_warning_min);
        } else if (monitoring.temperature >= monitoring.thresholds.temp_warning_max) {
            addAlarm("temperature", "Temperature above warning threshold", 
                    "WARNING", monitoring.temperature, monitoring.thresholds.temp_warning_max);
        }
    }
    
    // Check current thresholds
    if (monitoring.current_config.monitor) {
        if (monitoring.current <= monitoring.thresholds.current_error_min) {
            addAlarm("current", "Current below error threshold", 
                    "ERROR", monitoring.current, monitoring.thresholds.current_error_min);
        } else if (monitoring.current >= monitoring.thresholds.current_error_max) {
            addAlarm("current", "Current above error threshold", 
                    "ERROR", monitoring.current, monitoring.thresholds.current_error_max);
        } else if (monitoring.current <= monitoring.thresholds.current_warning_min) {
            addAlarm("current", "Current below warning threshold", 
                    "WARNING", monitoring.current, monitoring.thresholds.current_warning_min);
        } else if (monitoring.current >= monitoring.thresholds.current_warning_max) {
            addAlarm("current", "Current above warning threshold", 
                    "WARNING", monitoring.current, monitoring.thresholds.current_warning_max);
        }
    }
    
    // Check power thresholds
    if (monitoring.power_config.monitor) {
        if (monitoring.power <= monitoring.thresholds.power_error_min) {
            addAlarm("power", "Power below error threshold", 
                    "ERROR", monitoring.power, monitoring.thresholds.power_error_min);
        } else if (monitoring.power >= monitoring.thresholds.power_error_max) {
            addAlarm("power", "Power above error threshold", 
                    "ERROR", monitoring.power, monitoring.thresholds.power_error_max);
        } else if (monitoring.power <= monitoring.thresholds.power_warning_min) {
            addAlarm("power", "Power below warning threshold", 
                    "WARNING", monitoring.power, monitoring.thresholds.power_warning_min);
        } else if (monitoring.power >= monitoring.thresholds.power_warning_max) {
            addAlarm("power", "Power above warning threshold", 
                    "WARNING", monitoring.power, monitoring.thresholds.power_warning_max);
        }
    }
}

SystemData::MonitoringData::SensorConfig* SystemData::getSensorConfig(const std::string& sensor_name) {
    if (sensor_name == "temperature") return &monitoring.temp_config;
    if (sensor_name == "current") return &monitoring.current_config;
    if (sensor_name == "power") return &monitoring.power_config;
    if (sensor_name == "voltage") return &monitoring.voltage_config;
    return nullptr;
}