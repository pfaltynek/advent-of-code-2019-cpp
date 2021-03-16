#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include <map>
#include <queue>
#include <set>

const std::string C_START_PORTAL_NAME = "AA";
const std::string C_END_PORTAL_NAME = "ZZ";

class AoC2019_day20 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	std::map<coord_str, bool> paths_;
	std::map<coord_str, coord_str> portals_;
	coord_str start_, end_;

	uint64_t find_shortest_path();
};

bool AoC2019_day20::init(const std::vector<std::string> lines) {
	std::vector<std::pair<char, coord_str>> chars;
	std::map<std::string, std::vector<coord_str>> portals;
	size_t idx, idx2remove;
	coord_str c1, c2n, c2s, c2w, c2e, c2, pt;
	bool portal_found, start_found, end_found;
	std::string portal_label;

	chars.clear();
	paths_.clear();
	portals_.clear();

	for (uint64_t i = 0; i < lines.size(); i++) {
		for (size_t j = 0; j < lines[i].size(); j++) {
			switch (lines[i][j]) {
				case '#':
				case ' ':
					break;
				case '.':
					paths_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = false;
					break;
				default:
					if (isupper(lines[i][j])) {
						chars.push_back({lines[i][j], {static_cast<int32_t>(j), static_cast<int32_t>(i)}});
					} else {
						std::cout << "Invalid input symbol at position " << j + 1 << " of line " << i + 1 << std::endl;
						return false;
					}
					break;
			}
		}
	}

	idx = 0;

	while (idx < chars.size()) {

		portal_found = false;
		portal_label = "  ";

		for (size_t i = idx + 1; i < chars.size(); i++) {

			c1 = chars[idx].second;
			c2 = chars[i].second;
			c2e = c2 + coord_step_east;
			c2w = c2 + coord_step_west;
			c2n = c2 + coord_step_north;
			c2s = c2 + coord_step_south;

			if (c1 == c2e) {
				if (paths_.count(c1 + coord_step_east)) {
					c2 = c1 + coord_step_east;
					portal_label[0] = chars[i].first;
					portal_label[1] = chars[idx].first;
					portal_found = true;
				} else if (paths_.count(c2w)) {
					c1 = c2;
					c2 = c2w;
					portal_label[0] = chars[i].first;
					portal_label[1] = chars[idx].first;
					portal_found = true;
				} else {
					std::cout << "Unable to decode portal at [" << c1.x << ", " << c1.y << "] '" << chars[idx].first << "'" << std::endl;
					return false;
				}
			} else if (c1 == c2w) {
				if (paths_.count(c1 + coord_step_west)) {
					c2 = c1 + coord_step_west;
					portal_label[0] = chars[idx].first;
					portal_label[1] = chars[i].first;
					portal_found = true;
				} else if (paths_.count(c2e)) {
					c1 = c2;
					c2 = c2e;
					portal_label[0] = chars[idx].first;
					portal_label[1] = chars[i].first;
					portal_found = true;
				} else {
					std::cout << "Unable to decode portal at [" << c1.x << ", " << c1.y << "] '" << chars[idx].first << "'" << std::endl;
					return false;
				}
			} else if (c1 == c2n) {
				if (paths_.count(c1 + coord_step_north)) {
					c2 = c1 + coord_step_north;
					portal_label[0] = chars[idx].first;
					portal_label[1] = chars[i].first;
					portal_found = true;
				} else if (paths_.count(c2s)) {
					c1 = c2;
					c2 = c2s;
					portal_label[0] = chars[idx].first;
					portal_label[1] = chars[i].first;
					portal_found = true;
				} else {
					std::cout << "Unable to decode portal at [" << c1.x << ", " << c1.y << "] '" << chars[idx].first << "'" << std::endl;
					return false;
				}
			} else if (c1 == c2s) {
				if (paths_.count(c1 + coord_step_south)) {
					c2 = c1 + coord_step_south;
					portal_label[0] = chars[i].first;
					portal_label[1] = chars[idx].first;
					portal_found = true;
				} else if (paths_.count(c2n)) {
					c1 = c2;
					c2 = c2n;
					portal_label[0] = chars[i].first;
					portal_label[1] = chars[idx].first;
					portal_found = true;
				} else {
					std::cout << "Unable to decode portal at [" << c1.x << ", " << c1.y << "] '" << chars[idx].first << "'" << std::endl;
					return false;
				}
			} else {
				continue;
			}

			if (portal_found) {
				idx2remove = i;
				break;
			}
		}

		if (portal_found) {
			chars.erase(chars.begin() + idx2remove);
			portals[portal_label].push_back(c1);
			portals[portal_label].push_back(c2);
			idx++;
		} else {
			std::cout << "Unable to complete portal at [" << c1.x << ", " << c1.y << "] '" << chars[idx].first << "'" << std::endl;
			return false;
		}
	}

	start_found = end_found = false;

	if (portals.count(C_START_PORTAL_NAME)) {
		if (portals[C_START_PORTAL_NAME].size() == 2) {
			start_ = portals[C_START_PORTAL_NAME][1];
			start_found = true;
			portals.erase(C_START_PORTAL_NAME);
		}
	}

	if (!start_found) {
		std::cout << "Start portal not found" << std::endl;
		return false;
	}

	if (portals.count(C_END_PORTAL_NAME)) {
		if (portals[C_END_PORTAL_NAME].size() == 2) {
			end_ = portals[C_END_PORTAL_NAME][1];
			end_found = true;
			portals.erase(C_END_PORTAL_NAME);
		}
	}

	if (!end_found) {
		std::cout << "End portal not found" << std::endl;
		return false;
	}

	for (auto it = portals.begin(); it != portals.end(); it++) {
		portals_[it->second[0]] = it->second[3];
		paths_[it->second[0]] = true;
		portals_[it->second[2]] = it->second[1];
		paths_[it->second[2]] = true;
	}

	return true;
}

uint64_t AoC2019_day20::find_shortest_path() {
	uint64_t steps;
	coord_str current, next;
	std::queue<std::pair<coord_str, uint64_t>> q;
	std::set<coord_str> history;
	std::vector<coord_str> neighbors = {coord_step_north, coord_step_south, coord_step_west, coord_step_east};

	history.clear();
	q = {};
	q.push({start_, 0});

	while (q.size()) {
		auto pos = q.front();
		q.pop();
		current = pos.first;
		steps = pos.second;

		if (!paths_.count(current)) {
			continue;
		}

		if (current == end_) {
			return steps;
		}

		if (history.count(current)) {
			continue;
		}

		history.emplace(current);

		if (paths_[current]) {
			current = portals_[current];
			history.emplace(current);
		}

		for (uint8_t i = 0; i < neighbors.size(); i++) {
			next = current + neighbors[i];
			if (paths_.count(next)) {
				q.push({next, steps + 1});
			}
		}
	}

	return 0;
}

int32_t AoC2019_day20::get_aoc_day() {
	return 17;
}

int32_t AoC2019_day20::get_aoc_year() {
	return 2019;
}

void AoC2019_day20::tests() {
	uint64_t result = 0;

	if (init({"         A           ", "         A           ", "  #######.#########  ", "  #######.........#  ", "  #######.#######.#  ",
			  "  #######.#######.#  ", "  #######.#######.#  ", "  #####  B    ###.#  ", "BC...##  C    ###.#  ", "  ##.##       ###.#  ",
			  "  ##...DE  F  ###.#  ", "  #####    G  ###.#  ", "  #########.#####.#  ", "DE..#######...###.#  ", "  #.#########.###.#  ",
			  "FG..#########.....#  ", "  ###########.#####  ", "             Z       ", "             Z       "})) {
		result = find_shortest_path();
	}

	if (init({"                   A               ", "                   A               ", "  #################.#############  ",
			  "  #.#...#...................#.#.#  ", "  #.#.#.###.###.###.#########.#.#  ", "  #.#.#.......#...#.....#.#.#...#  ",
			  "  #.#########.###.#####.#.#.###.#  ", "  #.............#.#.....#.......#  ", "  ###.###########.###.#####.#.#.#  ",
			  "  #.....#        A   C    #.#.#.#  ", "  #######        S   P    #####.#  ", "  #.#...#                 #......VT",
			  "  #.#.#.#                 #.#####  ", "  #...#.#               YN....#.#  ", "  #.###.#                 #####.#  ",
			  "DI....#.#                 #.....#  ", "  #####.#                 #.###.#  ", "ZZ......#               QG....#..AS",
			  "  ###.###                 #######  ", "JO..#.#.#                 #.....#  ", "  #.#.#.#                 ###.#.#  ",
			  "  #...#..DI             BU....#..LF", "  #####.#                 #.#####  ", "YN......#               VT..#....QG",
			  "  #.###.#                 #.###.#  ", "  #.#...#                 #.....#  ", "  ###.###    J L     J    #.#.###  ",
			  "  #.....#    O F     P    #.#...#  ", "  #.###.#####.#.#####.#####.###.#  ", "  #...#.#.#...#.....#.....#.#...#  ",
			  "  #.#####.###.###.#.#.#########.#  ", "  #...#.#.....#...#.#.#.#.....#.#  ", "  #.###.#####.###.###.#.#.#######  ",
			  "  #.#.........#...#.............#  ", "  #########.###.###.#############  ", "           B   J   C               ",
			  "           U   P   P               "})) {
		result = find_shortest_path();
	}
}

bool AoC2019_day20::part1() {

	result1_ = std::to_string(find_shortest_path());

	return true;
}

bool AoC2019_day20::part2() {

	result2_ = std::to_string(0);

	return true;
}

int main(void) {
	AoC2019_day20 day20;

	return day20.main_execution();
}
