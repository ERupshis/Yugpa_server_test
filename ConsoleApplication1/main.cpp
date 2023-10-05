#pragma once

#include <csignal>
#include <cstdlib>

#include "Config.h"
#include "Log.h"
#include "Server.h"

volatile sig_atomic_t g_shutdownFlag = 0;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "Received signal " << signal << ". Initiating graceful shutdown..." << std::endl;
        g_shutdownFlag = 1;
    }
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    Config cfg(8080, 4);
    cfg.parse();

    std::shared_ptr<Log> log = std::make_shared<Log>();
    std::unique_ptr<Server> server = std::make_unique<Server>(cfg, log);

    try {
        server->start();

        while (g_shutdownFlag == 0) {
            //Wait shutdown or exception.
        }
    }
    catch (std::exception& e) {
        if (g_shutdownFlag == 0) {
            log->error("[main] exception caought:", e.what());
        }
    }

    return 0;
}
