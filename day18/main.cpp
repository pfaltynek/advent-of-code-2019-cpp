#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include <map>
#include <queue>
#include <set>
#include <string>

const char Entrance = '@';

struct edge {
	char from, to;
	uint64_t steps;
	uint32_t keys_mask;
	uint32_t keys_meet;
};

struct maze_state {
	uint64_t steps;
	std::string path;
};

bool maze_state_compare(const maze_state& m1, const maze_state& m2) {
	if (m1.steps < m2.steps) {
		return true;
	} else if (m1.steps > m2.steps) {
		return false;
	} else {
		return m1.path < m2.path;
	}
}

class AoC2019_day18 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool find_shortest_path_to_key(const coord_str start_pos, edge& e);
	bool collect_all_keys_shortest_way(std::string& path, uint64_t& steps);
	void analyze_maze();
	uint32_t get_key(const char key);
	void add_key(const char key, uint32_t& keys);
	uint32_t pack_chars(const char c1, const char c2, const char c3, const char c4);
	void unpack_chars(const uint32_t packed, char& c1, char& c2, char& c3, char& c4);
	void modify_input_for_part2();

	std::map<coord_str, char> map_;
	std::map<char, coord_str> keys_map_, doors_map_;
	std::vector<coord_str> entrances_;
	uint32_t keys_mask_;
	std::string keys_;
	std::vector<std::map<char, std::map<char, edge>>> edges_;
};

bool AoC2019_day18::init(const std::vector<std::string> lines) {
	uint32_t row_len = 0;
	bool entrance_found = false;

	map_.clear();
	keys_map_.clear();
	doors_map_.clear();
	keys_.clear();
	keys_mask_ = 0;
	entrances_.clear();

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
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				entrances_.push_back({static_cast<int32_t>(j), static_cast<int32_t>(i)});
				entrance_found = true;
			} else if (islower(c)) {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				keys_map_[c] = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
				keys_ += c;
				add_key(c, keys_mask_);
			} else if (isupper(c)) {
				map_[{static_cast<int32_t>(j), static_cast<int32_t>(i)}] = c;
				doors_map_[c] = {static_cast<int32_t>(j), static_cast<int32_t>(i)};
			} else {
				std::cout << "Invalid map character at row " << i + 1 << " column " << j + 1 << "." << std::endl;
				return false;
			}
		}
	}

	std::sort(keys_.begin(), keys_.end());

	analyze_maze();

	return entrance_found;
}

void AoC2019_day18::add_key(const char key, uint32_t& keys) {
	keys |= get_key(key);
}

uint32_t AoC2019_day18::get_key(const char key) {
	int shift = tolower(key) - 'a';

	return (0x00000001 << shift);
}

uint32_t AoC2019_day18::pack_chars(const char c1, const char c2, const char c3, const char c4) {
	uint32_t result = 0;

	result = c4;
	result = result << 8;
	result |= c3;
	result = result << 8;
	result |= c2;
	result = result << 8;
	result |= c1;

	return result;
}

void AoC2019_day18::unpack_chars(const uint32_t packed, char& c1, char& c2, char& c3, char& c4) {
	c1 = packed & 0x000000FF;
	c2 = (packed >> 8) & 0x000000FF;
	c3 = (packed >> 16) & 0x000000FF;
	c4 = (packed >> 24) & 0x000000FF;
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

	if (init({"#######", "#a.#Cd#", "##...##", "##.@.##", "##...##", "#cB#Ab#", "#######"})) {
		modify_input_for_part2();
		if (collect_all_keys_shortest_way(path, steps)) {
			// 8 steps; one is: a, b, c, d
		}
	}

	if (init({"#############", "#DcBa.#.GhKl#", "#.###@#@#I###", "#e#d#####j#k#", "###C#@#@###J#", "#fEbA.#.FgHi#", "#############"})) {
		// modify_input_for_part2();
		if (collect_all_keys_shortest_way(path, steps)) {
			// 32 steps; one is: a, b, c, d, e, f, g, h, i, j, k, l
		}
	}

	if (init({"#############", "#g#f.D#..h#l#", "#F###e#E###.#", "#dCba@#@BcIJ#", "#############", "#nK.L@#@G...#", "#M###N#H###.#", "#o#m..#i#jk.#",
			  "#############"})) {
		// modify_input_for_part2();
		if (collect_all_keys_shortest_way(path, steps)) {
			// 72 steps; one is: e, h, i, a, b, c, d, f, g, k, j, l, n, m, o
		}
	}
}

void AoC2019_day18::analyze_maze() {
	edge e;
	edges_.clear();

	for (uint32_t en = 0; en < entrances_.size(); ++en) {
		edges_.push_back({});
		e.from = '@';
		for (uint32_t i = 0; i < keys_.size(); i++) {
			e.to = keys_[i];
			if (find_shortest_path_to_key(entrances_[en], e)) {
				edges_[en][e.from][e.to] = e;
			}
		}
	}

	for (uint32_t en = 0; en < entrances_.size(); ++en) {
		for (uint32_t i = 0; i < keys_.size(); i++) {
			e.from = keys_[i];
			for (uint32_t j = i + 1; j < keys_.size(); j++) {
				e.to = keys_[j];
				if (find_shortest_path_to_key(keys_map_[e.from], e)) {
					edges_[en][e.from][e.to] = e;
					edges_[en][e.to][e.from] = e;
				}
			}
		}
	}
}

bool AoC2019_day18::collect_all_keys_shortest_way(std::string& path, uint64_t& steps) {
	bool result_found = false;
	maze_state state, next;
	std::map<uint32_t, std::map<uint32_t, maze_state>> q1 = {}, q2 = {};
	std::map<uint32_t, maze_state> m;
	char to;
	char c[4];
	uint32_t key_bit, keys_having, keys_having_new, keys_required, keys_on_the_way, chars_packed;
	edge e;

	q1[pack_chars('@', '@', '@', '@')][0] = {0, ""};

	do {
		for (auto it1 = q1.begin(); it1 != q1.end(); it1++) {
			unpack_chars(it1->first, c[0], c[1], c[2], c[3]);
			m = it1->second;

			for (auto it2 = m.begin(); it2 != m.end(); it2++) {
				keys_having = it2->first;
				state = it2->second;

				if (keys_having == keys_mask_) {
					if (!result_found) {
						result_found = true;
						steps = state.steps;
						path = state.path;
					} else if (steps > state.steps) {
						steps = state.steps;
						path = state.path;
					}
					continue;
				}

				for (uint8_t i = 0; i < entrances_.size(); i++) {
					if (edges_[i].count(c[i])) {
						for (auto it2 = edges_[i][c[i]].begin(); it2 != edges_[i][c[i]].end(); it2++) {
							to = it2->first;
							e = it2->second;

							key_bit = get_key(to);
							if (key_bit & keys_having) {
								continue;
							}

							keys_required = e.keys_mask;
							if ((keys_required & keys_having) != keys_required) {
								continue;
							}
							keys_on_the_way = e.keys_meet;
							if ((keys_having & keys_on_the_way) != keys_on_the_way) {
								continue;
							}
							next = state;
							keys_having_new = keys_having | key_bit;
							next.path += to;
							next.steps += e.steps;

							std::swap(c[i], to);
							chars_packed = pack_chars(c[0], c[1], c[2], c[3]);
							std::swap(c[i], to);

							if (q2.count(chars_packed)) {
								if (q2[chars_packed].count(keys_having_new)) {
									if (q2[chars_packed][keys_having_new].steps > next.steps) {
										q2[chars_packed][keys_having_new] = next;
									}
								} else {
									q2[chars_packed][keys_having_new] = next;
								}
							} else {
								q2[chars_packed][keys_having_new] = next;
							}
						}
					}
				}
			}
		}
		q1.swap(q2);
		q2.clear();
	} while (q1.size());

	return result_found;
}

bool AoC2019_day18::find_shortest_path_to_key(const coord_str start_pos, edge& e) {
	struct location_info {
		coord_str pos;
		uint64_t steps;
		uint32_t keys_mask;
		uint32_t keys_meet;
	};

	std::queue<location_info> q;
	std::set<coord_str> history = {};
	uint32_t keys, meet;
	coord_str pos;
	uint64_t step_cnt;

	q.push({start_pos, 0, 0, 0});

	while (q.size()) {
		pos = q.front().pos;
		step_cnt = q.front().steps;
		keys = q.front().keys_mask;
		meet = q.front().keys_meet;
		q.pop();

		if (history.count(pos)) {
			continue;
		}

		if (!map_.count(pos)) {
			continue;
		}

		if (pos == keys_map_[e.to]) {
			e.keys_mask = keys;
			e.keys_meet = meet;
			e.steps = step_cnt;
			return true;
		}

		if (isupper(map_[pos])) {
			add_key(map_[pos], keys);
		}

		if (islower(map_[pos]) && (map_[pos] != e.to) && (map_[pos] != e.from)) {
			add_key(map_[pos], meet);
		}

		step_cnt++;
		q.push({pos + coord_step_north, step_cnt, keys, meet});
		q.push({pos + coord_step_south, step_cnt, keys, meet});
		q.push({pos + coord_step_west, step_cnt, keys, meet});
		q.push({pos + coord_step_east, step_cnt, keys, meet});
		history.insert(pos);
	}

	return false;
}

void AoC2019_day18::modify_input_for_part2() {
	coord_str entrance = entrances_[0];
	map_.erase(entrance);
	map_.erase(entrance + coord_step_north);
	map_.erase(entrance + coord_step_south);
	map_.erase(entrance + coord_step_east);
	map_.erase(entrance + coord_step_west);
	map_[entrance + coord_step_north + coord_step_west] = '@';
	map_[entrance + coord_step_north + coord_step_east] = '@';
	map_[entrance + coord_step_south + coord_step_west] = '@';
	map_[entrance + coord_step_south + coord_step_east] = '@';

	entrances_.clear();
	entrances_ = {entrance + coord_step_north + coord_step_west, entrance + coord_step_north + coord_step_east, entrance + coord_step_south + coord_step_west,
				  entrance + coord_step_south + coord_step_east};

	for (uint8_t i = 0; i < entrances_.size(); i++) {
		map_[entrances_[i]] = '@';
	}

	analyze_maze();
}

bool AoC2019_day18::part1() {
	std::string path;
	uint64_t result = 0;

	if (collect_all_keys_shortest_way(path, result)) {
		result1_ = std::to_string(result);
		return true;
	}

	return false;
}

bool AoC2019_day18::part2() {
	uint64_t result = 0;
	std::string path;

	modify_input_for_part2();

	if (collect_all_keys_shortest_way(path, result)) {
		result2_ = std::to_string(result);
		return true;
	}

	return false;
}

int main(void) {
	AoC2019_day18 day18;

	return day18.main_execution();
}
