#include "System.h"
#include <cctype>

/**
 * @brief System constructor
 * 
 * @param system_data Reference to shared system data
 * 
 * Initializes reference to data and performs first simulation update.
 */
System::System(SystemData& system_data) : data(system_data) {
    updateSimulation();
}

/**
 * @brief Sets callback for alarms
 * 
 * @param callback Function to call for alarms
 */
void System::setAlarmCallback(const std::function<void(const std::string&)>& callback) {
    alarm_callback_ = callback;
}

/**
 * @brief Checks validity of numeric string
 */
bool System::isValidNumber(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDecimal = false;
    bool hasDigit = false;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (i == 0 && (str[i] == '-' || str[i] == '+')) {
            continue;
        }
        else if (str[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        }
        else if (std::isdigit(str[i])) {
            hasDigit = true;
        }
        else {
            return false;
        }
    }
    
    return hasDigit;
}

/**
 * @brief Checks validity of boolean string
 */
bool System::isValidBoolean(const std::string& str) {
    return str == "true" || str == "false" || str == "1" || str == "0";
}

/**
 * @brief Updates simulation data
 */
void System::updateSimulation() {
    std::uniform_real_distribution<> temp_dis(-50, 120);
    std::uniform_real_distribution<> power_dis(-5, 15);
    std::uniform_real_distribution<> input_dis(-35, 5);

    data.temp = temp_dis(data.gen);
    data.real_output_power = power_dis(data.gen);
    data.input_power = input_dis(data.gen);
}

/**
 * @brief Calls alarm callback
 */
void System::triggerAlarm(const std::string& message) {
    if (alarm_callback_) {
        alarm_callback_(message);
    }
}