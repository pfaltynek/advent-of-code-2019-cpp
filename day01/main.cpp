#include "./../common/aoc.hpp"

class AoC2019_day01 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	int32_t get_module_required_fuel(int32_t mass);
	int32_t get_module_required_fuel_sequence(int32_t mass);
	std::vector<int32_t> masses_;
};

bool AoC2019_day01::init(const std::vector<std::string> lines) {
	masses_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		masses_.push_back(std::stoi(lines[i], nullptr));
	}

	return true;
}

int32_t AoC2019_day01::get_module_required_fuel(int32_t mass) {
	return (mass / 3) - 2;
}

int32_t AoC2019_day01::get_module_required_fuel_sequence(int32_t mass) {
	int32_t sum = 0, fuel;

	fuel = get_module_required_fuel(mass);

	while (fuel > 0) {
		sum += fuel;
		fuel = get_module_required_fuel(fuel);
	}

	return sum;
}

int32_t AoC2019_day01::get_aoc_day() {
	return 1;
}

int32_t AoC2019_day01::get_aoc_year() {
	return 2019;
}

void AoC2019_day01::tests() {
	int32_t result = 0;

	result = get_module_required_fuel(12);	 // 2
	result = get_module_required_fuel(14);	 // 2
	result = get_module_required_fuel(1969);   // 654
	result = get_module_required_fuel(100756); // 33583

	result = get_module_required_fuel_sequence(14);		// 2
	result = get_module_required_fuel_sequence(1969);   // 966
	result = get_module_required_fuel_sequence(100756); // 50346
}

bool AoC2019_day01::part1() {
	int32_t result = 0;

	for (uint32_t i = 0; i < masses_.size(); ++i) {
		result += get_module_required_fuel(masses_[i]);
	}

	result1_ = std::to_string(result);

	return true;
}

bool AoC2019_day01::part2() {
	int32_t result = 0;

	for (uint32_t i = 0; i < masses_.size(); ++i) {
		result += get_module_required_fuel_sequence(masses_[i]);
	}

	result2_ = std::to_string(result);

	return true;
}

int main(void) {
	AoC2019_day01 day01;

	return day01.main_execution();
}
