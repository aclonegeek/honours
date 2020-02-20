#pragma once

#include "session.hpp"

using asio::ip::tcp;

class TemporarySession : public Session,
                         public std::enable_shared_from_this<TemporarySession> {
public:
    TemporarySession(tcp::socket);

private:
    virtual void read_header() override;
    virtual void read_body() override;

    virtual void handle_input() override;

    virtual void write() override;
};
