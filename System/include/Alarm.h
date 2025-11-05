#pragma once
#include "../../Protocol/include/System.h"

/**
 * @brief Class for system state monitoring and alarm generation
 * 
 * @ingroup MonitoringClasses
 * 
 * ALARM performs continuous monitoring of critical system parameters
 * and generates alarms when dangerous conditions are detected. Supports
 * three levels of alarm criticality:
 * 
 * - Warning: Warnings about approaching critical values
 * - Error: Errors when exceeding safe ranges
 * - Critical: Critical errors requiring immediate intervention
 * 
 * Monitoring includes:
 * - Equipment temperature
 * - Output power
 * - Input power
 * - Deviations from nominal values
 */
class ALARM : public System {
public:
    /**
     * @brief ALARM constructor
     * 
     * @param system_data Reference to shared system data
     */
    ALARM(SystemData& system_data);

    /**
     * @brief Checks all types of alarms
     * 
     * Performs a complete check of all monitored system parameters
     * and generates appropriate alarms.
     * 
     * @see checkTemperatureAlarms()
     * @see checkOutputPowerAlarms() 
     * @see checkInputPowerAlarms()
     */
    void checkAllAlarms();

    /**
     * @brief Updates simulation and checks alarms
     * 
     * Combined method for convenience - first updates
     * simulation data, then checks all alarms.
     */
    void updateAndCheckAlarms();

private:
    /**
     * @brief Checks temperature alarms
     * 
     * Generates alarms under the following conditions:
     * - Warning: >80°C or <-20°C
     * - Error: >100°C or <-30°C  
     * - Critical: >110°C or <-40°C
     */
    void checkTemperatureAlarms();
    
    /**
     * @brief Checks output power alarms
     * 
     * Generates alarms under the following conditions:
     * - Warning: Deviation from nominal power >2dB
     * - Error: Output power <-2dB or >12dB
     */
    void checkOutputPowerAlarms();
    
    /**
     * @brief Checks input power alarms
     * 
     * Generates alarms under the following conditions:
     * - Warning: Input power < -30dB
     * - Error: Input power > 0dB
     */
    void checkInputPowerAlarms();
};