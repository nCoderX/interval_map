#include "pch.h"
#include "interval_map.h"

bool testOverhead = true;
class KHolder
{
	int m_k;
	mutable int m_comp_counter = 0;
	mutable int m_assign_counter = 0;
	void increment_assignment() const {
		if(testOverhead && m_assign_counter) throw std::exception("second assignment");
		++m_assign_counter;
	}
	void increment_comparison() const {
		if (testOverhead && m_comp_counter>1) throw std::exception("second comparison");
		++m_comp_counter;
	}
	KHolder() = delete;
public:
	KHolder(int k) : m_k(k) { increment_assignment(); }
	KHolder(const KHolder& other) : m_k(other.m_k) {
		increment_assignment();
		std::cout << "K copy ctor" << '\n';
	}
	KHolder(KHolder&& other) noexcept : m_k(other.m_k) {
		std::cout << "K move ctor" << '\n';
	}
	KHolder& operator=(const KHolder& other) {
		std::cout << "K copy assign" << '\n';
		increment_assignment();
		m_k = other.m_k; return *this;
	}
	KHolder& operator=(KHolder&& other) noexcept
	{
		std::cout << "K move assign" << '\n';
		m_k = other.m_k; return *this;
	}
	bool operator<(const KHolder& other) const {
		std::cout << "K compare" << '\n';
		increment_comparison();
		return m_k < other.m_k;
	}
	
	operator int() const { return m_k; }
	void swap(KHolder& other) noexcept {
		std::swap(m_k, other.m_k);
	}
};

class VHolder
{
	char m_v;
	mutable int m_comp_counter = 0;
	mutable int m_assign_counter = 0;
	void increment_assignment() const {
		if (testOverhead && m_assign_counter) throw std::exception("second assignment");
		++m_assign_counter;
	}
	void increment_comparison() const {
		if (testOverhead && m_comp_counter) throw std::exception("second comparison");
		++m_comp_counter;
	}
	VHolder() = delete;
	bool operator!=(const VHolder& other) = delete;
public:
	VHolder(char v) : m_v(v) { increment_assignment(); }
	VHolder(const VHolder& other) : m_v(other.m_v) {
		std::cout << "V copy ctor" << std::endl;
		increment_assignment();
	}
	VHolder(VHolder&& other) noexcept : m_v(other.m_v) {
		other.m_v = 0;
		std::cout << "V move ctor" << std::endl;
	}
	VHolder& operator=(const VHolder& other) {
		std::cout << "V copy assign" << std::endl;
		increment_assignment();
		m_v = other.m_v; return *this;
	}
	VHolder& operator=(VHolder&& other) {
		std::cout << "V move assign" << std::endl;
		m_v = other.m_v;
		other.m_v = 0;
		return *this;
	}
	bool operator==(const VHolder& other) const {
		std::cout << "V compare" << std::endl;
		increment_comparison();
		return m_v == other.m_v;
	}
	void swap(VHolder& other) noexcept {
		std::swap(m_v, other.m_v);
	}

	
};
static bool is_valid_interval_map(const interval_map<KHolder, VHolder>& map) {
	if (map.intervals().empty()) return true;
	
	auto prev = map.valBegin();
	auto prevIdx = -1;
	for (const auto & e : map.intervals()) {
		if (e.second == prev) return false;
		prev = e.second;
		if (!(prevIdx < e.first)) return false;
		prevIdx = e.first;
	}
	return true;
}

TEST(intervals, random_fill) {
	testOverhead = true;
	interval_map<KHolder, VHolder> map('-');
	srand(static_cast<unsigned>(time(nullptr)));
	testOverhead = false;
	// assign 1000 random intervals
	EXPECT_TRUE(is_valid_interval_map(map));
	for (int i = 0; i < 1000; ++i) {
		int start = rand() % 1000;
		int end = start + rand() % 1000;
		const char val = static_cast<char>(rand() % 256);
		map.assign(start, end, VHolder(val));
		EXPECT_TRUE(is_valid_interval_map(map));
	}
}

TEST(intervals, types_default_constructor) {
	interval_map<int, char> map('-');
	EXPECT_TRUE(map.intervals().empty());
	EXPECT_EQ(map[0], '-');
	EXPECT_EQ(map[-1], '-');
	EXPECT_EQ(map[1000], '-');
}

TEST(intervals, types_constrained_constructor) {
	testOverhead = true;
	interval_map<KHolder, VHolder> map('-');
	testOverhead = false;
	EXPECT_TRUE(map.intervals().empty());
	EXPECT_EQ(map[0], '-');
	EXPECT_EQ(map[-1], '-');
	EXPECT_EQ(map[1000], '-');
}

TEST(intervals, types_constrained_assigned) {
	testOverhead = true;
	interval_map<KHolder, VHolder> map('-');
	map.assign(10, 100, VHolder('A'));
	testOverhead = false;
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map[0], '-');
	EXPECT_EQ(map[-1], '-');
	EXPECT_EQ(map[1000], '-');
}

TEST(intervals, new_interval_empty) {
	interval_map<int, char> map('-');
	EXPECT_TRUE(map.intervals().empty());
}

TEST(intervals, insert_default_on_empty) {
	interval_map<int, char> map('-');
	map.assign(10, 20, '-');
	EXPECT_TRUE(map.intervals().empty());
}

TEST(intervals, insert_on_empty) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');

	EXPECT_EQ(2, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, '-' } }));
}

TEST(intervals, insert_default_before_first) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(5, 10, '-');
	EXPECT_EQ(2, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, '-' } }));
}


TEST(intervals, insert_default_before_first2) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(0, 5, '-');
	EXPECT_EQ(2, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, '-' } }));
}

TEST(intervals, insert_interval_before_first) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(0, 10, 'B');
	EXPECT_EQ(3, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {0, 'B'},  { 10, 'A' }, { 20, '-' } }));
}

TEST(intervals, insert_interval_before_first2) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(0, 5, 'B');
	EXPECT_EQ(4, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {0, 'B'}, { 5, '-' }, { 10, 'A' }, { 20, '-' } }));
}
TEST(intervals, insert_default_after_last) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(20, 25, '-');
	EXPECT_EQ(2, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, '-' } }));
}

TEST(intervals, insert_default_after_last2) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(25, 100, '-');
	EXPECT_EQ(2, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, '-' } }));
}

TEST(intervals, insert_consecutive_intervals) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(20, 30, 'B');
	map.assign(30, 40, 'C');
	map.assign(40, 50, 'D');
	map.assign(50, 60, 'E');
	EXPECT_EQ(6, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, 'B' }, { 30, 'C' }, { 40, 'D' }, { 50, 'E' }, { 60, '-' } }));
}

TEST(intervals, insert_non_consecutive_intervals) {
	interval_map<int, char> map('-');
	map.assign(10, 20, 'A');
	map.assign(25, 30, 'B');
	map.assign(35, 40, 'C');
	map.assign(45, 50, 'D');
	map.assign(55, 60, 'E');
	EXPECT_EQ(10, map.intervals().size());
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, '-' }, { 25, 'B' }, { 30, '-' }, { 35, 'C' }, { 40, '-' }, { 45, 'D' }, { 50, '-' }, { 55, 'E' }, { 60, '-' } }));
}


TEST(intervals, empty_map_returns_default) {
	interval_map<int, char> map('-');
	EXPECT_TRUE(map.intervals().empty());
	EXPECT_EQ(map[0], '-');
	EXPECT_EQ(map[-1], '-');
	EXPECT_EQ(map[1000], '-');
}

TEST(intervals, one_interval_returns_valid) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, {100, '-'} }));
	EXPECT_EQ(map[0], '-');

}

TEST(intervals, invalid_interval_not_added) {
	interval_map<int, char> map('-');
	map.assign(10, 10, 'A');
	map.assign(50, 10, 'B');
	EXPECT_TRUE(map.intervals().empty());
	EXPECT_EQ(map[0], '-');
}

TEST(intervals, two_intervals_returns_valid) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(50, 100, 'B');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 3);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, 'B' }, { 100, '-' } }));
	EXPECT_EQ(map[0], '-');
}

TEST(intervals, two_intervals_with_same_value_returns_valid) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(50, 100, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 100, '-' } }));
}

TEST(intervals, two_intervals_with_same_value_returns_valid2) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(100, 150, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 150, '-' } }));
}

TEST(intervals, two_intervals_with_same_value_returns_valid3) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(100, 150, 'A');
	map.assign(150, 200, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 200, '-' } }));
}

TEST(intervals, two_intervals_with_same_value_returns_valid4) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(100, 150, 'A');
	map.assign(150, 200, 'A');
	map.assign(200, 250, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 250, '-' } }));
}

TEST(intervals, add_redundant_interval_beginning) {
	interval_map<int, char> map('-');
	map.assign(10, 100, '-');
	EXPECT_TRUE(map.intervals().empty());
}

TEST(intervals, truncate_right) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(50, 100, '-');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, '-' } }));
}

TEST(intervals, add_redundant_interval_end) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(100, 150, '-');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 100, '-' } }));
}

TEST(intervals, add_redundant_interval_end2) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(100, 150, 'A');
	map.assign(150, 200, '-');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 150, '-' } }));
}

TEST(intervals, add_redundant_interval_middle_pastend) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(50, 150, 'A');
	map.assign(180, 200, '-');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 150, '-' } }));
}

TEST(intervals, add_redundant_interval_holes_middle_pastend2) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'A');
	map.assign(50, 150, 'A');
	map.assign(180, 200, 'A');
	map.assign(220, 250, '-');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 4);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 150, '-' }, { 180, 'A' }, { 200, '-' } }));
}

TEST(intervals, add_redundant_interval_holes_middle_pastend3) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'X');
	map.assign(5, 150, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {5, 'A'}, { 150, '-' }}));
}

TEST(intervals, add_redundant_interval_holes_middle_pastend4) {
	interval_map<int, char> map('-');
	map.assign(10, 100, 'X');
	map.assign(5, 150, 'A');
	map.assign(200, 250, 'B');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 4);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {5, 'A'}, { 150, '-' }, { 200, 'B' }, { 250, '-' }}));
}

TEST(intervals, add_redundant_interval_holes_middle) {
	interval_map<int, char> map('-');
	map.assign(0, 100, 'A');
	map.assign(100, 200, 'B');
	map.assign(20, 80, 'X');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 5);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {0, 'A'}, { 20, 'X' }, { 80, 'A' }, { 100, 'B' }, { 200, '-' }}));
}

TEST(intervals, extend_range_beginning) {
	interval_map<int, char> map('-');
	map.assign(50, 100, 'A');
	map.assign(10, 100, 'A');
	
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, {  100, '-' }}));
}

TEST(intervals, extend_range_end) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(20, 100, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 2);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 100, '-' }}));
}

TEST(intervals, extend_range_middle) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(50, 100, 'B');
	map.assign(30, 80, 'A');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 3);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 80, 'B' }, { 100, '-' }}));
}

TEST(intervals, extend_range_middle2) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(50, 100, 'B');
	map.assign(30, 80, 'B');
	EXPECT_FALSE(map.intervals().empty());
	EXPECT_EQ(map.intervals().size(), 3);
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 30, 'B' }, { 100, '-' }}));
}

TEST(intervals, remove_onlyitem) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(10, 50, '-');
	
	EXPECT_TRUE(map.intervals().empty());
}

TEST(intervals, remove_onlyitem2) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(100, 200, 'A');
	map.assign(10, 200, '-');

	EXPECT_TRUE(map.intervals().empty());
}

TEST(intervals, remove_onlyitem3) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(100, 200, 'B');
	map.assign(0, 2000, '-');

	EXPECT_TRUE(map.intervals().empty());
}

TEST(intervals, sequence_extra_end) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, '-' }}));
}

TEST(intervals, sequence_extra_end2) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(50, 60, 'B');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, 'B' }, {60, '-'}}));
}

TEST(intervals, sequence_extra_end3) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(50, 60, 'B');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, 'B' }, { 60, '-' }}));
}

TEST(intervals, sequence_extra_end4) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(50, 60, 'B');
	map.assign(60, 70, 'C');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, 'B' }, { 60, 'C' }, { 70, '-' }}));
}

TEST(intervals, sequence_extra_mid) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(60, 70, 'C');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 50, '-' }, { 60, 'C' }, { 70, '-' }}));
}

TEST(intervals, sequence_extra_mid2) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(60, 70, 'C');
	map.assign(20, 40, 'B');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, 'B' }, { 40, 'A' }, { 50, '-' }, { 60, 'C' }, { 70, '-' }}));
}

TEST(intervals, sequence_extra_mid3) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(60, 70, 'C');
	map.assign(20, 65, 'B');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'A'}, { 20, 'B' }, { 65, 'C' }, { 70, '-' }}));
}

TEST(intervals, sequence_extra_mid4) {
	interval_map<int, char> map('-');
	map.assign(10, 50, 'A');
	map.assign(60, 70, 'C');
	map.assign(10, 70, 'B');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'B' }, { 70, '-' }}));
}

TEST(intervals, sequence_extra_mid5) {
	interval_map<int, char> map('-');
	map.assign(15, 50, 'A');
	map.assign(60, 65, 'C');
	map.assign(10, 70, 'B');
	EXPECT_EQ(map.intervals(), (std::map<int, char>{ {10, 'B' }, { 70, '-' }}));
}

TEST(intervals, sequence_double_to_string) {
	const std::string def = "dead";
	interval_map<double, std::string> map(def);
	map.assign(-40, 10, std::string("get a jacket"));
	map.assign(10, 30, "cozy");
	map.assign(30, 60, "very hot");
	map.assign(60, 100, "melting");
	EXPECT_EQ(map[20], "cozy");
}