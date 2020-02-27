#pragma once

#include "session.hpp"
#include "university.hpp"

class StudentSession : public Session {
public:
    StudentSession(tcp::socket, University&);

    virtual void start() override;

protected:
    enum class State {
        WAITING_FOR_ACTION,
        REGISTER_FOR_COURSE,
        DEREGISTER_FROM_COURSE,
        DROP_COURSE,
    };

private:
    virtual bool handle_input() override;
    void greeting();

    bool register_for_course(const std::string&);
    bool deregister_from_course(const std::string&);
    bool drop_course(const std::string&);

    State state;
    University& university;
};
