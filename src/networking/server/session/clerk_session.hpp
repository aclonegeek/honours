#pragma once

#include "session.hpp"

class University;

class ClerkSession final : public Session {
public:
    ClerkSession(tcp::socket, University&);

    virtual void start() override;

private:
    enum class State {
        WAITING_FOR_ACTION,
        CREATING_COURSE,
        CREATING_STUDENT,
        DELETING_COURSE,
        DELETING_STUDENT,
    };

    virtual bool handle_input() override;

    void greeting();
    void write_options();
    void set_state();

    void create_course();
    void delete_course();

    void create_student();
    void delete_student();

    State state;
    University& university;
};
