#include "../include/SystemData.h"

/**
 * @brief SystemData constructor
 * 
 * Initializes random number generator and randomly sets initial modulation state.
 */
SystemData::SystemData() : gen(rd()) {
    std::uniform_int_distribution<> dis(0, 1);
    modulation = dis(gen);
}