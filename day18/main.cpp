#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include <map>
#include <queue>
#include <set>
#include <string>

const char Entrance = '@';

struct maze_state {
	coord_str pos;
	uint64_t steps;
	std::string keys_collected;
};

class AoC2019_day18 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool find_shortest_path_to_key(const coord_str from, char key, const std::string keys, uint64_t& doors);
	bool collect_all_keys_shortest_way(std::string& path, uint64_t& steps);

	std::map<coord_str, char> map_;
	std::map<char, coord_str> keys_, doors_;
	coord_str entrance_;
};

bool AoC2019_day18::init(const std::vector<std::string> lines) {
	uint32_t row_len = 0;
	bool entrance_found = false;

	map_.clear();
	keys_.clear();
	doors_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		if (!i) {
			row_len = lines[i].size();
		} else {
			if (row_len != lines[i].size()) {
				return false;
			}
		}
		for (uint32_t j = 0; j < lines[i].size(); j++) {
			char c = lines[i][j];
			if (c == '#') {
				continue;
			} else if (c == '.') {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
			} else if (c == '@') {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = '.';
				entrance_ = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
				entrance_found = true;
			} else if (islower(c)) {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				keys_[c] = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
			} else if (isupper(c)) {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				doors_[c] = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
			} else {
				std::cout << "Invalid map character at row " << i + 1 << " column " << j + 1 << "." << std::endl;
				return false;
			}
		}
	}

	return entrance_found;
}

int32_t AoC2019_day18::get_aoc_day() {
	return 18;
}

int32_t AoC2019_day18::get_aoc_year() {
	return 2019;
}

void AoC2019_day18::tests() {
	std::string path;
	uint64_t steps;

	if (init({"#########", "#b.A.@.a#", "#########"})) {
		if (collect_all_keys_shortest_way(path, steps)) {
			// 8 steps: a,b
		}
	}

	if (init({"########################", "#f.D.E.e.C.b.A.@.a.B.c.#", "######################.#", "#d.....................#", "########################"})) {
		if (collect_all_keys_shortest_way(path, steps)) {
			// 86 steps: a,b,c,d,e,f
		}
	}

	if (init({"########################", "#...............b.C.D.f#", "#.######################", "#.....@.a.B.c.d.A.e.F.g#", "########################"})) {
		if (collect_all_keys_shortest_way(path, steps)) {
			// 132 steps: b, a, c, d, f, e, g
		}
	}

	if (init({"#################", "#i.G..c...e..H.p#", "########.########", "#j.A..b...f..D.o#", "########@########", "#k.E..a...g..B.n#", "########.########",
			  "#l.F..d...h..C.m#", "#################"})) {
		if (collect_all_keys_shortest_way(path, steps)) {
			// 136 steps; one is: a, f, b, j, g, n, h, d, l, o, e, p, c, i, k, m
		}
	}

	if (init({"########################", "#@..............ac.GI.b#", "###d#e#f################", "###A#B#C################", "###g#h#i################",
			  "########################"})) {
		if (collect_all_keys_shortest_way(path, steps)) {
			// 81 steps; one is: a, c, f, i, d, g, b, e, h
		}
	}
}

bool AoC2019_day18::collect_all_keys_shortest_way(std::string& path, uint64_t& steps) {
	std::string keys2collect = "";
	bool result_found = false;
	maze_state state, next;
	std::queue<maze_state> q1 = {}, q2 = {};
	uint64_t steps_part;

	q1.push({entrance_, 0, {}});

	keys2collect.clear();

	for (auto it = keys_.begin(); it != keys_.end(); it++) {
		keys2collect += it->first;
	}

	std::sort(keys2collect.begin(), keys2collect.end());

	do {
		while (q1.size()) {
			state = q1.front();
			q1.pop();

			if (state.keys_collected.size() == keys2collect.size()) {
				std::string tmp = state.keys_collected;
				std::sort(tmp.begin(), tmp.end());
				if (tmp == keys2collect) {
					if (!result_found) {
						result_found = true;
						steps = state.steps;
						path = state.keys_collected;
					} else if (steps > state.steps) {
						steps = state.steps;
						path = state.keys_collected;
					}
					continue;
				}
			}

			for (uint32_t i = 0; i < keys2collect.size(); i++) {
				if (state.keys_collected.find(keys2collect[i]) == std::string::npos) {
					if (find_shortest_path_to_key(state.pos, keys2collect[i], state.keys_collected, steps_part)) {
						next = state;
						next.pos = keys_[keys2collect[i]];
						next.keys_collected += keys2collect[i];
						next.steps += steps_part;
						q2.emplace(next);
					}
				}
			}
		}
		q1.swap(q2);
	} while (q1.size());

	return result_found;
}

bool AoC2019_day18::find_shortest_path_to_key(const coord_str from, char key, const std::string keys, uint64_t& steps) {
	std::queue<std::pair<coord_str, int64_t>> q;
	std::set<coord_str> history = {};
	coord_str pos, next;
	int64_t step_cnt;

	q.push({from, 0});

	while (q.size()) {
		pos = q.front().first;
		step_cnt = q.front().second;
		q.pop();

		if (history.count(pos)) {
			continue;
		}

		if (!map_.count(pos)) {
			continue;
		}

		if (pos == keys_[key]) {
			steps = step_cnt;
			return true;
		}

		if (isupper(map_[pos])) {
			if (keys.find(tolower(map_[pos])) == std::string::npos) {
				continue;
			}
		}

		step_cnt++;
		q.push(std::make_pair(pos + coord_step_north, step_cnt));
		q.push(std::make_pair(pos + coord_step_south, step_cnt));
		q.push(std::make_pair(pos + coord_step_west, step_cnt));
		q.push(std::make_pair(pos + coord_step_east, step_cnt));
		history.insert(pos);
	}

	return false;
}

bool AoC2019_day18::part1() {
	int64_t result = 0;

	if (1) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day18::part2() {
	int64_t result = 0;
	if (1) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day18 day18;

	return day18.main_execution();
}
