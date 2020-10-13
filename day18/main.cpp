#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include <map>

const char Entrance = '@';
class AoC2019_day18 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	std::map<coord_str, char> map_;
	std::map<char, coord_str> keys_, doors_;
	coord_str entrance_;
};

bool AoC2019_day18::init(const std::vector<std::string> lines) {
	uint32_t row_len = 0;
	bool entrance_found = false;

	map_.clear();
	keys_.clear();
	doors_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		if (!i) {
			row_len = lines[i].size();
		} else {
			if (row_len != lines[i].size()) {
				return false;
			}
		}
		for (uint32_t j = 0; j < lines[i].size(); j++) {
			char c = lines[i][j];
			if (c == '#') {
				continue;
			} else if (c == '.') {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
			} else if (c == '@') {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				entrance_ = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
				entrance_found = true;
			} else if ((c >= 'a') && (c <= 'z')) {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				keys_[c] = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
			} else if ((c >= 'A') && (c <= 'Z')) {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				doors_[c] = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
			} else {
				std::cout << "Invalid map character at row " << i + 1 << " column " << j + 1 << "." << std::endl;
				return false;
			}
		}
	}

	return entrance_found;
}

int32_t AoC2019_day18::get_aoc_day() {
	return 18;
}

int32_t AoC2019_day18::get_aoc_year() {
	return 2019;
}

void AoC2019_day18::tests() {
}

bool AoC2019_day18::part1() {
	int64_t result = 0;

	if (1) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day18::part2() {
	int64_t result = 0;
	if (1) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day18 day18;

	return day18.main_execution();
}
