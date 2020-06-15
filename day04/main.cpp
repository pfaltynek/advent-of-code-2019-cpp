#include "./../common/aoc.hpp"
#include <regex>
#include <unordered_map>

const std::regex limits("^(\\d+)-(\\d+)$");

class AoC2019_day04 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool check_digits(const int32_t password);
	bool check_digits2(const int32_t password);
	int32_t find_password_count();
	int32_t find_password_count2();

	int32_t min_, max_;
};

bool AoC2019_day04::init(const std::vector<std::string> lines) {
	std::smatch sm;

	if (lines.size() < 1) {
		std::cout << "Insufficient input size - required 1 line" << std::endl;
		return false;
	}

	if (std::regex_match(lines[0], sm, limits)) {
		min_ = stoi(sm.str(1));
		max_ = stoi(sm.str(2));
	} else {
		std::cout << "Invalid input format" << std::endl;
		return false;
	}

	return true;
}

bool AoC2019_day04::check_digits(const int32_t password) {
	std::string tmp = std::to_string(password);
	bool same = false;

	for (uint32_t i = 0; i < tmp.size() - 1; i++) {
		if (!same) {
			if (tmp[i] == tmp[i + 1]) {
				same = true;
			}
		}
		if (tmp[i] > tmp[i + 1]) {
			return false;
		}
	}

	return same;
}

bool AoC2019_day04::check_digits2(const int32_t password) {
	std::string tmp = std::to_string(password);
	bool same = false;

	for (uint32_t i = 0; i < tmp.size() - 1; i++) {
		if (!same) {
			if (tmp[i] == tmp[i + 1]) {
				if (std::count(tmp.begin(), tmp.end(), tmp[i]) == 2) {
					same = true;
				}
			}
		}
		if (tmp[i] > tmp[i + 1]) {
			return false;
		}
	}

	return same;
}

int32_t AoC2019_day04::find_password_count() {
	int32_t result = 0;

	for (int32_t i = min_; i <= max_; i++) {
		if (check_digits(i)) {
			result++;
		}
	}

	return result;
}

int32_t AoC2019_day04::find_password_count2() {
	int32_t result = 0;

	for (int32_t i = min_; i <= max_; i++) {
		if (check_digits2(i)) {
			result++;
		}
	}

	return result;
}

int32_t AoC2019_day04::get_aoc_day() {
	return 4;
}

int32_t AoC2019_day04::get_aoc_year() {
	return 2019;
}

void AoC2019_day04::tests() {
	int32_t result = 0;

	if (check_digits(111111)) { // OK
		result++;
	}

	if (check_digits(223450)) { // KO
		result++;
	}

	if (check_digits(123789)) { // KO
		result++;
	}

	if (check_digits2(112233)) { // OK
		result++;
	}

	if (check_digits2(123444)) { // KO
		result++;
	}

	if (check_digits2(111122)) { // OK
		result++;
	}
}

bool AoC2019_day04::part1() {
	int32_t result = 0;

	result = find_password_count();
	result1_ = std::to_string(result);

	return true;
}

bool AoC2019_day04::part2() {
	int32_t result = 0;

	result = find_password_count2();
	result2_ = std::to_string(result);

	return true;
}

int main(void) {
	AoC2019_day04 day04;

	return day04.main_execution();
}
