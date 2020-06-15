#include "./../common/aoc.hpp"
#include <regex>
#include <unordered_map>

const std::regex orbit("^([A-Z0-9]{1,3})\\)([A-Z0-9]{1,3})$");
const std::string root = "COM";
const std::string me = "YOU";
const std::string santa = "SAN";

typedef struct LEAVE {
	std::string name;
	std::vector<LEAVE> leaves;
} leave_str;

class AoC2019_day06 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	int32_t get_orbits_count();
	void parse_orbit(const std::string root, const int32_t depth, int32_t& sum);
	int32_t get_orbital_transfers_count();
	std::vector<std::string> get_path_from_orbit(const std::string orbit);
	std::unordered_map<std::string, std::vector<std::string>> map_;
	std::unordered_map<std::string, std::string> rev_map_;
};

bool AoC2019_day06::init(const std::vector<std::string> lines) {
	std::smatch sm;

	map_.clear();
	rev_map_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		if (std::regex_match(lines[i], sm, orbit)) {
			map_[sm.str(1)].push_back(sm.str(2));
			rev_map_[sm.str(2)] = sm.str(1);
		} else {
			std::cout << "Invalid input format at line " << i + 1 << std::endl;
			return false;
		}
	}

	return true;
}

int32_t AoC2019_day06::get_orbits_count() {
	int32_t sum;

	parse_orbit(root, 0, sum);

	return sum;
}

void AoC2019_day06::parse_orbit(const std::string root, const int32_t depth, int32_t& sum) {
	std::vector<std::string> items;

	sum += depth;

	items = map_[root];
	for (uint32_t i = 0; i < items.size(); i++) {
		parse_orbit(items[i], depth + 1, sum);
	}
}
std::vector<std::string> AoC2019_day06::get_path_from_orbit(const std::string orbit) {
	std::string next = orbit;
	std::vector<std::string> result = {};

	while (rev_map_.count(next)) {
		next = rev_map_[next];
		result.push_back(next);
	}

	std::reverse(result.begin(), result.end());

	return result;
}

int32_t AoC2019_day06::get_orbital_transfers_count() {
	std::vector<std::string> path1, path2;
	uint32_t idx = 0;

	path1 = get_path_from_orbit(me);
	path2 = get_path_from_orbit(santa);

	if (!path1.size() || !path2.size()) {
		return -1;
	}

	idx = 0;

	while (path1[idx] == path2[idx]) {
		idx++;

		if ((path1.size() <= idx) || (path2.size() <= idx)){
			return -1;
		}
	}

	return path1.size() - idx + path2.size() - idx;
}

int32_t AoC2019_day06::get_aoc_day() {
	return 6;
}

int32_t AoC2019_day06::get_aoc_year() {
	return 2019;
}

void AoC2019_day06::tests() {
	int32_t result = 0;

	if (init({"COM)B", "B)C", "C)D", "D)E", "E)F", "B)G", "G)H", "D)I", "E)J", "J)K", "K)L"})) {
		result = get_orbits_count();
	}
	if (init({"COM)B", "B)C", "C)D", "D)E", "E)F", "B)G", "G)H", "D)I", "E)J", "J)K", "K)L", "K)YOU", "I)SAN"})) {
		result = get_orbital_transfers_count();
	}
}

bool AoC2019_day06::part1() {
	int32_t result = 0;

	result = get_orbits_count();
	result1_ = std::to_string(result);
	return true;
}

bool AoC2019_day06::part2() {
	int32_t result = 0;

	result = get_orbital_transfers_count();
	result2_ = std::to_string(result);
	return true;
}

int main(void) {
	AoC2019_day06 day06;

	return day06.main_execution();
}
