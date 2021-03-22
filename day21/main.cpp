#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"

const std::string C_INSTRUCTIONS1 = "NOT A T\nNOT B J\nOR T J\nNOT C T\nOR T J\nAND D J\nWALK\n";
const std::string C_INSTRUCTIONS2 = "NOT B J\nNOT C T\nOR T J\nAND D J\nAND H J\nNOT A T\nOR T J\nRUN\n";

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

class AoC2019_day21 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool simulate_intcode(const std::string& input, int64_t& result);
	bool get_int(const uint64_t idx, const int32_t param_mode, int64_t& value);
	bool set_int(const uint64_t idx, const int32_t param_mode, const int64_t value);
	void reset();
	void safe_memory(uint64_t idx);

	std::vector<int64_t> ints_, backup_;
	uint64_t ints_idx_;
	int64_t relative_base_;
};

void AoC2019_day21::reset() {
	ints_ = backup_;
	relative_base_ = 0;
	ints_idx_ = 0;
}

bool AoC2019_day21::init(const std::vector<std::string> lines) {
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
	relative_base_ = 0;
	ints_idx_ = 0;

	return true;
}

void AoC2019_day21::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool AoC2019_day21::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
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

bool AoC2019_day21::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
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

bool AoC2019_day21::simulate_intcode(const std::string& input, int64_t& result) {
	uint64_t inp_idx = 0;
	int64_t op1, op2;
	bool found = false;

	result = 0;
	reset();

	if (!ints_.size()) {
		return false;
	}

	while (true) {
		int64_t opcode, param_modes, param_mode[3];

		opcode = ints_[ints_idx_] % 100;
		param_modes = ints_[ints_idx_] / 100;

		for (uint64_t i = 0; i < 3; i++) {
			param_mode[i] = param_modes % 10;
			param_modes /= 10;
		}

		switch (static_cast<opcodes_t>(opcode)) {
			case opcodes_t::OP_ADD:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				get_int(ints_[ints_idx_ + 2], param_mode[1], op2);
				set_int(ints_[ints_idx_ + 3], param_mode[2], op1 + op2);
				ints_idx_ += 4;
				break;
			case opcodes_t::OP_MUL:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				get_int(ints_[ints_idx_ + 2], param_mode[1], op2);
				set_int(ints_[ints_idx_ + 3], param_mode[2], op1 * op2);
				ints_idx_ += 4;
				break;
			case opcodes_t::OP_INP:
				if (inp_idx >= input.size()) {
					return false;
				} else {
					set_int(ints_[ints_idx_ + 1], param_mode[0], input[inp_idx]);
					inp_idx++;
				}
				ints_idx_ += 2;
				break;
			case opcodes_t::OP_OUT:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				if (op1 > 127) {
					result = op1;
					found = true;
				} else {
					//std::cout << static_cast<char>(op1);
				}
				ints_idx_ += 2;
				break;
			case opcodes_t::OP_JNZ:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				if (op1) {
					get_int(ints_[ints_idx_ + 2], param_mode[1], op2);
					ints_idx_ = op2;
				} else {
					ints_idx_ += 3;
				}
				break;
			case opcodes_t::OP_JZ:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				if (!op1) {
					get_int(ints_[ints_idx_ + 2], param_mode[1], op2);
					ints_idx_ = op2;
				} else {
					ints_idx_ += 3;
				}
				break;
			case opcodes_t::OP_LES:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				get_int(ints_[ints_idx_ + 2], param_mode[1], op2);
				set_int(ints_[ints_idx_ + 3], param_mode[2], op1 < op2 ? 1 : 0);
				ints_idx_ += 4;
				break;
			case opcodes_t::OP_EQL:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				get_int(ints_[ints_idx_ + 2], param_mode[1], op2);
				set_int(ints_[ints_idx_ + 3], param_mode[2], op1 == op2 ? 1 : 0);
				ints_idx_ += 4;
				break;
			case opcodes_t::OP_SET_REL_BASE:
				get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
				relative_base_ += op1;
				ints_idx_ += 2;
				break;
			case opcodes_t::OP_FIN:
				ints_idx_++;
				return found;
			default:
				return false;
		}
	}

	return false;
}

void AoC2019_day21::tests() {
	uint64_t result = 0;
}

bool AoC2019_day21::part1() {
	int64_t result;

	if (simulate_intcode(C_INSTRUCTIONS1, result)) {
		result1_ = std::to_string(result);

		return true;
	} else {
		return false;
	}
}

bool AoC2019_day21::part2() {
	int64_t result;

	if (simulate_intcode(C_INSTRUCTIONS2, result)) {
		result2_ = std::to_string(result);

		return true;
	} else {
		return false;
	}
}

int32_t AoC2019_day21::get_aoc_day() {
	return 21;
}

int32_t AoC2019_day21::get_aoc_year() {
	return 2019;
}

int main(void) {
	AoC2019_day21 day21;

	return day21.main_execution();
}
