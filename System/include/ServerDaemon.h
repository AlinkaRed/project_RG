#pragma once
#include "../../DaemonLib/include/DaemonBase.h"
#include "Server.h"

class ServerDaemon : public DaemonBase {
private:
    Server server;
    
protected:
    void mainLoop() override;
    void cleanup() override;
    
public:
    ServerDaemon();
    ~ServerDaemon();
};