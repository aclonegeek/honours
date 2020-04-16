#include <iostream>
#include <stdexcept>

#include "server.hpp"
#include "university.hpp"

int main(const int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>\n";
        return 1;
    }

    try {
        University university;
        asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[1]));
        Server server{io_context, endpoint, university};

        io_context.run();
    } catch (const std::invalid_argument& ia) {
        std::cerr << "ERROR - Invalid port.\n";
    } catch (const std::out_of_range& oor) {
        std::cerr << "ERROR - Invalid port.\n";
    } catch (const std::exception& e) {
        std::cerr << "ERROR - Exception: " << e.what() << "\n";
    }

    return 0;
}
