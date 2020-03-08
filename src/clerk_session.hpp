#pragma once

#include "session.hpp"
#include "university.hpp"

class ClerkSession final : public Session {
public:
    ClerkSession(tcp::socket, University&);

    virtual void start() override;

protected:
    enum class State {
        WAITING_FOR_ACTION,
        CREATING_COURSE,
        CREATING_STUDENT,
        DELETING_COURSE,
        DELETING_STUDENT,
    };

private:
    virtual bool handle_input() override;

    void greeting();
    void write_options();

    bool create_course(const std::string&);
    bool delete_course(const std::string&);

    bool create_student(const std::string&);
    bool delete_student(const std::string&);

    State state;
    University& university;
};
