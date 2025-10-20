#include <iostream>
#include <string>
#include <functional>
#include <random>
#include <map>
#include <vector>
#include <locale>
#include <sstream>
#include <cctype>


class SystemData {
public:
    // ������������� ���������
    double nominal_output_power = 0.0;    // dBm, 0-10
    double frequency = 25.0;         // MHz, 25-26
    bool automatic_modulation = true; // ���/����
    bool modulation = true;  // ���/����

    // ��������� ������ ��� ������
    double temp = 0.0;               // C
    double real_output_power = 0.0;  // dBm, -2 - 12
    double input_power = -15.0;      // dBm, -30 - 0

    // ��������� ��������� �����
    std::random_device rd;
    std::mt19937 gen;

    SystemData() : gen(rd()) {
        std::uniform_int_distribution<> dis(0, 1);
        modulation = dis(gen);
    }
};

class System {
protected:
    SystemData& data;
    std::function<void(const std::string&)> alarm_callback_;

    bool isValidNumber(const std::string& str) {
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

    bool isValidBoolean(const std::string& str) {
        return str == "true" || str == "false" || str == "1" || str == "0";
    }

    void updateSimulation() {
        std::uniform_real_distribution<> temp_dis(-50, 120);
        std::uniform_real_distribution<> power_dis(-5, 15);
        std::uniform_real_distribution<> input_dis(-35, 5);

        data.temp = temp_dis(data.gen);
        data.real_output_power = power_dis(data.gen);
        data.input_power = input_dis(data.gen);
    }

public:
    System(SystemData& system_data) : data(system_data) {
        updateSimulation();
    }

    void setAlarmCallback(const std::function<void(const std::string&)>& callback) {
        alarm_callback_ = callback;
    }

protected:
    void triggerAlarm(const std::string& message) {
        if (alarm_callback_) {
            alarm_callback_(message);
        }
    }
};

class SET : public System {
public:
    SET(SystemData& system_data) : System(system_data) {}

    bool execute(const std::string& parameter, const std::string& value) {
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

private:
    bool setNominalPower(const std::string& value) {
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

    bool setFrequency(const std::string& value) {
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

    bool setAutomaticModulation(const std::string& value) {
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

    bool setModulation(const std::string& value) {
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
};

class GET : public System {
public:
    GET(SystemData& system_data) : System(system_data) {}

    std::string execute(const std::string& parameter) const {
        if (parameter == "nominal_output_power") {
            return std::to_string(data.nominal_output_power);
        }
        else if (parameter == "frequency") {
            return std::to_string(data.frequency);
        }
        else if (parameter == "automatic_modulation") {
            return data.automatic_modulation ? "���" : "����";
        }
        else if (parameter == "modulation") {
            if (data.automatic_modulation) {
                return "����";
            }
            return data.modulation ? "���" : "����";
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
        return "Error: ����������� ��������";
    }
};

class ALARM : public System {
public:
    ALARM(SystemData& system_data) : System(system_data) {}

    void checkAllAlarms() {
        checkTemperatureAlarms();
        checkOutputPowerAlarms();
        checkInputPowerAlarms();
    }

    void updateAndCheckAlarms() {
        updateSimulation();
        checkAllAlarms();
    }

private:
    void checkTemperatureAlarms() {
        if (data.temp > 80 && data.temp <= 100) {
            triggerAlarm("Warning: ����������� ������������ � ����������� ������� (>80�C)");
        } else if (data.temp > 100 && data.temp <= 110) {
            triggerAlarm("Error: ������� ����������� (>100�C)");
        } else if (data.temp > 110) {
            triggerAlarm("Critical: ����������� ����������� (>110�C)");
        }

        if (data.temp < -20 && data.temp >= -30) {
            triggerAlarm("Warning: ����������� ������������ � ����������� ������� (<-20�C)");
        } else if (data.temp < -30 && data.temp >= -40) {
            triggerAlarm("Error: ������� ����������� (<-30�C)");
        } else if (data.temp < -40) {
            triggerAlarm("Critical: ����������� ����������� (<-40�C)");
        }
    }

    void checkOutputPowerAlarms() {
        if (std::abs(data.real_output_power - data.nominal_output_power) > 2) {
            triggerAlarm("Warning: ���������� �������� �������� >2dB");
        }

        if (data.real_output_power < -2 || data.real_output_power > 12) {
            triggerAlarm("Error: �������� �������� ��� ����������� ��������� (<-2dB ��� >12dB)");
        }
    }

    void checkInputPowerAlarms() {
        if (data.input_power < -30) {
            triggerAlarm("Warning: ������� �������� ���� -30dB");
        }

        if (data.input_power > 0) {
            triggerAlarm("Error: ������� �������� ���� 0dB");
        }
    }
};

// ������ �������������
int main() {
    setlocale(LC_ALL, "Russian");
    
    SystemData shared_data;
    
    SET set_system(shared_data);
    GET get_system(shared_data);
    ALARM alarm_system(shared_data);

    // ��������� callback ��� ��������� �������
    auto alarm_callback = [](const std::string& alarm_msg) {
        std::cout << alarm_msg << std::endl;
    };
    
    set_system.setAlarmCallback(alarm_callback);
    get_system.setAlarmCallback(alarm_callback);
    alarm_system.setAlarmCallback(alarm_callback);

    // ������������ SET ������
    std::cout << "=== ���������� SET ������� ===" << std::endl;
    std::cout << "SET nominal_output_power 5: " << (set_system.execute("nominal_output_power", "5") ? "�����" : "�������") << std::endl;
    std::cout << "SET frequency 25.5: " << (set_system.execute("frequency", "25.5") ? "�����" : "�������") << std::endl;
    std::cout << "SET automatic_modulation false: " << (set_system.execute("automatic_modulation", "false") ? "�����" : "�������") << std::endl;
    std::cout << "SET modulation true: " << (set_system.execute("modulation", "true") ? "�����" : "�������") << std::endl;

    std::cout << "\n=== ������������ SET ������� (���� ���������) ===" << std::endl;
    std::cout << "SET nominal_output_power abc: " << (set_system.execute("nominal_output_power", "abc") ? "�����" : "�������") << std::endl;
    std::cout << "SET frequency 25.5abc: " << (set_system.execute("frequency", "25.5abc") ? "�����" : "�������") << std::endl;
    std::cout << "SET nominal_output_power 15: " << (set_system.execute("nominal_output_power", "15") ? "�����" : "�������") << std::endl;
    std::cout << "SET frequency 24.9: " << (set_system.execute("frequency", "24.9") ? "�����" : "�������") << std::endl;
    std::cout << "SET automatic_modulation invalid: " << (set_system.execute("automatic_modulation", "invalid") ? "�����" : "�������") << std::endl;

    // ������������ GET ������
    std::cout << "\n=== GET ������� ===" << std::endl;
    std::cout << "����������� �������� ��������: " << get_system.execute("nominal_output_power") << " dBm" << std::endl;
    std::cout << "�������: " << get_system.execute("frequency") << " MHz" << std::endl;
    std::cout << "�������������� ���������: " << get_system.execute("automatic_modulation") << std::endl;
    std::cout << "���������: " << get_system.execute("modulation") << std::endl;
    std::cout << "�����������: " << get_system.execute("temp") << " C" << std::endl;
    std::cout << "�������� �������� ��������: " << get_system.execute("real_output_power") << " dBm" << std::endl;
    std::cout << "������� ��������: " << get_system.execute("input_power") << " dBm" << std::endl;

    // ��������� ��������� ��� ������������ �������
    for (int i = 0; i < 10; ++i) {
        std::cout << "\n--- ��� ��������� " << i + 1 << " ---" << std::endl;
        alarm_system.updateAndCheckAlarms();
        
        std::cout << "�����������: " << get_system.execute("temp") << " C" << std::endl;
        std::cout << "�������� �������� ��������: " << get_system.execute("real_output_power") << " dBm" << std::endl;
        std::cout << "������� ��������: " << get_system.execute("input_power") << " dBm" << std::endl;
    }

    return 0;
}