#include <iostream>

#include "server.hpp"
#include "university.hpp"

// TODO: More error handling, probably extract args parsing out into a
// function and return the port.
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>\n";
        return 1;
    }

    try {
        University university;
        asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
        Server server{io_context, endpoint, university};

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
