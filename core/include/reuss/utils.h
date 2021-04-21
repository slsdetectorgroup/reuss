#pragma once
#include <algorithm>
#include <cstddef>
#include <string>

namespace ur {

void pin_this_thread(int i);
void set_realtime_priority();
void direct_input();
void reset_terminal();
template <typename Container> bool allEqual(const Container &c) {
    return (!c.empty() &&
            std::all_of(begin(c), end(c),
                        [c](const typename Container::value_type &element) {
                            return element == c.front();
                        }));
}
} // namespace ur
