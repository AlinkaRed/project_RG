#include <gtest/gtest.h>
#include "../Protocol/include/SystemData.h"
#include "../Protocol/include/System.h"
#include "../Protocol/include/Set.h"
#include "../Protocol/include/Get.h"
#include "../System/include/Alarm.h"

// SystemData tests
TEST(SystemData, can_create_system_data)
{
    ASSERT_NO_THROW(SystemData data);
}

TEST(SystemData, has_correct_initial_values)
{
    SystemData data;
    EXPECT_DOUBLE_EQ(0.0, data.nominal_output_power);
    EXPECT_DOUBLE_EQ(25.0, data.frequency);
    EXPECT_TRUE(data.automatic_modulation);
    EXPECT_DOUBLE_EQ(0.0, data.temp);
    EXPECT_DOUBLE_EQ(0.0, data.real_output_power);
    EXPECT_DOUBLE_EQ(-15.0, data.input_power);
}

// System tests
TEST(System, can_create_system_with_system_data)
{
    SystemData system_data;
    ASSERT_NO_THROW(System system(system_data));
}

TEST(System, can_set_alarm_callback)
{
    SystemData system_data;
    System system(system_data);
    
    bool callback_called = false;
    auto callback = [&](const std::string& msg) { callback_called = true; };
    
    system.setAlarmCallback(callback);
    
    SUCCEED();
}

// SET tests
TEST(SET, can_create_set_instance)
{
    SystemData system_data;
    ASSERT_NO_THROW(SET set(system_data));
}

TEST(SET, execute_sets_valid_nominal_power)
{
    SystemData system_data;
    SET set(system_data);
    
    EXPECT_TRUE(set.execute("nominal_output_power", "5.0"));
    EXPECT_DOUBLE_EQ(5.0, system_data.nominal_output_power);
    
    EXPECT_TRUE(set.execute("nominal_output_power", "0.0"));
    EXPECT_DOUBLE_EQ(0.0, system_data.nominal_output_power);
    
    EXPECT_TRUE(set.execute("nominal_output_power", "10.0"));
    EXPECT_DOUBLE_EQ(10.0, system_data.nominal_output_power);
}

TEST(SET, execute_rejects_invalid_nominal_power)
{
    SystemData system_data;
    SET set(system_data);
    
    EXPECT_FALSE(set.execute("nominal_output_power", "-1.0"));
    EXPECT_FALSE(set.execute("nominal_output_power", "11.0"));
    EXPECT_FALSE(set.execute("nominal_output_power", "abc"));
    EXPECT_FALSE(set.execute("nominal_output_power", ""));
}

TEST(SET, execute_sets_valid_frequency)
{
    SystemData system_data;
    SET set(system_data);
    
    EXPECT_TRUE(set.execute("frequency", "25.0"));
    EXPECT_DOUBLE_EQ(25.0, system_data.frequency);
    
    EXPECT_TRUE(set.execute("frequency", "25.5"));
    EXPECT_DOUBLE_EQ(25.5, system_data.frequency);
    
    EXPECT_TRUE(set.execute("frequency", "26.0"));
    EXPECT_DOUBLE_EQ(26.0, system_data.frequency);
}

TEST(SET, execute_rejects_invalid_frequency)
{
    SystemData system_data;
    SET set(system_data);
    
    EXPECT_FALSE(set.execute("frequency", "24.9"));
    EXPECT_FALSE(set.execute("frequency", "26.1"));
    EXPECT_FALSE(set.execute("frequency", "25.12")); // invalid step
    EXPECT_FALSE(set.execute("frequency", "abc"));
}

TEST(SET, execute_sets_automatic_modulation)
{
    SystemData system_data;
    SET set(system_data);
    
    EXPECT_TRUE(set.execute("automatic_modulation", "true"));
    EXPECT_TRUE(system_data.automatic_modulation);
    
    EXPECT_TRUE(set.execute("automatic_modulation", "false"));
    EXPECT_FALSE(system_data.automatic_modulation);
    
    EXPECT_TRUE(set.execute("automatic_modulation", "1"));
    EXPECT_TRUE(system_data.automatic_modulation);
    
    EXPECT_TRUE(set.execute("automatic_modulation", "0"));
    EXPECT_FALSE(system_data.automatic_modulation);
}

TEST(SET, execute_handles_modulation_based_on_automatic_mode)
{
    SystemData system_data;
    SET set(system_data);
    
    system_data.automatic_modulation = true;
    EXPECT_FALSE(set.execute("modulation", "true"));
    EXPECT_FALSE(set.execute("modulation", "false"));
    
    system_data.automatic_modulation = false;
    EXPECT_TRUE(set.execute("modulation", "true"));
    EXPECT_TRUE(system_data.modulation);
    
    EXPECT_TRUE(set.execute("modulation", "false"));
    EXPECT_FALSE(system_data.modulation);
}

TEST(SET, execute_returns_false_for_unknown_parameter)
{
    SystemData system_data;
    SET set(system_data);
    
    EXPECT_FALSE(set.execute("unknown_parameter", "value"));
    EXPECT_FALSE(set.execute("", "value"));
}

// GET tests
TEST(GET, can_create_get_instance)
{
    SystemData system_data;
    ASSERT_NO_THROW(GET get(system_data));
}

TEST(GET, execute_returns_nominal_power)
{
    SystemData system_data;
    GET get(system_data);
    
    system_data.nominal_output_power = 7.5;
    std::string result = get.execute("nominal_output_power");
    EXPECT_FALSE(result.empty());
    EXPECT_NE(std::string::npos, result.find("7.5"));
}

TEST(GET, execute_returns_frequency)
{
    SystemData system_data;
    GET get(system_data);
    
    system_data.frequency = 25.7;
    std::string result = get.execute("frequency");
    EXPECT_FALSE(result.empty());
    EXPECT_NE(std::string::npos, result.find("25.7"));
}

TEST(GET, execute_returns_automatic_modulation_status)
{
    SystemData system_data;
    GET get(system_data);
    
    system_data.automatic_modulation = true;
    EXPECT_EQ("on", get.execute("automatic_modulation"));
    
    system_data.automatic_modulation = false;
    EXPECT_EQ("off", get.execute("automatic_modulation"));
}

TEST(GET, execute_returns_modulation_status)
{
    SystemData system_data;
    GET get(system_data);
    
    system_data.automatic_modulation = true;
    EXPECT_EQ("auto", get.execute("modulation"));
    
    system_data.automatic_modulation = false;
    system_data.modulation = true;
    EXPECT_EQ("on", get.execute("modulation"));
    
    system_data.automatic_modulation = false;
    system_data.modulation = false;
    EXPECT_EQ("off", get.execute("modulation"));
}

TEST(GET, execute_returns_temperature)
{
    SystemData system_data;
    GET get(system_data);
    
    system_data.temp = 25.5;
    std::string result = get.execute("temp");
    EXPECT_FALSE(result.empty());
}

TEST(GET, execute_returns_power_values)
{
    SystemData system_data;
    GET get(system_data);
    
    system_data.real_output_power = 5.5;
    system_data.input_power = -10.0;
    
    std::string real_power = get.execute("real_output_power");
    std::string input_power = get.execute("input_power");
    
    EXPECT_FALSE(real_power.empty());
    EXPECT_FALSE(input_power.empty());
}

TEST(GET, execute_returns_error_for_unknown_parameter)
{
    SystemData system_data;
    GET get(system_data);
    
    EXPECT_EQ("Error: Unknown parameter", get.execute("unknown_parameter"));
    EXPECT_EQ("Error: Unknown parameter", get.execute(""));
}

// ALARM tests
TEST(ALARM, can_create_alarm_instance)
{
    SystemData system_data;
    ASSERT_NO_THROW(ALARM alarm(system_data));
}

TEST(ALARM, checkAllAlarms_triggers_callback_for_alarm_conditions)
{
    SystemData system_data;
    ALARM alarm(system_data);
    
    std::vector<std::string> alarm_messages;
    alarm.setAlarmCallback([&](const std::string& message) {
        alarm_messages.push_back(message);
    });
    
    system_data.temp = 90.0;
    system_data.nominal_output_power = 5.0;
    system_data.real_output_power = 8.5;
    system_data.input_power = -35.0;
    
    alarm.checkAllAlarms();
    
    EXPECT_FALSE(alarm_messages.empty());
}

TEST(ALARM, updateAndCheckAlarms_updates_simulation_data)
{
    SystemData system_data;
    ALARM alarm(system_data);
    
    double initial_temp = system_data.temp;
    double initial_real_power = system_data.real_output_power;
    double initial_input_power = system_data.input_power;
    
    alarm.updateAndCheckAlarms();
    
    EXPECT_NE(initial_temp, system_data.temp);
    EXPECT_NE(initial_real_power, system_data.real_output_power);
    EXPECT_NE(initial_input_power, system_data.input_power);
}

TEST(ALARM, alarm_callback_receives_messages)
{
    SystemData system_data;
    ALARM alarm(system_data);
    
    std::string received_message;
    alarm.setAlarmCallback([&](const std::string& message) {
        received_message = message;
    });
    
    system_data.input_power = 2.0;
    alarm.checkAllAlarms();
    
    EXPECT_FALSE(received_message.empty());
    EXPECT_NE(std::string::npos, received_message.find("Input power"));
}

TEST(ALARM, no_alarms_under_normal_conditions)
{
    SystemData system_data;
    ALARM alarm(system_data);
    
    bool alarm_triggered = false;
    alarm.setAlarmCallback([&](const std::string&) {
        alarm_triggered = true;
    });
    
    system_data.temp = 25.0;
    system_data.nominal_output_power = 5.0;
    system_data.real_output_power = 5.1;
    system_data.input_power = -15.0;
    
    alarm.checkAllAlarms();
    
    EXPECT_FALSE(alarm_triggered);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}