#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include <algorithm>
#include <unordered_map>

typedef enum OPCODES { OP_ADD = 1, OP_MUL = 2, OP_INP = 3, OP_OUT = 4, OP_JNZ = 5, OP_JZ = 6, OP_LES = 7, OP_EQL, OP_FIN = 99 } opcodes_t;

typedef struct AMPLIFIER {
	std::vector<int32_t> ram, io;
	uint32_t last_output, ip, io_idx;
} amplifier_str;

class AoC2019_day07 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	int32_t get_amplified_value(const std::vector<int32_t> phases);
	int32_t get_amplified_value_with_feedback(const std::vector<int32_t> phases);
	bool simulate_intcode(const std::vector<int32_t> inputs, const int32_t first_input, int32_t& result);
	bool get_int(uint32_t idx, const int32_t param_mode, const amplifier_str& amplifier, int32_t& value);
	bool set_int(uint32_t idx, int32_t value, amplifier_str& amplifier);
	void reset();

	std::vector<int32_t> ints_, backup_;
};

void AoC2019_day07::reset() {
	ints_ = backup_;
}

bool AoC2019_day07::init(const std::vector<std::string> lines) {
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

bool AoC2019_day07::get_int(const uint32_t idx, const int32_t param_mode, const amplifier_str& amplifier, int32_t& value) {
	if (param_mode) {
		value = idx;
	} else {
		if (idx >= amplifier.ram.size()) {
			return false;
		}
		value = amplifier.ram[idx];
	}

	return true;
}
bool AoC2019_day07::set_int(uint32_t idx, int32_t value, amplifier_str& amplifier) {
	if (idx >= amplifier.ram.size()) {
		return false;
	}

	amplifier.ram[idx] = value;

	return true;
}

bool AoC2019_day07::simulate_intcode(const std::vector<int32_t> inputs, const int32_t first_input, int32_t& result) {
	int32_t out_idx;
	int32_t op1, op2;
	std::vector<amplifier_str> amplifiers = {};

	reset();

	for (uint32_t i = 0; i < inputs.size(); i++) {
		amplifier_str amp;

		amp.ram = ints_;
		amp.io.clear();
		amp.ip = amp.io_idx = amp.last_output = 0;
		amp.io.push_back(inputs[i]);
		if (!i) {
			amp.io.push_back(first_input);
		}
		amplifiers.push_back(amp);
	}

	while (true) {
		for (uint32_t i = 0; i < amplifiers.size(); i++) {
			int32_t opcode, param_modes, param_mode[3];

			out_idx = (i + 1) % amplifiers.size();

			opcode = amplifiers[i].ram[amplifiers[i].ip] % 100;
			param_modes = amplifiers[i].ram[amplifiers[i].ip] / 100;

			for (uint32_t i = 0; i < 3; i++) {
				param_mode[i] = param_modes % 10;
				param_modes /= 10;
			}

			switch (static_cast<opcodes_t>(opcode)) {
				case opcodes_t::OP_ADD:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					get_int(amplifiers[i].ram[amplifiers[i].ip + 2], param_mode[1], amplifiers[i], op2);
					set_int(amplifiers[i].ram[amplifiers[i].ip + 3], op1 + op2, amplifiers[i]);
					amplifiers[i].ip += 4;
					break;
				case opcodes_t::OP_MUL:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					get_int(amplifiers[i].ram[amplifiers[i].ip + 2], param_mode[1], amplifiers[i], op2);
					set_int(amplifiers[i].ram[amplifiers[i].ip + 3], op1 * op2, amplifiers[i]);
					amplifiers[i].ip += 4;
					break;
				case opcodes_t::OP_INP:
					if (amplifiers[i].io.size() > amplifiers[i].io_idx) {
						set_int(amplifiers[i].ram[amplifiers[i].ip + 1], amplifiers[i].io[amplifiers[i].io_idx], amplifiers[i]);
						amplifiers[i].io_idx++;
						amplifiers[i].ip += 2;
					}
					break;
				case opcodes_t::OP_OUT:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					amplifiers[out_idx].io.push_back(op1);
					amplifiers[i].last_output = op1;
					amplifiers[i].ip += 2;
					break;
				case opcodes_t::OP_JNZ:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					if (op1) {
						get_int(amplifiers[i].ram[amplifiers[i].ip + 2], param_mode[1], amplifiers[i], op2);
						amplifiers[i].ip = op2;
					} else {
						amplifiers[i].ip += 3;
					}
					break;
				case opcodes_t::OP_JZ:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					if (!op1) {
						get_int(amplifiers[i].ram[amplifiers[i].ip + 2], param_mode[1], amplifiers[i], op2);
						amplifiers[i].ip = op2;
					} else {
						amplifiers[i].ip += 3;
					}
					break;
				case opcodes_t::OP_LES:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					get_int(amplifiers[i].ram[amplifiers[i].ip + 2], param_mode[1], amplifiers[i], op2);
					set_int(amplifiers[i].ram[amplifiers[i].ip + 3], op1 < op2 ? 1 : 0, amplifiers[i]);
					amplifiers[i].ip += 4;
					break;
				case opcodes_t::OP_EQL:
					get_int(amplifiers[i].ram[amplifiers[i].ip + 1], param_mode[0], amplifiers[i], op1);
					get_int(amplifiers[i].ram[amplifiers[i].ip + 2], param_mode[1], amplifiers[i], op2);
					set_int(amplifiers[i].ram[amplifiers[i].ip + 3], op1 == op2 ? 1 : 0, amplifiers[i]);
					amplifiers[i].ip += 4;
					break;
				case opcodes_t::OP_FIN:
					if ((i + 1) == amplifiers.size()) {
						result = amplifiers[i].last_output;
						return true;
					}
					break;
				default:
					return false;
			}
		}
	}

	return false;
}

int32_t AoC2019_day07::get_amplified_value(const std::vector<int32_t> phases) {
	int32_t result = 0, init_input = 0;

	for (uint32_t i = 0; i < phases.size(); i++) {
		std::vector<int32_t> inputs = {phases[i]};

		simulate_intcode(inputs, init_input, result);
		init_input = result;
	}

	return result;
}

int32_t AoC2019_day07::get_amplified_value_with_feedback(const std::vector<int32_t> phases) {
	int32_t result = 0, init_input = 0;

	simulate_intcode(phases, init_input, result);

	return result;
}

int32_t AoC2019_day07::get_aoc_day() {
	return 7;
}

int32_t AoC2019_day07::get_aoc_year() {
	return 2019;
}

void AoC2019_day07::tests() {
	int32_t result = 0;

	if (init({"3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0"})) {
		result = get_amplified_value({4, 3, 2, 1, 0}); // 43210
	}

	if (init({"3,23,3,24,1002,24,10,24,1002,23,-1,23,101,5,23,23,1,24,23,23,4,23,99,0,0"})) {
		result = get_amplified_value({0, 1, 2, 3, 4}); // 54321
	}

	if (init({"3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,1002,33,7,33,1,33,31,31,1,32,31,31,4,31,99,0,0,0"})) {
		result = get_amplified_value({1, 0, 4, 3, 2}); // 65210
	}

	if (init({"3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5"})) {
		result = get_amplified_value_with_feedback({9, 8, 7, 6, 5}); // 139629729
	}

	if (init({"3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,53,1001,56,"
			  "-1,56,1005,56,6,99,0,0,0,0,10"})) {
		result = get_amplified_value_with_feedback({9, 7, 8, 5, 6}); // 18216
	}
}

bool AoC2019_day07::part1() {
	int32_t result = 0, tmp;

	std::vector<int32_t> phases({0, 1, 2, 3, 4});

	do {
		tmp = get_amplified_value(phases);
		if (tmp > result) {
			result = tmp;
		}
	} while (std::next_permutation(phases.begin(), phases.end()));

	result1_ = std::to_string(result);
	return true;
}

bool AoC2019_day07::part2() {
	int32_t result = 0, tmp;

	std::vector<int32_t> phases({5, 6, 7, 8, 9});

	do {
		tmp = get_amplified_value_with_feedback(phases);
		if (tmp > result) {
			result = tmp;
		}
	} while (std::next_permutation(phases.begin(), phases.end()));

	result2_ = std::to_string(result);
	return true;
}

int main(void) {
	AoC2019_day07 day07;

	return day07.main_execution();
}
