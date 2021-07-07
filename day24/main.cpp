#include "./../common/aoc.hpp"
#include <unordered_set>

const int32_t C_MAX_BUGS_COUNT = 25;
const uint8_t C_NEIGHBORS_COUNT = 4;
const uint32_t C_BUGS_MASK = (1 << C_MAX_BUGS_COUNT) - 1;
const std::vector<std::vector<int32_t>> C_NEIGHBORS_MAP = {
	{1, 5},			{-1, 1, 5},	 {-1, 1, 5},	 {-1, 1, 5},	 {-1, 5},		 {-5, 1, 5},  {-5, -1, 1, 5}, {-5, -1, 1, 5}, {-5, -1, 1, 5},
	{-5, -1, 5},	{-5, 1, 5},	 {-5, -1, 1, 5}, {-5, -1, 1, 5}, {-5, -1, 1, 5}, {-5, -1, 5}, {-5, 1, 5},	  {-5, -1, 1, 5}, {-5, -1, 1, 5},
	{-5, -1, 1, 5}, {-5, -1, 5}, {-5, 1},		 {-5, -1, 1},	 {-5, -1, 1},	 {-5, -1, 1}, {-5, -1}};

class AoC2019_day24 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	uint32_t init_biodiversity_;
	uint32_t get_next_biodiversity(const uint32_t initial_state) const;
	void print_area(const uint32_t state, std::vector<std::string>& printout);
	uint32_t get_first_duplicate_biodiversity();
};

bool AoC2019_day24::init(const std::vector<std::string> lines) {
	uint32_t mask = 1;
	std::vector<std::string> area_printout;
	init_biodiversity_ = 0;

	// input must be 5 rows of 5 characters '#' or '.'
	if (lines.size() != 5) {
		std::cout << "Invalid input lines count " << std::endl;
		return false;
	}

	for (uint32_t i = 0; i < lines.size(); i++) {
		if (lines[i].size() != 5) {
			std::cout << "Invalid input size at line " << i + 1 << std::endl;
			return false;
		}

		for (size_t j = 0; j < lines[i].size(); j++) {
			switch (lines[i][j]) {
				case '.':
					break;
				case '#':
					init_biodiversity_ |= mask;
					break;
				default:
					std::cout << "Invalid input character at line " << i + 1 << ", position " << j + 1 << std::endl;
					return false;
					break;
			}
			mask = mask << 1;
		}
	}

	return true;
}

uint32_t AoC2019_day24::get_next_biodiversity(const uint32_t initial_biodiversity) const {
	uint32_t result = 0, mask, neg_mask;
	int32_t new_idx;
	uint8_t neighbors_bugs;

	result = initial_biodiversity;

	for (int32_t i = 0; i < static_cast<int32_t>(C_NEIGHBORS_MAP.size()); i++) {
		neighbors_bugs = 0;

		for (uint8_t j = 0; j < C_NEIGHBORS_MAP[i].size(); j++) {
			new_idx = i + C_NEIGHBORS_MAP[i][j];
			mask = 1;
			mask = mask << new_idx;

			if (mask & initial_biodiversity) {
				neighbors_bugs++;
			}
		}

		mask = 1 << i;
		neg_mask = ~mask & (C_BUGS_MASK);

		if ((mask & initial_biodiversity) != 0) {
			if (neighbors_bugs != 1) {
				result &= neg_mask;
			}
		} else {
			if ((neighbors_bugs >= 1) & (neighbors_bugs <= 2)) {
				result |= mask;
			}
		}
	}

	return result;
}

uint32_t AoC2019_day24::get_first_duplicate_biodiversity() {
	std::unordered_set<uint32_t> map;
	uint32_t result;

	result = init_biodiversity_;
	while(true) {
		map.emplace(result);
		result = get_next_biodiversity(result);
		if (map.count(result) > 0) {
			break;
		}
	}

	return result;
}

void AoC2019_day24::print_area(const uint32_t state, std::vector<std::string>& printout) {
	uint32_t cnt = 0, mask;
	std::string line = "";

	printout.clear();

	for (size_t i = 0; i < C_MAX_BUGS_COUNT; i++) {
		mask = 1 << i;

		if ((mask & state) != 0) {
			line += '#';
		} else {
			line += '.';
		}

		if (++cnt == 5) {
			cnt = 0;
			printout.push_back(line);
			line = "";
		}
	}
}

void AoC2019_day24::tests() {
	uint32_t result;
	std::vector<std::string> printout;

	if (init({".....", ".....", ".....", "#....", ".#..."})) {
		// 2129920
	}

	if (init({"....#", "#..#.", "#..##", "..#..", "#...."})) {
		result = init_biodiversity_;
		print_area(result, printout);
		// Initial state:
		//....#
		//#..#.
		//#..##
		//..#..
		//#....
		result = get_next_biodiversity(result);
		print_area(result, printout);
		// After 1 minute:
		// #..#.
		// ####.
		// ###.#
		// ##.##
		// .##..
		result = get_next_biodiversity(result);
		print_area(result, printout);
		// After 2 minutes:
		// #####
		// ....#
		// ....#
		// ...#.
		// #.###
		result = get_next_biodiversity(result);
		print_area(result, printout);
		// After 3 minutes:
		// #....
		// ####.
		// ...##
		// #.##.
		// .##.#
		result = get_next_biodiversity(result);
		print_area(result, printout);
		// After 4 minutes:
		// ####.
		// ....#
		// ##..#
		// .....
		// ##...
	}

	if (init({"....#", "#..#.", "#..##", "..#..", "#...."})) {
		result = get_first_duplicate_biodiversity();
	}
}

bool AoC2019_day24::part1() {

	result1_ = std::to_string(get_first_duplicate_biodiversity());

	return true;
}

bool AoC2019_day24::part2() {

	result2_ = std::to_string(0);

	return true;
}

int32_t AoC2019_day24::get_aoc_day() {
	return 24;
}

int32_t AoC2019_day24::get_aoc_year() {
	return 2019;
}

int main(void) {
	AoC2019_day24 day24;

	return day24.main_execution();
}
