#pragma once

#include <random>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>

/**
 * @brief Class for storing all system data with monitoring extensions
 * 
 * @ingroup DataClasses
 * 
 * SystemData - storage for all system parameters:
 * - Configurable parameters (nominal power, frequency, modulation)
 * - Read-only parameters (temperature, real power, input power)
 * - Monitoring parameters (sensors, alarms, configuration)
 * - Random number generator for system operation simulation
 * 
 * All system classes work with a single reference to SystemData object,
 * ensuring data consistency.
 */
class SystemData {
public:
    // Configurable parameters
    
    /**
     * @brief Nominal output power
     * 
     * Range: 0-10 dBm
     * Set by user, used as target value for power management system.
     */
    double nominal_output_power = 0.0;
    
    /**
     * @brief Operating frequency
     * 
     * Range: 25.0-26.0 MHz
     * Tuning step: 0.1 MHz
     */
    double frequency = 25.0;
    
    /**
     * @brief Automatic modulation mode
     * 
     * When automatic modulation is enabled, manual modulation control
     * is blocked.
     */
    bool automatic_modulation = true;
    
    /**
     * @brief Modulation state
     * 
     * Controlled automatically or manually depending on
     * automatic_modulation.
     */
    bool modulation = true;

    // Read-only parameters
    
    /**
     * @brief Equipment temperature
     * 
     * Range: -50°C to +120°C
     * Generated randomly during simulation
     */
    double temp = 0.0;
    
    /**
     * @brief Real output power
     * 
     * Range: -2 dBm to 12 dBm
     * Deviation from nominal power may trigger alarms
     */
    double real_output_power = 0.0;
    
    /**
     * @brief Input power
     * 
     * Range: -30 dBm to 0 dBm
     * Values outside range trigger alarms
     */
    double input_power = -15.0;

    // Monitoring system data
    
    /**
     * @brief Monitoring system configuration and state
     */
    struct MonitoringData {
        // Sensor values
        double temperature = 25.0;
        double current = 5.0;
        double power = 50.0;
        double voltage = 220.0;
        
        // Sensor configurations
        struct SensorConfig {
            double min_value;
            double max_value;
            double anomaly_probability;
            double anomaly_scale;
            bool enabled;
            bool monitor;
            
            SensorConfig(double min, double max, double prob = 0.05, double scale = 1.5) 
                : min_value(min), max_value(max), 
                  anomaly_probability(prob), anomaly_scale(scale),
                  enabled(true), monitor(true) {}
        };
        
        SensorConfig temp_config = {-40.0, 85.0, 0.02, 1.5};
        SensorConfig current_config = {0.0, 10.0, 0.03, 1.5};
        SensorConfig power_config = {0.0, 100.0, 0.03, 1.5};
        SensorConfig voltage_config = {200.0, 240.0, 0.01, 1.2};
        
        // Monitoring service state
        bool service_enabled = true;
        int polling_interval_ms = 1000;
        
        // Thresholds for alarms
        struct Thresholds {
            double temp_warning_min = -20.0;
            double temp_warning_max = 70.0;
            double temp_error_min = -30.0;
            double temp_error_max = 85.0;
            
            double current_warning_min = 1.0;
            double current_warning_max = 8.0;
            double current_error_min = 0.5;
            double current_error_max = 9.0;
            
            double power_warning_min = 10.0;
            double power_warning_max = 80.0;
            double power_error_min = 5.0;
            double power_error_max = 90.0;
        } thresholds;
        
        // Alarm structure
        struct Alarm {
            std::string id;
            std::string sensor;
            std::string message;
            std::string severity; // WARNING, ERROR, CRITICAL
            double value;
            double threshold;
            std::chrono::system_clock::time_point timestamp;
            bool acknowledged;
            bool active;
            
            Alarm() : acknowledged(false), active(true) {}
        };
        
        std::vector<Alarm> active_alarms;
        mutable std::mutex alarms_mutex;
        
        // Statistics
        std::chrono::system_clock::time_point last_update;
        int total_sensor_updates = 0;
        int total_alarms_triggered = 0;
        int alarm_id_counter = 1;
        
        MonitoringData() {
            last_update = std::chrono::system_clock::now();
        }
    } monitoring;
    
    // Random number generator
    
    std::random_device rd;    ///< Device for obtaining random seed
    std::mt19937 gen;         ///< Mersenne Twister pseudorandom number generator

    /**
     * @brief Default constructor
     * 
     * Initializes random number generator and sets
     * initial random modulation state.
     */
    SystemData();
    
    /**
     * @brief Update monitoring sensor values
     */
    void updateMonitoringSensors();
    
    /**
     * @brief Add alarm to monitoring system
     */
    std::string addAlarm(const std::string& sensor, const std::string& message, 
                        const std::string& severity, double value, double threshold);
    
    /**
     * @brief Get active alarms
     */
    std::vector<MonitoringData::Alarm> getActiveAlarms() const;
    
    /**
     * @brief Acknowledge alarm
     */
    bool acknowledgeAlarm(const std::string& alarm_id);
    
    /**
     * @brief Clear acknowledged alarms
     */
    void clearAcknowledgedAlarms();
    
    /**
     * @brief Clear all alarms
     */
    void clearAllAlarms();
    
    /**
     * @brief Check sensor thresholds and trigger alarms
     */
    void checkMonitoringThresholds();
    
    /**
     * @brief Get sensor configuration
     */
    MonitoringData::SensorConfig* getSensorConfig(const std::string& sensor_name);
};