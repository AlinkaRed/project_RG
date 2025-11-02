#include <gtest/gtest.h>
#include "../Protocol/SystemData.h"

TEST(SystemData, minimal_test) {
    SystemData data;
    EXPECT_EQ(0.0, data.nominal_output_power);
    EXPECT_EQ(25.0, data.frequency);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}