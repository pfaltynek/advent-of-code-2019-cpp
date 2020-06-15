#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include "./../common/string-utils.hpp"
#include <regex>

const std::regex navigation("^[UDLR](\\d+)$");

typedef struct LINE {
	coord_str from, diff;

	bool is_horz() {
		return (diff.y == 0);
	}
} line_str;

typedef struct CROSS {
	coord_str cross_pt;
	uint32_t wire_line_idx[2];
} cross_str;

class AoC2019_day03 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool are_lines_crossed(const line_str horz, const line_str vert);
	bool find_nearest_cross_distance(int32_t& dist);
	bool find_nearest_cross_steps(int32_t& steps);
	void find_crosses();

	std::vector<line_str> wire_[2];
	std::vector<cross_str> crosses_;
};

bool AoC2019_day03::init(const std::vector<std::string> lines) {
	int32_t x, y, value;
	std::vector<std::string> items;
	std::smatch sm;
	std::vector<coord_str> crosses;

	wire_[0].clear();
	wire_[1].clear();

	if (lines.size() < 2) {
		std::cout << "Insufficient input size - required 2 lines" << std::endl;
		return false;
	}

	for (uint32_t i = 0; i < 2; i++) {
		items = split(lines[i], ",");
		x = y = 0;

		for (uint32_t j = 0; j < items.size(); j++) {
			if (std::regex_match(items[j], sm, navigation)) {
				value = stoi(sm.str(1));
				line_str line;

				line.from.x = x;
				line.from.y = y;
				line.diff.x = 0;
				line.diff.y = 0;
				switch (items[j][0]) {
					case 'U':
						line.diff.y += value;
						wire_[i].push_back(line);
						break;
					case 'D':
						line.diff.y -= value;
						wire_[i].push_back(line);
						break;
					case 'L':
						line.diff.x -= value;
						wire_[i].push_back(line);
						break;
					case 'R':
						line.diff.x += value;
						wire_[i].push_back(line);
						break;
				}

				x += line.diff.x;
				y += line.diff.y;
			} else {
				std::cout << "Invalid input - item " << j + 1 << " at line " << i + 1 << std::endl;
				return false;
			}
		}
	}

	find_crosses();

	return true;
}

void AoC2019_day03::find_crosses() {
	cross_str cross;
	crosses_.clear();

	for (uint32_t i = 0; i < wire_[0].size(); i++) {
		for (uint32_t j = 0; j < wire_[1].size(); j++) {
			if (!wire_[0][i].is_horz() && wire_[1][j].is_horz()) {
				if (are_lines_crossed(wire_[1][j], wire_[0][i])) {
					cross.cross_pt.x = wire_[0][i].from.x;
					cross.cross_pt.y = wire_[1][j].from.y;
					cross.wire_line_idx[0] = i;
					cross.wire_line_idx[1] = j;
					crosses_.push_back(cross);
				}
			} else if (wire_[0][i].is_horz() && !wire_[1][j].is_horz()) {
				if (are_lines_crossed(wire_[0][i], wire_[1][j])) {
					cross.cross_pt.x = wire_[1][j].from.x;
					cross.cross_pt.y = wire_[0][i].from.y;
					cross.wire_line_idx[0] = i;
					cross.wire_line_idx[1] = j;
					crosses_.push_back(cross);
				}
			}
		}
	}
}

bool AoC2019_day03::find_nearest_cross_distance(int32_t& dist) {
	bool found = false;
	int32_t d;

	for (uint32_t i = 0; i < crosses_.size(); i++) {
		d = abs(crosses_[i].cross_pt.x) + abs(crosses_[i].cross_pt.y);

		if (found) {
			if (d < dist) {
				dist = d;
			}
		} else {
			found = true;
			dist = d;
		}
	}

	return found;
}

bool AoC2019_day03::find_nearest_cross_steps(int32_t& steps) {
	bool found = false;
	int32_t s, part;

	for (uint32_t i = 0; i < crosses_.size(); i++) {
		s = 0;

		for (uint32_t j = 0; j < 2; j++) {
			part = 0;

			for (uint32_t k = 0; k < crosses_[i].wire_line_idx[j]; k++) {
				part += abs(wire_[j][k].diff.x) + abs(wire_[j][k].diff.y);
			}

			part += abs(crosses_[i].cross_pt.x - wire_[j][crosses_[i].wire_line_idx[j]].from.x) +
					abs(crosses_[i].cross_pt.y - wire_[j][crosses_[i].wire_line_idx[j]].from.y);

			s += part;
		}

		if (found) {
			if (s < steps) {
				steps = s;
			}
		} else {
			found = true;
			steps = s;
		}
	}

	return found;
}

bool AoC2019_day03::are_lines_crossed(const line_str horz, const line_str vert) {
	int32_t hx1, hx2, vy1, vy2;

	hx1 = std::min(horz.from.x, horz.from.x + horz.diff.x);
	hx2 = std::max(horz.from.x, horz.from.x + horz.diff.x);
	vy1 = std::min(vert.from.y, vert.from.y + vert.diff.y);
	vy2 = std::max(vert.from.y, vert.from.y + vert.diff.y);

	if (vert.from.x || horz.from.y) {
		return ((vert.from.x >= hx1) && (vert.from.x <= hx2) && (horz.from.y >= vy1) && (horz.from.y <= vy2));
	} else {
		return false;
	}
}

int32_t AoC2019_day03::get_aoc_day() {
	return 3;
}

int32_t AoC2019_day03::get_aoc_year() {
	return 2019;
}

void AoC2019_day03::tests() {
	int32_t result1 = 0, result2 = 0;
	bool found;

	if (init({"R8,U5,L5,D3", "U7,R6,D4,L4"})) {
		found = find_nearest_cross_distance(result1); // 6, 30
		found = find_nearest_cross_steps(result2);
	}

	if (init({"R75,D30,R83,U83,L12,D49,R71,U7,L72", "U62,R66,U55,R34,D71,R55,D58,R83"})) {
		found = find_nearest_cross_distance(result1); // 159, 610
		found = find_nearest_cross_steps(result2);
	}

	if (init({"R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51", "U98,R91,D20,R16,D67,R40,U7,R15,U6,R7"})) {
		found = find_nearest_cross_distance(result1); // 135, 410
		found = find_nearest_cross_steps(result2);
	}
}

bool AoC2019_day03::part1() {
	int32_t result;

	if (find_nearest_cross_distance(result)) {
		result1_ = std::to_string(result);

		return true;
	}

	return false;
}

bool AoC2019_day03::part2() {
	int32_t result = 0;

	if (find_nearest_cross_steps(result)) {
		result2_ = std::to_string(result);
	}

	return true;
}

int main(void) {
	AoC2019_day03 day03;

	return day03.main_execution();
}
