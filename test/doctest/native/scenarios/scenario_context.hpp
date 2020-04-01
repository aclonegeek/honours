#pragma once

#include <chrono>
#include <thread>

#include "client.hpp"
#include "server.hpp"

static constexpr std::string_view host = "localhost";
static constexpr std::string_view port = "5001";

class ScenarioContext {
public:
    ScenarioContext()
        : server_endpoint(tcp::endpoint(tcp::v4(), 5001)),
          server(Server(this->server_io_context, this->server_endpoint,
                        this->_university)) {
        this->server_thread =
            std::thread([&] { this->server_io_context.run(); });

        // Give everything some time to start up.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ~ScenarioContext() {
        this->server_io_context.stop();
        this->server_thread.join();
    }

    virtual void background() = 0;

    const University& university() const { return this->_university; }

private:
    asio::io_context server_io_context;
    tcp::endpoint server_endpoint;

    std::thread server_thread;

    University _university;
    Server server;
};
