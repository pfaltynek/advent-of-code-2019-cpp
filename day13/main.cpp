#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include "./../common/string-utils.hpp"
#include "./../common/terminal.hpp"
#include <map>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

void sleepcp(int milliseconds);

void sleepcp(int milliseconds) // Cross-platform sleep function
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif // _WIN32
}

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

class AoC2019_day13 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();
	void print_title();
	void print_part1_title();
	void print_part2_title();
	void print_part1_result();
	void print_part2_result();
	void print_time(int32_t time);

  private:
	bool simulate_intcode(int64_t& result, bool part1);
	bool get_int(const uint64_t idx, const int32_t param_mode, int64_t& value);
	bool set_int(const uint64_t idx, const int32_t param_mode, const int64_t value);
	void reset();
	void safe_memory(uint64_t idx);

	std::vector<int64_t> ints_, backup_;
	int64_t relative_base_;
	std::map<coord_str, int32_t> game_;
	coord_str output_max_, output_min_;
};

void AoC2019_day13::reset() {
	ints_ = backup_;
}

bool AoC2019_day13::init(const std::vector<std::string> lines) {
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

void AoC2019_day13::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool AoC2019_day13::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
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
bool AoC2019_day13::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
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

bool AoC2019_day13::simulate_intcode(int64_t& result, bool part1) {
	uint64_t idx = 0;
	int64_t op1, op2, input;
	std::vector<int64_t> outputs;
	coord_str padle, ball, prev_ball;

	outputs.clear();
	game_.clear();

	input = 0;

	if (part1) {
		output_max_.x = output_max_.y = INT32_MIN;
		output_min_.x = output_min_.y = INT32_MAX;
	} else {
		ints_[0] = 2;
	}

	relative_base_ = 0;
	result = 0;

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
				if (padle.x < ball.x) {
					input = 1;
				} else if (padle.x > ball.x) {
					input = -1;
				} else {
					input = 0;
				}

				sleepcp(20);

				set_int(ints_[idx + 1], param_mode[0], input);
				idx += 2;
				break;
			case opcodes_t::OP_OUT:
				get_int(ints_[idx + 1], param_mode[0], op1);
				outputs.push_back(op1);
				if (outputs.size() == 3) {
					game_[{static_cast<int32_t>(outputs[0]), static_cast<int32_t>(outputs[1])}] = static_cast<int32_t>(outputs[2]);
					if (part1) {
						if (outputs[0] > output_max_.x) {
							output_max_.x = outputs[0];
						}
						if (outputs[1] > output_max_.y) {
							output_max_.y = outputs[1];
						}
						if (outputs[0] < output_min_.x) {
							output_min_.x = outputs[0];
						}
						if (outputs[1] < output_min_.y) {
							output_min_.y = outputs[1];
						}

						if (outputs[2] == 2) {
							result++;
						}
					} else {
						if (outputs[0] == -1 && outputs[1] == 0) {
							result = outputs[2];
							SetTerminalCursorPosition(60, 10);
							std::cout << "Score:";
							SetTerminalCursorPosition(60, 11);
							std::cout << "                  ";
							SetTerminalCursorPosition(60, 11);
							std::cout << result;
						}
					}

					SetTerminalCursorPosition(outputs[0] + 2, outputs[1]);
					switch (outputs[2]) {
						case 1:
							std::cout << "X";
							break;
						case 2:
							std::cout << "=";
							break;
						case 3:
							std::cout << "-";
							padle = {static_cast<int32_t>(outputs[0]), static_cast<int32_t>(outputs[1])};
							break;
						case 4:
							std::cout << "o";
							prev_ball = ball;
							ball = {static_cast<int32_t>(outputs[0]), static_cast<int32_t>(outputs[1])};
							break;
						case 0:
						default:
							std::cout << " ";
							break;
					}

					outputs.clear();
				}
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
				return true;
			default:
				return false;
		}
	}

	return false;
}

void AoC2019_day13::print_title() {
	SetTerminalCursorPosition(46, 0);
	AoC::print_title();
}

void AoC2019_day13::print_part1_title() {
	SetTerminalCursorPosition(46, 1);
	AoC::print_part1_title();
}

void AoC2019_day13::print_part2_title() {
	SetTerminalCursorPosition(46, 3);
	AoC::print_part2_title();
}

void AoC2019_day13::print_part1_result() {
	SetTerminalCursorPosition(46, 2);
	AoC::print_part1_result();
}

void AoC2019_day13::print_part2_result() {
	SetTerminalCursorPosition(46, 4);
	AoC::print_part2_result();
}

void AoC2019_day13::print_time(int32_t time) {
	SetTerminalCursorPosition(46, 5);
	AoC::print_time(time);
	SetTerminalCursorPosition(0, output_max_.y + 1);
}

int32_t AoC2019_day13::get_aoc_day() {
	return 13;
}

int32_t AoC2019_day13::get_aoc_year() {
	return 2019;
}

void AoC2019_day13::tests() {
}

bool AoC2019_day13::part1() {
	int64_t result = 0;

	if (simulate_intcode(result, true)) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day13::part2() {
	int64_t result = 0;

	if (simulate_intcode(result, false)) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day13 day13;

	return day13.main_execution();
}
