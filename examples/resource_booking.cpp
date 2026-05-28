#include <iostream>
#include <string>
#include <interval_map/interval_map.hpp>

// Example: Simple room booking system using IntervalMap
// Each time slot [start, end) is mapped to the person who booked it.

int main() {
    using namespace interval_map;

    // 'Free' is the default state
    IntervalMap<int, std::string> bookings("Free");

    // Book some meetings
    bookings.assign(9, 11, "Alice");      // 9:00 - 11:00
    bookings.assign(10, 12, "Bob");       // Overlaps with Alice
    bookings.assign(14, 16, "Charlie");   // Afternoon meeting

    std::cout << "Room Schedule:\n";
    for (int time = 8; time < 18; ++time) {
        std::cout << "  " << time << ":00 -> " << bookings[time] << "\n";
    }

    std::cout << "\nInternal representation (change points):\n";
    for (const auto& [start, person] : bookings.intervals()) {
        std::cout << "  From " << start << ":00 → " << person << "\n";
    }

    return 0;
}
