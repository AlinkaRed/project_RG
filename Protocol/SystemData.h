#pragma once

#include <random>

/**
 * @brief Class for storing all system data
 * 
 * @ingroup DataClasses
 * 
 * SystemData - storage for all system parameters:
 * - Configurable parameters (nominal power, frequency, modulation)
 * - Read-only parameters (temperature, real power, input power)
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
};