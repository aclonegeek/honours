#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

enum class State;

class StateChangeTimerTask {
public:
    StateChangeTimerTask(State& state, const State next_state,
                         const std::chrono::seconds duration)
        : running(true), timer([&, next_state, duration]() {
              std::unique_lock lock(this->mutex);
              if (this->kill_timer.wait_for(
                      lock, duration, [this]() { return !this->running; })) {
                  // If we get in here, University's destructor was called, so
                  // we want to bail.
                  return;
              }

              state = next_state;
          }) {}

    ~StateChangeTimerTask() {
        {
            std::lock_guard lock(this->mutex);
            this->running = false;
        }

        this->kill_timer.notify_one();
        this->timer.join();
    }

private:
    bool running;
    std::thread timer;
    std::condition_variable kill_timer;
    std::mutex mutex;
};
