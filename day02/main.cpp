#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include <unordered_map>

typedef enum OPCODES { OP_ADD = 1, OP_MUL = 2, OP_FIN = 99 } opcodes_t;

class AoC2019_day02 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool simulate_intcode(std::unordered_map<uint32_t, int32_t> init_program, int32_t& result);
	bool get_int(uint32_t idx, int32_t& value);
	bool set_int(uint32_t idx, int32_t value);
	void reset();

	std::vector<int32_t> ints_, backup_;
};

void AoC2019_day02::reset() {
	ints_ = backup_;
}

bool AoC2019_day02::init(const std::vector<std::string> lines) {
	ints_.clear();
	std::string line{};
	std::vector<std::string> items{};

	for (uint32_t i = 0; i < lines.size(); i++) {
		line.append(lines[i]);
	}

	items = split(line, ",");

	for (uint32_t i = 0; i < items.size(); i++) {
		ints_.push_back(std::stoi(items[i]));
	}

	backup_ = ints_;

	return true;
}

bool AoC2019_day02::get_int(uint32_t idx, int32_t& value) {
	if (idx >= ints_.size()) {
		return false;
	}

	value = ints_[idx];

	return true;
}
bool AoC2019_day02::set_int(uint32_t idx, int32_t value) {
	if (idx >= ints_.size()) {
		return false;
	}

	ints_[idx] = value;

	return true;
}

bool AoC2019_day02::simulate_intcode(std::unordered_map<uint32_t, int32_t> init_program, int32_t& result) {
	uint32_t idx = 0;
	int32_t op1, op2;

	for (auto it = init_program.begin(); it != init_program.end(); it++) {
		set_int(it->first, it->second);
	}

	while (true) {
		switch (static_cast<opcodes_t>(ints_[idx])) {
			case opcodes_t::OP_ADD:
				get_int(ints_[idx + 1], op1);
				get_int(ints_[idx + 2], op2);
				set_int(ints_[idx + 3], op1 + op2);
				idx += 4;
				break;
			case opcodes_t::OP_MUL:
				get_int(ints_[idx + 1], op1);
				get_int(ints_[idx + 2], op2);
				set_int(ints_[idx + 3], op1 * op2);
				idx += 4;
				break;
			case opcodes_t::OP_FIN:
				result = ints_[0];
				return true;
			default:
				return false;
		}
	}

	return false;
}

int32_t AoC2019_day02::get_aoc_day() {
	return 2;
}

int32_t AoC2019_day02::get_aoc_year() {
	return 2019;
}

void AoC2019_day02::tests() {
	int32_t result = 0;

	init({"1,9,10,3,2,3,11,0,99,30,40,50"});
	if (simulate_intcode({}, result)) {
		// 3500,9,10,70,2,3,11,0,99,30,40,50
	}

	init({"1,0,0,0,99"});
	if (simulate_intcode({}, result)) {
		// 2,0,0,0,99
	}

	init({"2,3,0,3,99"});
	if (simulate_intcode({}, result)) {
		// 2,3,0,6,99
	}

	init({"2,4,4,5,99,0"});
	if (simulate_intcode({}, result)) {
		// 2,4,4,5,99,9801
	}

	init({"1,1,1,4,99,5,6,0,99"});
	if (simulate_intcode({}, result)) {
		// 30,1,1,4,2,5,6,0,99
	}
}

bool AoC2019_day02::part1() {
	int32_t result = 0;

	if (simulate_intcode({{1, 12}, {2, 2}}, result)) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day02::part2() {
	int32_t result = 0;

	for (int32_t i = 0; i < 99; i++) {
		for (int32_t j = 0; j < 99; j++) {
			reset();

			if (simulate_intcode({{1, i}, {2, j}}, result)) {
				if (result == 19690720) {
					result2_ = std::to_string(i * 100 + j);
					return true;
				}
			}
		}
	}

	return false;
}

int main(void) {
	AoC2019_day02 day02;

	return day02.main_execution();
}
