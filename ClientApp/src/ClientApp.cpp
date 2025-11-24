#include <iostream>
#include <chrono>
#include <thread>
#include "../../System/include/Client.h"

int main() {
    std::cout << "Waiting for server to start..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    Client client;
    client.run();
    return 0;
}