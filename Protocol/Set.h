#pragma once
#include "System.h"

/**
 * @brief Class for setting system parameters
 * 
 * @ingroup CommandClasses
 * 
 * SET provides functionality for changing configurable system
 * parameters with full input data validation. All changes are checked
 * for correct ranges and formats.
 * 
 * Supported parameters:
 * - nominal_output_power: Nominal output power (0-10 dBm)
 * - frequency: Operating frequency (25.0-26.0 MHz with 0.1 step)
 * - automatic_modulation: Automatic modulation (on/off)
 * - modulation: Modulation state (on/off, only when automatic is off)
 */
class SET : public System {
public:
    /**
     * @brief SET constructor
     * 
     * @param system_data Reference to shared system data
     */
    SET(SystemData& system_data);

    /**
     * @brief Executes system parameter setting
     * 
     * @param parameter Name of parameter to set
     * @param value Parameter value in string format
     * @return true Parameter successfully set
     * @return false Validation error or unknown parameter
     * 
     * @note After successful parameter setting, system simulation
     * is automatically updated to reflect changes.
     * 
     * @see setNominalPower()
     * @see setFrequency() 
     * @see setAutomaticModulation()
     * @see setModulation()
     */
    bool execute(const std::string& parameter, const std::string& value);

private:
    /**
     * @brief Sets nominal output power
     */
    bool setNominalPower(const std::string& value);
    
    /**
     * @brief Sets operating frequency
     */
    bool setFrequency(const std::string& value);
    
    /**
     * @brief Sets automatic modulation mode
     */
    bool setAutomaticModulation(const std::string& value);
    
    /**
     * @brief Sets modulation state
     */
    bool setModulation(const std::string& value);
};