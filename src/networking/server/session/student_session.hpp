#pragma once

#include "session.hpp"
#include "university.hpp"

class StudentSession final : public Session {
public:
    StudentSession(tcp::socket, University&, const std::uint32_t);

    virtual void start() override;

private:
    enum class State {
        WAITING_FOR_ACTION,
        REGISTER_FOR_COURSE,
        DEREGISTER_FROM_COURSE,
        DROP_COURSE,
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
