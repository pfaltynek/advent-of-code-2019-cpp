#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include "./../common/string-utils.hpp"
#include <map>
#include <queue>
#include <set>

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

typedef enum MAP_ITEM { MAP_WALL = 0, MAP_FREE = 1, MAP_OXYGEN = 2 } map_item_t;

typedef enum COMMAND { COMMAND_NORTH = 1, COMMAND_SOUTH = 2, COMMAND_WEST = 3, COMMAND_EAST = 4 } command_t;

class AoC2019_day15 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool build_map(coord_str& oxygen);
	bool get_shortest_path(coord_str oxygen, int64_t& result);
	bool get_oxygen_spread_time(coord_str oxygen, int64_t& result);
	bool simulate_intcode(const int64_t input, int64_t& result);
	bool get_int(const uint64_t idx, const int32_t param_mode, int64_t& value);
	bool set_int(const uint64_t idx, const int32_t param_mode, const int64_t value);
	void reset();
	void safe_memory(uint64_t idx);

	std::vector<int64_t> ints_, backup_;
	int64_t relative_base_;
	std::map<coord_str, map_item_t> map_;
	std::vector<command_t> directions_ = {COMMAND_NORTH, COMMAND_SOUTH, COMMAND_WEST, COMMAND_EAST};
	std::map<command_t, coord_str> steps_ = {
		{COMMAND_NORTH, coord_step_north}, {COMMAND_SOUTH, coord_step_south}, {COMMAND_WEST, coord_step_west}, {COMMAND_EAST, coord_step_east}};
	coord_str oxygen_;
};

void AoC2019_day15::reset() {
	ints_ = backup_;
}

bool AoC2019_day15::init(const std::vector<std::string> lines) {
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

void AoC2019_day15::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool AoC2019_day15::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
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
bool AoC2019_day15::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
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

bool AoC2019_day15::simulate_intcode(const int64_t input, int64_t& result) {
	uint64_t idx = 0;
	int64_t op1, op2;
	bool output_set = false;

	relative_base_ = 0;

	while (!output_set) {
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
				result = op1;
				output_set = true;
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

bool AoC2019_day15::build_map(coord_str& oxygen) {
	coord_str position = {0, 0}, next, tmp;
	std::vector<command_t> history = {};
	std::map<command_t, command_t> step_back = {
		{COMMAND_NORTH, COMMAND_SOUTH}, {COMMAND_SOUTH, COMMAND_NORTH}, {COMMAND_EAST, COMMAND_WEST}, {COMMAND_WEST, COMMAND_EAST}};
	std::set<coord_str> explore = {};
	int64_t step_result;
	map_item_t map_item;
	bool result = false;

	map_.clear();
	reset();

	explore.insert(position);
	explore.insert(coord_step_north);
	explore.insert(coord_step_south);
	explore.insert(coord_step_west);
	explore.insert(coord_step_east);

	while (explore.size()) {
		bool direction_found = false;

		for (uint8_t i = 0; i < directions_.size(); i++) {
			next = position + steps_[directions_[i]];
			if (!map_.count(next)) {

				simulate_intcode(directions_[i], step_result);
				map_item = static_cast<map_item_t>(step_result);
				map_[next] = map_item;
				explore.erase(next);

				if (map_item == map_item_t::MAP_WALL) {
					continue;
				} else {
					history.push_back(directions_[i]);
					direction_found = true;

					if (map_item == map_item_t::MAP_OXYGEN) {
						oxygen = next;
						result = true;
					}

					for (uint8_t j = i + 1; j < directions_.size(); j++) {
						tmp = position + steps_[directions_[j]];
						if (!map_.count(tmp)) {
							explore.insert(tmp);
						}
					}
					position = next;
					break;
				}
			}
		}

		if (!direction_found) {
			if (history.size()) {
				command_t back = history.back();
				history.pop_back();
				back = step_back[back];
				simulate_intcode(back, step_result);
				if (step_result != map_item_t::MAP_WALL) {
					position = position + steps_[back];
				}
			}
		}
	}

	return result;
}

bool AoC2019_day15::get_shortest_path(coord_str oxygen, int64_t& result) {
	std::queue<std::pair<coord_str, int64_t>> q;
	std::set<coord_str> history = {};
	coord_str pos, next;
	int64_t steps;

	q.push({{0, 0}, 0});

	while (q.size()) {
		pos = q.front().first;
		steps = q.front().second;
		q.pop();

		if (pos == oxygen) {
			result = steps;
			return true;
		}
		if (map_[pos] == map_item_t::MAP_WALL) {
			continue;
		}
		if (history.count(pos)) {
			continue;
		}
		steps++;
		q.push(std::make_pair(pos + coord_step_north, steps));
		q.push(std::make_pair(pos + coord_step_south, steps));
		q.push(std::make_pair(pos + coord_step_west, steps));
		q.push(std::make_pair(pos + coord_step_east, steps));
		history.insert(pos);
	}

	return false;
}

bool AoC2019_day15::get_oxygen_spread_time(coord_str oxygen, int64_t& result) {
	std::set<coord_str> past = {}, news = {}, last = {};
	coord_str pos, next;

	result = 0;
	last.insert(oxygen);

	while (last.size()) {
		news = {};
		for (auto it = last.begin(); it != last.end(); it++) {
			pos = *it;

			for (uint8_t i = 0; i < directions_.size(); i++) {
				next = pos + steps_[directions_[i]];
				if (!last.count(next) && !past.count(next)) {
					if (map_[next] != MAP_WALL) {
						news.insert(next);
					}
				}
			}
		}
		if (news.size()) {
			result++;
		}
		past.insert(last.begin(), last.end());
		last = news;
	}

	return true;
}

int32_t AoC2019_day15::get_aoc_day() {
	return 15;
}

int32_t AoC2019_day15::get_aoc_year() {
	return 2019;
}

void AoC2019_day15::tests() {
}

bool AoC2019_day15::part1() {
	int64_t result = 0;

	if (build_map(oxygen_)) {
		if (get_shortest_path(oxygen_, result)) {
			result1_ = std::to_string(result);
			return true;
		}
	}

	return false;
}

bool AoC2019_day15::part2() {
	int64_t result = 0;

	if (get_oxygen_spread_time(oxygen_, result)) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day15 day15;

	return day15.main_execution();
}
