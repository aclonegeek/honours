#pragma once

#include "session.hpp"

class ClerkSession : public Session {
public:
    ClerkSession(tcp::socket);

    virtual void start() override;

protected:
    enum class State {};

private:
    virtual bool handle_input() override;
    void greeting();

    State state;
};
