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
	coord_str start_, end_, min_, max_;

	uint64_t find_shortest_path(const bool part2 = false);
	bool is_portal_outer(const coord_str portal) const;
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
	max_ = {INT32_MIN, INT32_MIN};
	min_ = {INT32_MAX, INT32_MAX};

	for (uint64_t i = 0; i < lines.size(); i++) {
		for (size_t j = 0; j < lines[i].size(); j++) {
			switch (lines[i][j]) {
				case '#':
				case ' ':
					break;
				case '.':
					paths_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = false;
					if (static_cast<int32_t>(i) > max_.y) {
						max_.y = i;
					}
					if (static_cast<int32_t>(j) > max_.x) {
						max_.x = j;
					}
					if (static_cast<int32_t>(i) < min_.y) {
						min_.y = i;
					}
					if (static_cast<int32_t>(j) < min_.x) {
						min_.x = j;
					}
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

bool AoC2019_day20::is_portal_outer(const coord_str portal) const {
	return (portal.x > max_.x) || (portal.y > max_.y) || (portal.x < min_.x) || (portal.y < min_.y);
}

uint64_t AoC2019_day20::find_shortest_path(const bool part2) {
	uint32_t steps;
	int32_t level;
	coord_str current, next;
	bool portal_is_outer;
	coord_3d_str hist_item;
	std::queue<coord_4d_str> q;
	std::set<coord_3d_str> history;
	std::vector<coord_str> neighbors = {coord_step_north, coord_step_south, coord_step_west, coord_step_east};

	history.clear();
	q = {};
	q.push({start_.x, start_.y, 0, 0});

	while (q.size()) {
		current = {q.front().x, q.front().y};
		steps = q.front().z;
		level = q.front().w;
		q.pop();

		if (!paths_.count(current)) {
			continue;
		}

		if (current == end_) {
			if (part2) {
				if (level == 0) {
					return steps;
				}
			} else {
				return steps;
			}
		}

		hist_item = {current.x, current.y, level};
		if (history.count(hist_item)) {
			continue;
		}

		history.emplace(hist_item);

		if (paths_[current]) {
			bool portal_active = true;

			if (part2) {

				portal_is_outer = is_portal_outer(current);

				if (level == 0) {
					if (portal_is_outer) {
						portal_active = false;
					}
				}
			}

			if (portal_active) {
				current = portals_[current];
				hist_item.x = current.x;
				hist_item.y = current.y;
				
				if (portal_is_outer) {
					level--;
				} else {
					level++;
				}

				hist_item.z = level;
				history.emplace(hist_item);
			} else {
				continue;
			}
		}

		for (uint8_t i = 0; i < neighbors.size(); i++) {
			next = current + neighbors[i];
			if (paths_.count(next)) {
				q.push({next.x, next.y, static_cast<int32_t>(steps + 1), level});
			}
		}
	}

	return 0;
}

int32_t AoC2019_day20::get_aoc_day() {
	return 20;
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
		result = find_shortest_path();	   // 23
		result = find_shortest_path(true); // 26
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
		result = find_shortest_path(); // 58
		if (init({"             Z L X W       C                 ", "             Z P Q B       K                 ",
				  "  ###########.#.#.#.#######.###############  ", "  #...#.......#.#.......#.#.......#.#.#...#  ",
				  "  ###.#.#.#.#.#.#.#.###.#.#.#######.#.#.###  ", "  #.#...#.#.#...#.#.#...#...#...#.#.......#  ",
				  "  #.###.#######.###.###.#.###.###.#.#######  ", "  #...#.......#.#...#...#.............#...#  ",
				  "  #.#########.#######.#.#######.#######.###  ", "  #...#.#    F       R I       Z    #.#.#.#  ",
				  "  #.###.#    D       E C       H    #.#.#.#  ", "  #.#...#                           #...#.#  ",
				  "  #.###.#                           #.###.#  ", "  #.#....OA                       WB..#.#..ZH",
				  "  #.###.#                           #.#.#.#  ", "CJ......#                           #.....#  ",
				  "  #######                           #######  ", "  #.#....CK                         #......IC",
				  "  #.###.#                           #.###.#  ", "  #.....#                           #...#.#  ",
				  "  ###.###                           #.#.#.#  ", "XF....#.#                         RF..#.#.#  ",
				  "  #####.#                           #######  ", "  #......CJ                       NM..#...#  ",
				  "  ###.#.#                           #.###.#  ", "RE....#.#                           #......RF",
				  "  ###.###        X   X       L      #.#.#.#  ", "  #.....#        F   Q       P      #.#.#.#  ",
				  "  ###.###########.###.#######.#########.###  ", "  #.....#...#.....#.......#...#.....#.#...#  ",
				  "  #####.#.###.#######.#######.###.###.#.#.#  ", "  #.......#.......#.#.#.#.#...#...#...#.#.#  ",
				  "  #####.###.#####.#.#.#.#.###.###.#.###.###  ", "  #.......#.....#.#...#...............#...#  ",
				  "  #############.#.#.###.###################  ", "               A O F   N                     ",
				  "               A A D   M                     "})) {
			result = find_shortest_path(true); // 396
		}
	}
}

bool AoC2019_day20::part1() {

	result1_ = std::to_string(find_shortest_path());

	return true;
}

bool AoC2019_day20::part2() {

	result2_ = std::to_string(find_shortest_path(true));

	return true;
}

int main(void) {
	AoC2019_day20 day20;

	return day20.main_execution();
}
