#include <iostream>
#include <interval_map/interval_map.hpp>

// Example: Tiered pricing based on quantity purchased
// [0, 10)     → $10 per unit
// [10, 50)    → $8 per unit
// [50, 100)   → $6 per unit
// [100, ∞)    → $5 per unit

int main() {
    using namespace interval_map;

    IntervalMap<int, int> price_per_unit(10);   // Default: $10

    price_per_unit.assign(10, 50, 8);
    price_per_unit.assign(50, 100, 6);
    price_per_unit.assign(100, 1000000, 5);     // "infinity"

    auto calculate_total = [&](int quantity) {
        int unit_price = price_per_unit[quantity];
        return quantity * unit_price;
    };

    std::cout << "Pricing Tiers:\n";
    for (const auto& [qty, price] : price_per_unit.intervals()) {
        std::cout << "  " << qty << "+ units → $" << price << "/unit\n";
    }

    std::cout << "\nExample orders:\n";
    for (int q : {5, 12, 45, 67, 150}) {
        std::cout << "  " << q << " units → $" << calculate_total(q) << " total\n";
    }

    return 0;
}
