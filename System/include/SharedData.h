#pragma once

#include <cstring>

inline constexpr const char* SHM_NAME = "/radio_control_memory";
inline constexpr const char* SEM_CLIENT_NAME = "/sem_radio_client";
inline constexpr const char* SEM_SERVER_NAME = "/sem_radio_server";

struct SharedData {
    char command[256];
    char response[1024];
    
    struct {
        double temperature;
        double current;
        double power;
        double voltage;
        int active_alarms_count;
        bool service_enabled;
        char last_update[64];
    } monitoring;
    
    SharedData() {
        memset(command, 0, sizeof(command));
        memset(response, 0, sizeof(response));
        monitoring.temperature = 0.0;
        monitoring.current = 0.0;
        monitoring.power = 0.0;
        monitoring.voltage = 0.0;
        monitoring.active_alarms_count = 0;
        monitoring.service_enabled = true;
        memset(monitoring.last_update, 0, sizeof(monitoring.last_update));
    }
};