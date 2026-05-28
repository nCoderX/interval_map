#include <iostream>
#include <string>
#include <interval_map/interval_map.hpp>

// Example: Feature flag or configuration that changes over time
// Useful for A/B testing, gradual rollouts, or historical data.

int main() {
    using namespace interval_map;

    // Default logging level
    IntervalMap<int, std::string> log_level("INFO");

    // Gradual rollout of DEBUG logging for a service
    log_level.assign(100, 500, "DEBUG");      // Experiment window
    log_level.assign(800, 1200, "WARN");      // Maintenance period

    std::cout << "Log level at different points in time:\n";
    for (int t : {50, 200, 600, 900, 1500}) {
        std::cout << "  Time=" << t << " → " << log_level[t] << "\n";
    }

    return 0;
}
