#pragma once

#include "../../Protocol/include/System.h"

class ALARM : public System {
public:
    ALARM(SystemData& system_data);

    void checkAllAlarms();

    void updateAndCheckAlarms();

private:
    void checkTemperatureAlarms();

    void checkOutputPowerAlarms();

    void checkInputPowerAlarms();
};