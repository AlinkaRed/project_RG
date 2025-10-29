#include <iostream>
#include <locale>
#include "Protocol/Set.h"
#include "Protocol/Get.h"
#include "Alarm.h"

int main() {
    setlocale(LC_ALL, "Russian");
    
    SystemData shared_data;
    
    SET set_system(shared_data);
    GET get_system(shared_data);
    ALARM alarm_system(shared_data);

    // Set callback for alarm handling
    auto alarm_callback = [](const std::string& alarm_msg) {
        std::cout << alarm_msg << std::endl;
    };
    
    set_system.setAlarmCallback(alarm_callback);
    get_system.setAlarmCallback(alarm_callback);
    alarm_system.setAlarmCallback(alarm_callback);

    // Testing SET commands
    std::cout << "=== Correct SET commands ===" << std::endl;
    std::cout << "SET nominal_output_power 5: " << (set_system.execute("nominal_output_power", "5") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET frequency 25.5: " << (set_system.execute("frequency", "25.5") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET automatic_modulation false: " << (set_system.execute("automatic_modulation", "false") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET modulation true: " << (set_system.execute("modulation", "true") ? "SUCCESS" : "FAIL") << std::endl;

    std::cout << "\n=== Incorrect SET commands (validation test) ===" << std::endl;
    std::cout << "SET nominal_output_power abc: " << (set_system.execute("nominal_output_power", "abc") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET frequency 25.5abc: " << (set_system.execute("frequency", "25.5abc") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET nominal_output_power 15: " << (set_system.execute("nominal_output_power", "15") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET frequency 24.9: " << (set_system.execute("frequency", "24.9") ? "SUCCESS" : "FAIL") << std::endl;
    std::cout << "SET automatic_modulation invalid: " << (set_system.execute("automatic_modulation", "invalid") ? "SUCCESS" : "FAIL") << std::endl;

    // Testing GET commands
    std::cout << "\n=== GET commands ===" << std::endl;
    std::cout << "Nominal output power: " << get_system.execute("nominal_output_power") << " dBm" << std::endl;
    std::cout << "Frequency: " << get_system.execute("frequency") << " MHz" << std::endl;
    std::cout << "Automatic modulation: " << get_system.execute("automatic_modulation") << std::endl;
    std::cout << "Modulation: " << get_system.execute("modulation") << std::endl;
    std::cout << "Temperature: " << get_system.execute("temp") << " C" << std::endl;
    std::cout << "Real output power: " << get_system.execute("real_output_power") << " dBm" << std::endl;
    std::cout << "Input power: " << get_system.execute("input_power") << " dBm" << std::endl;

    // Simulation changes for alarm testing
    for (int i = 0; i < 10; ++i) {
        std::cout << "\n--- Simulation step " << i + 1 << " ---" << std::endl;
        alarm_system.updateAndCheckAlarms();
        
        std::cout << "Temperature: " << get_system.execute("temp") << " C" << std::endl;
        std::cout << "Real output power: " << get_system.execute("real_output_power") << " dBm" << std::endl;
        std::cout << "Input power: " << get_system.execute("input_power") << " dBm" << std::endl;
    }

    return 0;
}