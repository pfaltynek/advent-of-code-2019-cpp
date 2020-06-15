#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include <unordered_map>

typedef enum OPCODES {
	OP_ADD = 1,
	OP_MUL = 2,
	OP_INP = 3,
	OP_OUT = 4,
	OP_JNZ = 5,
	OP_JZ = 6,
	OP_LES = 7,
	OP_EQL = 8,
	OP_SET_REL_BASE = 9,
	OP_FIN = 99
} opcodes_t;

class AoC2019_day09 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool simulate_intcode(const int64_t input, int64_t& result);
	bool get_int(const uint64_t idx, const int32_t param_mode, int64_t& value);
	bool set_int(const uint64_t idx, const int32_t param_mode, const int64_t value);
	void reset();
	void safe_memory(uint64_t idx);

	std::vector<int64_t> ints_, backup_;
	int64_t relative_base_;
};

void AoC2019_day09::reset() {
	ints_ = backup_;
}

bool AoC2019_day09::init(const std::vector<std::string> lines) {
	ints_.clear();
	std::string line{};
	std::vector<std::string> items{};

	for (uint64_t i = 0; i < lines.size(); i++) {
		line.append(lines[i]);
	}

	items = split(line, ",");

	for (uint64_t i = 0; i < items.size(); i++) {
		ints_.push_back(std::stoll(items[i]));
	}

	backup_ = ints_;

	return true;
}

void AoC2019_day09::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool AoC2019_day09::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
	switch (param_mode) {
		case 2:
			safe_memory(relative_base_ + idx);
			value = ints_[relative_base_ + idx];
			break;
		case 1:
			value = idx;
			break;
		case 0:
			safe_memory(idx);
			value = ints_[idx];
			break;
		default:
			return false;
			break;
	}

	return true;
}
bool AoC2019_day09::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
	switch (param_mode) {
		case 2:
			safe_memory(relative_base_ + idx);
			ints_[relative_base_ + idx] = value;
			break;
		case 1:
			return false;
			break;
		case 0:
			safe_memory(idx);
			ints_[idx] = value;
			break;
		default:
			return false;
			break;
	}

	return true;
}

bool AoC2019_day09::simulate_intcode(const int64_t input, int64_t& result) {
	uint64_t idx = 0;
	int64_t op1, op2;
	std::vector<int64_t> outputs;

	outputs.clear();

	relative_base_ = 0;

	while (true) {
		int64_t opcode, param_modes, param_mode[3];

		opcode = ints_[idx] % 100;
		param_modes = ints_[idx] / 100;

		for (uint64_t i = 0; i < 3; i++) {
			param_mode[i] = param_modes % 10;
			param_modes /= 10;
		}

		switch (static_cast<opcodes_t>(opcode)) {
			case opcodes_t::OP_ADD:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], param_mode[2], op1 + op2);
				idx += 4;
				break;
			case opcodes_t::OP_MUL:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], param_mode[2], op1 * op2);
				idx += 4;
				break;
			case opcodes_t::OP_INP:
				set_int(ints_[idx + 1], param_mode[0], input);
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
				set_int(ints_[idx + 3], param_mode[2], op1 < op2 ? 1 : 0);
				idx += 4;
				break;
			case opcodes_t::OP_EQL:
				get_int(ints_[idx + 1], param_mode[0], op1);
				get_int(ints_[idx + 2], param_mode[1], op2);
				set_int(ints_[idx + 3], param_mode[2], op1 == op2 ? 1 : 0);
				idx += 4;
				break;
			case opcodes_t::OP_SET_REL_BASE:
				get_int(ints_[idx + 1], param_mode[0], op1);
				relative_base_ += op1;
				idx += 2;
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

int32_t AoC2019_day09::get_aoc_day() {
	return 9;
}

int32_t AoC2019_day09::get_aoc_year() {
	return 2019;
}

void AoC2019_day09::tests() {
	int64_t result = 0;

	if (init({"109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99"})) {
		simulate_intcode(1, result); // copies itself into output
	}

	if (init({"1102,34915192,34915192,7,4,7,99,0"})) {
		simulate_intcode(1, result); // outputs 16-digit number 1 219 070 632 396 864
	}

	if (init({"104,1125899906842624,99"})) {
		simulate_intcode(1, result); // outputs number 1125899906842624
	}

}

bool AoC2019_day09::part1() {
	int64_t result = 0;

	if (simulate_intcode(1, result)) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day09::part2() {
	int64_t result = 0;

	reset();
	if (simulate_intcode(2, result)) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day09 day09;

	return day09.main_execution();
}
