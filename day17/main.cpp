#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
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

typedef enum DIRECTION { DIRECTION_NORTH = '^', DIRECTION_SOUTH = 'v', DIRECTION_WEST = '<', DIRECTION_EAST = '>' } direction_t;

const std::string all_directions = "^v<>";
const uint32_t max_function_length = 20;

class AoC2019_day17 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool simulate_intcode(const bool part2, const std::vector<int32_t> part2_input, std::vector<std::string>& result1, int64_t& part2_result);
	int64_t get_alignment_params_sum();
	std::string explore_map();
	bool identify_functions(const std::string input, std::vector<std::string>& funcs);
	bool identify_rest2functions(const std::vector<std::string> input, std::vector<std::string>& funcs);
	std::string build_main_function(const std::string input, const std::vector<std::string> funcs);
	std::string format_function_data(const std::string data);
	uint32_t find_shortest_string(const std::vector<std::string> rests);
	bool convert_functions(const std::vector<std::string> funcs, std::vector<int32_t>& result);
	std::vector<std::string> split_multi(const std::string delimiter, std::vector<std::string> parts);
	bool set_map(const std::vector<std::string> map);
	bool is_scaffold(coord_str coord);
	bool get_int(const uint64_t idx, const int32_t param_mode, int64_t& value);
	bool set_int(const uint64_t idx, const int32_t param_mode, const int64_t value);
	void reset();
	void safe_memory(uint64_t idx);

	std::vector<int64_t> ints_, backup_;
	uint64_t ints_idx_;
	int64_t relative_base_;
	std::vector<std::string> map_;
	std::vector<coord_str> crosses_;
	coord_str robot_;
	direction_t direction_;
};

void AoC2019_day17::reset() {
	ints_ = backup_;
	relative_base_ = 0;
	ints_idx_ = 0;
}

bool AoC2019_day17::init(const std::vector<std::string> lines) {
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

void AoC2019_day17::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool AoC2019_day17::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
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

bool AoC2019_day17::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
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

bool AoC2019_day17::simulate_intcode(const bool part2, const std::vector<int32_t> part2_input, std::vector<std::string>& result1, int64_t& part2_result) {
	uint64_t inp_idx = 0;
	int64_t op1, op2;
	std::string line = "";
	bool found = false;

	if (!ints_.size()) {
		return false;
	}

	if (part2) {
		ints_[0] = 2;
	} else {
		ints_[0] = 1;
	}

	result1.clear();

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
				if (part2) {
					if (inp_idx >= part2_input.size()) {
						return false;
					} else {
						set_int(ints_[ints_idx_ + 1], param_mode[0], part2_input[inp_idx]);
						inp_idx++;
					}
				} else {
					set_int(ints_[ints_idx_ + 1], param_mode[0], 0);
				}
				ints_idx_ += 2;
				break;
			case opcodes_t::OP_OUT:
				if (part2) {
					get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
					if (op1 > 255) {
						part2_result = op1;
						found = true;
					}
				} else {
					get_int(ints_[ints_idx_ + 1], param_mode[0], op1);
					if (op1 == 10) {
						if (line.size()) {
							result1.push_back(line);
						}
						line.clear();
					} else {
						if (all_directions.find_first_of(op1) != std::string::npos) {
							found = true;
							robot_ = {static_cast<int32_t>(line.size()), static_cast<int32_t>(result1.size())};
							direction_ = static_cast<direction_t>(op1);
						}
						line += op1;
					}
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

int64_t AoC2019_day17::get_alignment_params_sum() {
	int64_t result = 0;

	crosses_.clear();
	for (uint32_t i = 1; i < map_.size() - 1; i++) {
		for (uint32_t j = 1; j < map_[i].size() - 1; j++) {
			if (map_[i][j] == '#') {
				if ((map_[i - 1][j] == '#') && (map_[i + 1][j] == '#') && (map_[i][j - 1] == '#') && (map_[i][j + 1] == '#')) {
					result += i * j;
					crosses_.push_back({static_cast<int32_t>(j), static_cast<int32_t>(i)});
				}
			}
		}
	}

	return result;
}

bool AoC2019_day17::set_map(const std::vector<std::string> map) {
	uint32_t size = 0;
	bool robot_not_found = true;
	size_t pos;

	for (uint32_t i = 0; i < map.size(); i++) {
		if (!i) {
			size = map[i].size();
		} else {
			if (size != map[i].size()) {
				return false;
			}
		}
	}

	for (uint32_t i = 0; i < map.size(); i++) {
		// std::cout << map[i].c_str() << std::endl;

		pos = map[i].find_first_of(all_directions);

		if (pos != std::string::npos) {
			direction_ = static_cast<direction_t>(map[i][pos]);
			robot_ = {static_cast<int32_t>(pos), static_cast<int32_t>(i)};
			robot_not_found = false;
		}
	}

	if (robot_not_found) {
		return false;
	}

	map_ = map;

	return true;
}

bool AoC2019_day17::is_scaffold(coord_str coord) {
	if (static_cast<uint32_t>(coord.y) >= map_.size()) {
		return false;
	}
	if (map_.size()) {
		if (static_cast<uint32_t>(coord.x) >= map_[0].size()) {
			return false;
		}
	} else {
		return false;
	}
	return (map_[coord.y][coord.x] == '#');
}

std::string AoC2019_day17::explore_map() {
	std::string result = {};
	direction_t direction = direction_, next_direction_left, next_direction_right, next_direction;
	coord_str ahead, left, right, position = robot_;
	int8_t rotate = 0, rot = 0;
	uint32_t steps = 0;

	while (true) {
		switch (direction) {
			case DIRECTION_NORTH:
				ahead = coord_step_north;
				left = coord_step_west;
				right = coord_step_east;
				next_direction_left = DIRECTION_WEST;
				next_direction_right = DIRECTION_EAST;
				break;
			case DIRECTION_SOUTH:
				ahead = coord_step_south;
				left = coord_step_east;
				right = coord_step_west;
				next_direction_left = DIRECTION_EAST;
				next_direction_right = DIRECTION_WEST;
				break;
			case DIRECTION_WEST:
				ahead = coord_step_west;
				left = coord_step_south;
				right = coord_step_north;
				next_direction_left = DIRECTION_SOUTH;
				next_direction_right = DIRECTION_NORTH;
				break;
			case DIRECTION_EAST:
				ahead = coord_step_east;
				left = coord_step_north;
				right = coord_step_south;
				next_direction_left = DIRECTION_NORTH;
				next_direction_right = DIRECTION_SOUTH;
				break;
		}

		ahead = position + ahead;
		left = position + left;
		right = position + right;

		if (is_scaffold(ahead)) {
			rot = 0;
			next_direction = direction;
			position = ahead;
		} else if (is_scaffold(left)) {
			rot = -1;
			next_direction = next_direction_left;
			position = left;
		} else if (is_scaffold(right)) {
			rot = 1;
			next_direction = next_direction_right;
			position = right;
		} else {
			if (steps) {
				result.push_back(rotate * steps);
			}
			break;
		}

		if (next_direction != direction) {
			if (steps) {
				result.push_back(rotate * steps);
				steps = 0;
			}
			rotate = rot;
			direction = next_direction;
		}
		steps++;
	}

	// std::cout << format_function_data(result) << std::endl;

	return result;
}

bool AoC2019_day17::identify_functions(const std::string input, std::vector<std::string>& funcs) {
	std::string tmp, tmp2;
	std::vector<std::string> rest;

	funcs = {"", "", "", ""};

	for (size_t i = 0; i < input.size() / 2; i++) {
		for (size_t l = 2; l < input.size() - i; l++) {
			tmp = input.substr(i, l);
			tmp2 = format_function_data(tmp);

			if (tmp2.size() > max_function_length) {
				continue;
			}

			funcs[1] = tmp2;
			rest = split(input, tmp);

			if (rest.size() == 2) { // speed hack from data analysis
				if (identify_rest2functions(rest, funcs)) {
					funcs[0] = build_main_function(input, funcs);

					return true;
				}
			}
		}
	}
	return false;
}

bool AoC2019_day17::identify_rest2functions(const std::vector<std::string> input, std::vector<std::string>& funcs) {
	std::string tmp, tmp2;

	// find the shortest rest part
	uint32_t idx = find_shortest_string(input);
	tmp = input[idx];
	tmp2 = format_function_data(tmp);

	if (tmp2.size() > max_function_length) {
		return false;
	}

	funcs[2] = tmp2;

	// remove it from rest
	std::vector<std::string> rests = split_multi(tmp, input);

	if (rests.size() >= 2) { // speed hack from data analysis
		for (size_t i = 1; i < rests.size(); i++) {
			if (rests[0] != rests[i]) {
				return false;
			}
		}
		tmp = rests[0];
		tmp2 = format_function_data(tmp);

		if (tmp2.size() > max_function_length) {
			return false;
		}

		funcs[3] = tmp2;

		return true;
	}

	return false;
}

std::string AoC2019_day17::build_main_function(const std::string input, const std::vector<std::string> funcs) {
	std::string result = {}, tmp = format_function_data(input);

	while (tmp.size()) {
		for (uint8_t i = 1; i < funcs.size(); i++) {
			if (funcs[i].size()) {
				if (tmp.rfind(funcs[i], 0) == 0) {
					size_t pos = funcs[i].size() + 1;
					if (pos > tmp.size()) {
						tmp.clear();
					} else {
						tmp = tmp.substr(pos, tmp.size());
					}
					if (result.size()) {
						result.append(",");
					}
					result += ('A' + i - 1);
					break;
				}
			}
		}
	}

	return result;
}

std::vector<std::string> AoC2019_day17::split_multi(const std::string delimiter, std::vector<std::string> parts) {
	std::vector<std::string> result = {}, tmp;

	for (uint32_t i = 0; i < parts.size(); i++) {
		tmp = split(parts[i], delimiter);
		result.insert(result.end(), tmp.begin(), tmp.end());
	}

	return result;
}

uint32_t AoC2019_day17::find_shortest_string(const std::vector<std::string> rests) {
	uint32_t len = rests[0].size();
	uint32_t idx = 0;

	for (size_t j = 1; j < rests.size(); j++) {
		if (rests[j].size() < len) {
			len = rests[j].size();
			idx = j;
		}
	}

	return idx;
}

std::string AoC2019_day17::format_function_data(const std::string data) {
	std::string result = "";

	for (uint32_t i = 0; i < data.size(); i++) {
		if (result.size()) {
			result.append(",");
		}
		if (data[i] < 0) {
			result.append("L,");
		} else {
			result.append("R,");
		}
		result.append(std::to_string(abs(data[i])));
	}
	return result;
}

bool AoC2019_day17::convert_functions(const std::vector<std::string> funcs, std::vector<int32_t>& result) {
	if (funcs.size() != 4) {
		return false;
	}

	result.clear();

	for (uint8_t i = 0; i < 4; ++i) {
		for (uint8_t j = 0; j < funcs[i].size(); ++j) {
			result.push_back(funcs[i][j]);
		}
		result.push_back('\n');
	}

	result.push_back('n');
	result.push_back('\n');

	return true;
}

int32_t AoC2019_day17::get_aoc_day() {
	return 17;
}

int32_t AoC2019_day17::get_aoc_year() {
	return 2019;
}

void AoC2019_day17::tests() {
	int64_t result = 0;
	std::string data;
	std::vector<std::string> funcs;

	if (set_map({"..#..........", "..#..........", "#######...###", "#.#...#...#.#", "#############", "..#...#...#..", "..#####...^.."})) {
		result = get_alignment_params_sum(); // 76
	}

	if (set_map({"#######...#####", "#.....#...#...#", "#.....#...#...#", "......#...#...#", "......#...###.#", "......#.....#.#", "^########...#.#",
				 "......#.#...#.#", "......#########", "........#...#..", "....#########..", "....#...#......", "....#...#......", "....#...#......",
				 "....#####......"})) {
		data = explore_map();
		if (identify_functions(data, funcs)) {
			// = {"R,8,R,8", "R,8,L,6,L,2", "R,4,R,4", "A,C,B,C,A,B"}
		}
	}
}

bool AoC2019_day17::part1() {
	int64_t result = 0, tmp;
	std::vector<std::string> map;

	if (simulate_intcode(false, {}, map, tmp)) {
		if (set_map(map)) {
			result = get_alignment_params_sum();
			result1_ = std::to_string(result);
			return true;
		}
	}

	return false;
}

bool AoC2019_day17::part2() {
	int64_t result = 0;
	std::string data;
	std::vector<std::string> funcs;
	std::vector<int32_t> fcs;

	data = explore_map();

	if (identify_functions(data, funcs)) {
		if (convert_functions(funcs, fcs)) {
			if (simulate_intcode(true, fcs, funcs, result)) {
				result2_ = std::to_string(result);
				return true;
			}
		}
	}

	return false;
}

int main(void) {
	AoC2019_day17 day17;

	return day17.main_execution();
}
