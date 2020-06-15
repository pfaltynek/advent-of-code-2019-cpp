#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include <unordered_map>

typedef enum OPCODES { OP_ADD = 1, OP_MUL = 2, OP_INP = 3, OP_OUT = 4, OP_JNZ = 5, OP_JZ = 6, OP_LES = 7, OP_EQL, OP_FIN = 99 } opcodes_t;

class AoC2019_day05 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool simulate_intcode(const std::unordered_map<uint32_t, int32_t> init_program, const int32_t input, int32_t& result);
	bool get_int(uint32_t idx, const int32_t param_mode, int32_t& value);
	bool set_int(uint32_t idx, int32_t value);
	void reset();

	std::vector<int32_t> ints_, backup_;
};

void AoC2019_day05::reset() {
	ints_ = backup_;
}

bool AoC2019_day05::init(const std::vector<std::string> lines) {
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

bool AoC2019_day05::get_int(const uint32_t idx, const int32_t param_mode, int32_t& value) {
	if (param_mode) {
		value = idx;
	} else {
		if (idx >= ints_.size()) {
			return false;
		}
		value = ints_[idx];
	}

	return true;
}
bool AoC2019_day05::set_int(uint32_t idx, int32_t value) {
	if (idx >= ints_.size()) {
		return false;
	}

	ints_[idx] = value;

	return true;
}

bool AoC2019_day05::simulate_intcode(const std::unordered_map<uint32_t, int32_t> init_program, const int32_t input, int32_t& result) {
	uint32_t idx = 0;
	int32_t op1, op2;
	std::vector<int32_t> outputs;

	for (auto it = init_program.begin(); it != init_program.end(); it++) {
		set_int(it->first, it->second);
	}

	outputs.clear();

	while (true) {
		int32_t opcode, param_modes, param_mode[3];

		opcode = ints_[idx] % 100;
		param_modes = ints_[idx] / 100;

		for (uint32_t i = 0; i < 3; i++) {
			param_mode[i] = param_modes % 10;
			param_modes /= 10;
		}

		switch (static_cast<opcodes_t>(opcode)) {
			case opcodes_t::OP_ADD:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], op1 + op2);
				idx += 4;
				break;
			case opcodes_t::OP_MUL:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], op1 * op2);
				idx += 4;
				break;
			case opcodes_t::OP_INP:
				set_int(ints_[idx + 1], input);
				idx += 2;
				break;
			case opcodes_t::OP_OUT:
				get_int(ints_[idx + 1], param_mode[0], op1);
				outputs.push_back(op1);
				idx += 2;
				break;
			case opcodes_t::OP_JNZ:
				get_int(ints_[idx + 1], param_mode[0], op1);
				if (op1) {
					get_int(ints_[idx + 2], param_mode[1], op2);
					idx = op2;
				} else {
					idx += 3;
				}
				break;
			case opcodes_t::OP_JZ:
				get_int(ints_[idx + 1], param_mode[0], op1);
				if (!op1) {
					get_int(ints_[idx + 2], param_mode[1], op2);
					idx = op2;
				} else {
					idx += 3;
				}
				break;
			case opcodes_t::OP_LES:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], op1 < op2 ? 1 : 0);
				idx += 4;
				break;
			case opcodes_t::OP_EQL:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], op1 == op2 ? 1 : 0);
				idx += 4;
				break;
			case opcodes_t::OP_FIN:
				if (outputs.size()) {
					result = outputs[outputs.size() - 1];
					return true;
				} else {
					return false;
				}
			default:
				return false;
		}
	}

	return false;
}

int32_t AoC2019_day05::get_aoc_day() {
	return 5;
}

int32_t AoC2019_day05::get_aoc_year() {
	return 2019;
}

void AoC2019_day05::tests() {
	int32_t result = 0;

	if (init({"1002,4,3,4,33"})) {
		simulate_intcode({}, 1, result); // 1002,4,3,4,99
	}

	if (init({"3,9,8,9,10,9,4,9,99,-1,8"})) {
		if (simulate_intcode({}, 8, result)) { // 1
			reset();
			if (simulate_intcode({}, 7, result)) { // 0
			}
		}
	}
	if (init({"3,9,7,9,10,9,4,9,99,-1,8"})) {
		if (simulate_intcode({}, 8, result)) { // 0
			reset();
			if (simulate_intcode({}, 7, result)) { // 1
			}
		}
	}
	if (init({"3,3,1108,-1,8,3,4,3,99"})) {
		if (simulate_intcode({}, 8, result)) { // 1
			reset();
			if (simulate_intcode({}, 7, result)) { // 0
			}
		}
	}
	if (init({"3,3,1107,-1,8,3,4,3,99"})) {
		if (simulate_intcode({}, 8, result)) { // 0
			reset();
			if (simulate_intcode({}, 7, result)) { // 1
			}
		}
	}

	if (init({"3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9"})) {
		if (simulate_intcode({}, 0, result)) { // 0
			reset();
			if (simulate_intcode({}, 7, result)) { // 1
			}
		}
	}

	if (init({"3,3,1105,-1,9,1101,0,0,12,4,12,99,1"})) {
		if (simulate_intcode({}, 0, result)) { // 0
			reset();
			if (simulate_intcode({}, 5, result)) { // 1
			}
		}
	}

	if (init({"3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,1106,0,36,98,0,0,1002,21,125,20,4,20,1105,1,46,104,999,1105,1,46,1101,1000,1,20,4,20,1105,1,"
			  "46,98,99"})) {
		if (simulate_intcode({}, 7, result)) { // 999
			reset();
			if (simulate_intcode({}, 8, result)) { // 1000
				reset();
				if (simulate_intcode({}, 9, result)) { // 1001
				}
			}
		}
	}
}

bool AoC2019_day05::part1() {
	int32_t result = 0;

	if (simulate_intcode({}, 1, result)) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day05::part2() {
	int32_t result = 0;

	reset();
	if (simulate_intcode({}, 5, result)) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day05 day05;

	return day05.main_execution();
}
