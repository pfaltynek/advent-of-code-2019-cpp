#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include "./../common/string-utils.hpp"
#include <map>

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

class AoC2019_day11 : public AoC {
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
	std::map<coord_str, int32_t> map_;
	coord_str min_, max_;
};

void AoC2019_day11::reset() {
	ints_ = backup_;
}

bool AoC2019_day11::init(const std::vector<std::string> lines) {
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

void AoC2019_day11::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool AoC2019_day11::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
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
bool AoC2019_day11::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
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

bool AoC2019_day11::simulate_intcode(const int64_t input, int64_t& result) {
	uint64_t idx = 0;
	int64_t op1, op2;
	coord_str position = {};
	bool output_color = true;
	uint8_t head = 0; // up
	int32_t color;
	int32_t start_color;

	start_color = input;
	map_.clear();

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
				if (map_.count(position)) {
					color = map_[position];
				} else {
					color = start_color;
					start_color = 0;
				}
				set_int(ints_[idx + 1], param_mode[0], color);
				idx += 2;
				break;
			case opcodes_t::OP_OUT:
				get_int(ints_[idx + 1], param_mode[0], op1);
				if (output_color) {
					map_[position] = op1;
					if (position.x < min_.x) {
						min_.x = position.x;
					}
					if (position.y < min_.y) {
						min_.y = position.y;
					}
					if (position.x > max_.x) {
						max_.x = position.x;
					}
					if (position.y > max_.y) {
						max_.y = position.y;
					}
				} else {
					if (op1) {
						head++;
					} else {
						head--;
					}
					head %= 4;

					switch (head) {
						case 0:
							position.y--;
							break;
						case 1:
							position.x++;
							break;
						case 2:
							position.y++;
							break;
						case 3:
							position.x--;
							break;
					}
				}
				output_color = !output_color;
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
				result = map_.size();
				return true;
			default:
				return false;
		}
	}

	return false;
}

int32_t AoC2019_day11::get_aoc_day() {
	return 11;
}

int32_t AoC2019_day11::get_aoc_year() {
	return 2019;
}

void AoC2019_day11::tests() {
}

bool AoC2019_day11::part1() {
	int64_t result = 0;

	if (simulate_intcode(0, result)) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day11::part2() {
	int64_t result = 0;

	reset();
	if (simulate_intcode(1, result)) {
		result2_.clear();
		result2_.append("\n");
		for (int32_t i = min_.y; i <= max_.y; i++) {
			for (int32_t j = min_.x; j <= max_.x; j++) {
				if (map_[{j, i}]) {
					result2_.append("#");
				} else {
					result2_.append(" ");
				}
			}
			result2_.append("\n");
		}

		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day11 day11;

	return day11.main_execution();
}
