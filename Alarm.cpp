#include "ALARM.h"

/**
 * @brief ALARM constructor
 */
ALARM::ALARM(SystemData& system_data) : System(system_data) {}

/**
 * @brief Checks all alarms
 */
void ALARM::checkAllAlarms() {
    checkTemperatureAlarms();
    checkOutputPowerAlarms();
    checkInputPowerAlarms();
}

/**
 * @brief Updates simulation and checks alarms
 */
void ALARM::updateAndCheckAlarms() {
    updateSimulation();
    checkAllAlarms();
}

/**
 * @brief Checks temperature alarms
 */
void ALARM::checkTemperatureAlarms() {
    // High temperature check
    if (data.temp > 80 && data.temp <= 100) {
        triggerAlarm("Warning: Temperature approaching critical levels (>80°C)");
    } else if (data.temp > 100 && data.temp <= 110) {
        triggerAlarm("Error: Dangerous temperature (>100°C)");
    } else if (data.temp > 110) {
        triggerAlarm("Critical: Critical temperature (>110°C)");
    }

    if (data.temp < -20 && data.temp >= -30) {
        triggerAlarm("Warning: Temperature approaching critical levels (<-20°C)");
    } else if (data.temp < -30 && data.temp >= -40) {
        triggerAlarm("Error: Dangerous temperature (<-30°C)");
    } else if (data.temp < -40) {
        triggerAlarm("Critical: Critical temperature (<-40°C)");
    }
}

/**
 * @brief Checks output power alarms
 */
void ALARM::checkOutputPowerAlarms() {
    if (std::abs(data.real_output_power - data.nominal_output_power) > 2) {
        triggerAlarm("Warning: Output power deviation >2dB");
    }

    if (data.real_output_power < -2 || data.real_output_power > 12) {
        triggerAlarm("Error: Output power outside safe range (<-2dB or >12dB)");
    }
}

/**
 * @brief Checks input power alarms
 */
void ALARM::checkInputPowerAlarms() {
    if (data.input_power < -30) {
        triggerAlarm("Warning: Input power below -30dB");
    }

    if (data.input_power > 0) {
        triggerAlarm("Error: Input power above 0dB");
    }
}