#include <iostream>

#include "client.hpp"

// TODO : More error handling, probably extract args parsing out into a function
// and return the port.
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: client <host> <port>\n";
        return 1;
    }

    try {
        asio::io_context io_context;

        tcp::resolver resolver{io_context};
        const auto endpoints = resolver.resolve(argv[1], argv[2]);
        Client client{io_context, endpoints};

        std::thread t([&io_context]() { io_context.run(); });

        char line[MAX_BODY_LENGTH + 1];
        while (std::cin.getline(line, MAX_BODY_LENGTH + 1)) {
            Message message(line);
            client.send(message);
        }

        client.close();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
