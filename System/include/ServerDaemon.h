#pragma once
#include "../../DaemonLib/include/DaemonBase.h"
#include "Server.h"

class ServerDaemon : public DaemonBase {
protected:
    void mainLoop() override;
    void cleanup() override;
    
public:
    ServerDaemon();
    ~ServerDaemon() override;
    
    static void signalHandlerWrapper(int signum);
};