#pragma once

#include "session.hpp"

class TemporarySession : public Session {
public:
    TemporarySession(tcp::socket);

private:
    virtual void handle_input() override;
};
