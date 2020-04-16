#pragma once

#include "session.hpp"

class University;

class StudentSession final : public Session {
public:
    StudentSession(tcp::socket, University&, const std::uint32_t);

    virtual void start() override;

private:
    enum class State {
        WAITING_FOR_ACTION,
        REGISTERING_FOR_COURSE,
        DEREGISTERING_FROM_COURSE,
        DROPPING_COURSE,
    };

    virtual bool handle_input() override;

    void greeting();
    void set_state();
    void write_options();

    void register_for_course();
    void deregister_from_course();
    void drop_course();

    State state;
    University& university;

    const std::uint32_t id;
};
