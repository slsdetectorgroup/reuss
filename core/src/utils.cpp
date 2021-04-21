#include "reuss/utils.h"
#include "reuss/project_defs.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <stdexcept>
#include <thread>

#include <termios.h>
#include <unistd.h>

namespace reuss {
void pin_this_thread(int i) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
        throw std::runtime_error("Could not pin thread");
}

void set_realtime_priority() {
    struct sched_param params {};
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &params) != 0)
        fmt::print(fg(fmt::color::yellow),
                   "Warning could not set thread priority!\n");
}

void direct_input() {
    struct termios ctrl {};
    tcgetattr(STDIN_FILENO, &ctrl);
    ctrl.c_lflag &= ~ICANON;
    ctrl.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
}

void reset_terminal() {
    struct termios ctrl {};
    tcgetattr(STDIN_FILENO, &ctrl);
    ctrl.c_lflag |= ICANON;
    ctrl.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
}

} // namespace reuss