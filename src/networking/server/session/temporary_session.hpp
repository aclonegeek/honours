#pragma once

#include "session.hpp"
#include "university.hpp"

class TemporarySession final : public Session {
public:
    TemporarySession(tcp::socket, University&);

    virtual void start() override;

protected:
    enum class State {
        DETERMINING_SESSION_TYPE,
        CLERK_LOGIN,
        STUDENT_LOGIN,
    };

private:
    virtual bool handle_input() override;
    void greeting();

    State state;
    University& university;
};
