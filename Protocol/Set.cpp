#include "Set.h"

/**
 * @brief SET constructor
 */
SET::SET(SystemData& system_data) : System(system_data) {}

/**
 * @brief Executes parameter setting
 */
bool SET::execute(const std::string& parameter, const std::string& value) {
    bool result = false;
    
    if (parameter == "nominal_output_power") {
        result = setNominalPower(value);
    }
    else if (parameter == "frequency") {
        result = setFrequency(value);
    }
    else if (parameter == "automatic_modulation") {
        result = setAutomaticModulation(value);
    }
    else if (parameter == "modulation") {
        result = setModulation(value);
    }
    
    if (result) {
        updateSimulation();
    }
    
    return result;
}

/**
 * @brief Sets nominal output power
 */
bool SET::setNominalPower(const std::string& value) {
    if (!isValidNumber(value)) return false;
    
    try {
        double power = std::stod(value);
        if (power < 0 || power > 10) return false;
        data.nominal_output_power = power;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

/**
 * @brief Sets operating frequency
 */
bool SET::setFrequency(const std::string& value) {
    if (!isValidNumber(value)) return false;
    
    try {
        double freq = std::stod(value);
        if (freq < 25.0 || freq > 26.0) return false;
        if (static_cast<int>(freq * 1000) % 100 != 0) return false;
        data.frequency = freq;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

/**
 * @brief Sets automatic modulation mode
 */
bool SET::setAutomaticModulation(const std::string& value) {
    if (!isValidBoolean(value)) return false;
    
    if (value == "true" || value == "1") {
        data.automatic_modulation = true;
        return true;
    } else if (value == "false" || value == "0") {
        data.automatic_modulation = false;
        return true;
    }
    return false;
}

/**
 * @brief Sets modulation state
 */
bool SET::setModulation(const std::string& value) {
    if (data.automatic_modulation) return false;
    
    if (!isValidBoolean(value)) return false;
    
    if (value == "true" || value == "1") {
        data.modulation = true;
        return true;
    } else if (value == "false" || value == "0") {
        data.modulation = false;
        return true;
    }
    return false;
}