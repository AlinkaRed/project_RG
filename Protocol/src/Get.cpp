#include "../include/Get.h"

/**
 * @brief GET constructor
 */
GET::GET(SystemData& system_data) : System(system_data) {}

/**
 * @brief Executes parameter reading
 */
std::string GET::execute(const std::string& parameter) const {
    if (parameter == "nominal_output_power") {
        return std::to_string(data.nominal_output_power);
    }
    else if (parameter == "frequency") {
        return std::to_string(data.frequency);
    }
    else if (parameter == "automatic_modulation") {
        return data.automatic_modulation ? "on" : "off";
    }
    else if (parameter == "modulation") {
        if (data.automatic_modulation) {
            return "auto";
        }
        return data.modulation ? "on" : "off";
    }
    else if (parameter == "temp") {
        return std::to_string(data.temp);
    }
    else if (parameter == "real_output_power") {
        return std::to_string(data.real_output_power);
    }
    else if (parameter == "input_power") {
        return std::to_string(data.input_power);
    }
    return "Error: Unknown parameter";
}