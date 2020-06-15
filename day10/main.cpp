#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include <algorithm>

class AoC2019_day10 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool is_coord_in_area(const coord_str coord);
	bool is_coord_in_area(const int32_t x, const int32_t y);
	std::vector<std::vector<coord_str>> get_asteroid_rays(const coord_str center);
	std::vector<std::vector<coord_str>> get_asteroid_rays_sorted_by_angle(const coord_str center);
	int32_t detect_asteroids(const uint32_t x, const uint32_t y);
	int32_t find_maximum_asteroids(coord_str& pos);
	int32_t get_vaporized_coord(const uint32_t order, const coord_str center);

	std::vector<std::string> map_;
	uint32_t w_, h_;
	coord_str pos_;
};

bool AoC2019_day10::init(const std::vector<std::string> lines) {

	map_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		if (i) {
			if (lines[i].size() != w_) {
				std::cout << "Invalid size of line " << i + 1 << std::endl;
				return false;
			}
		} else {
			w_ = lines[i].size();
			h_ = lines.size();
		}

		size_t pos = lines[i].find_first_not_of(".#");
		if (pos != std::string::npos) {
			std::cout << "Invalid input format at line " << i + 1 << " position " << pos << std::endl;
			return false;
		}

		map_.push_back(lines[i]);
	}

	return true;
}

bool AoC2019_day10::is_coord_in_area(const coord_str coord) {
	if ((coord.x < 0) || (coord.y < 0) || (coord.x >= (int32_t)w_) || (coord.y >= (int32_t)h_)) {
		return false;
	} else {
		return true;
	}
}

bool AoC2019_day10::is_coord_in_area(const int32_t x, const int32_t y) {
	return is_coord_in_area({x, y});
}

int32_t AoC2019_day10::find_maximum_asteroids(coord_str& pos) {
	int32_t result = 0, tmp;

	pos = {-1, -1};

	for (uint32_t y = 0; y < h_; y++) {
		for (uint32_t x = 0; x < w_; x++) {
			tmp = detect_asteroids(x, y);
			if (tmp > result) {
				result = tmp;
				pos = {(int32_t)x, (int32_t)y};
			}
		}
	}

	return result;
}

inline static int get_quadrant(coord_str coord) {
	if ((coord.x >= 0) && (coord.y < 0)) {
		return 1;
	} else if ((coord.x > 0) && (coord.y >= 0)) {
		return 2;
	} else if ((coord.x <= 0) && (coord.y > 0)) {
		return 3;
	} else if ((coord.x < 0) && (coord.y <= 0)) {
		return 4;
	}

	return -1;
}

inline static bool compare_by_angle(std::vector<coord_str> first, std::vector<coord_str> second) {
	if (first.empty()) {
		return false;
	}
	if (second.empty()) {
		return true;
	}

	coord_str c1, c2;
	int32_t q1, q2;
	c1 = first[0];
	c2 = second[0];
	q1 = get_quadrant(c1);
	q2 = get_quadrant(c2);

	if (q1 < q2) {
		return true;
	} else if (q1 > q2) {
		return false;
	}

	switch (q1) {
		case 1:
			c1.y *= -1;
			c2.y *= -1;
			break;
		case 2:
			std::swap(c1.x, c1.y);
			std::swap(c2.x, c2.y);
			break;
		case 3:
			c1.x *= -1;
			c2.x *= -1;
			break;
		case 4:
			c1.x *= -1;
			c2.x *= -1;
			std::swap(c1.x, c1.y);
			std::swap(c2.x, c2.y);
			c1.x *= -1;
			c2.x *= -1;
			break;
	}

	q1 = c1.x * 1000 / c1.y;
	q2 = c2.x * 1000 / c2.y;

	return q1 < q2;
}

std::vector<std::vector<coord_str>> AoC2019_day10::get_asteroid_rays(const coord_str center) {
	bool hist[h_][w_];
	int32_t max, idx, tx, ty, diff_x, diff_y, dx, dy;
	coord_str tl, br;
	std::vector<std::vector<coord_str>> result;
	std::vector<coord_str> points, ray;

	for (uint32_t i = 0; i < w_; i++) {
		for (uint32_t j = 0; j < h_; j++) {
			hist[j][i] = false;
		}
	}

	hist[center.y][center.x] = true;
	max = std::max(std::max((int32_t)w_ - center.x, center.x), std::max((int32_t)h_ - center.y, center.y));
	result.clear();
	points.clear();

	for (int32_t d = 1; d <= max; d++) {
		br = tl = center;
		tl.x -= d;
		tl.y -= d;
		br.x += d;
		br.y += d;

		for (int32_t p = tl.x; p <= br.x; p++) {
			points.push_back({p, tl.y});
			points.push_back({p, br.y});
		}
		for (int32_t p = tl.y + 1; p < br.y; p++) {
			points.push_back({tl.x, p});
			points.push_back({br.x, p});
		}

		idx = 0;
		while (idx < (int32_t)points.size()) {
			if (!is_coord_in_area(points[idx]) || (hist[points[idx].y][points[idx].x])) {
				points.erase(points.begin() + idx);
			} else {
				idx++;
			}
		}

		for (uint32_t p = 0; p < points.size(); p++) {
			diff_x = points[p].x - center.x;
			diff_y = points[p].y - center.y;

			ray.clear();
			for (int32_t m = 1; m <= max; m++) {
				dx = m * diff_x;
				dy = m * diff_y;
				tx = center.x + dx;
				ty = center.y + dy;
				if (is_coord_in_area(tx, ty)) {
					if (map_[ty][tx] == '#') {
						ray.push_back({dx, dy});
					}
					hist[ty][tx] = true;
				} else {
					break;
				}
			}

			if (ray.size()) {
				result.push_back(ray);
			}
		}
	}

	return result;
}

std::vector<std::vector<coord_str>> AoC2019_day10::get_asteroid_rays_sorted_by_angle(const coord_str center) {
	std::vector<std::vector<coord_str>> result;

	result = get_asteroid_rays(center);

	std::sort(result.begin(), result.end(), compare_by_angle);

	return result;
}

int32_t AoC2019_day10::detect_asteroids(const uint32_t x, const uint32_t y) {

	if (map_[y][x] != '#') {
		return 0;
	}
	return get_asteroid_rays({(int32_t)x, (int32_t)y}).size();
}

int32_t AoC2019_day10::get_vaporized_coord(const uint32_t order, const coord_str center) {
	int32_t result = 0, idx = 0;
	std::vector<std::vector<coord_str>> rays = get_asteroid_rays_sorted_by_angle(center);
	std::vector<coord_str> vaporized = {};
	coord_str pt;

	while (!rays.empty()) {
		idx = idx % rays.size();
		pt = rays[idx][0];
		pt.x += center.x;
		pt.y += center.y;
		vaporized.push_back(pt);
		rays[idx].erase(rays[idx].begin());
		if (rays[idx].empty()) {
			rays.erase(rays.begin() + idx);
		} else {
			idx++;
		}
	}
	if (order <= vaporized.size()) {
		result = (vaporized[order - 1].x * 100) + vaporized[order - 1].y;
	}

	return result;
}

int32_t AoC2019_day10::get_aoc_day() {
	return 10;
}

int32_t AoC2019_day10::get_aoc_year() {
	return 2019;
}

void AoC2019_day10::tests() {
	int32_t result;
	coord_str pos;
	std::vector<std::vector<coord_str>> pts;

	pts = {{{0, -3}}, {{1, -3}}, {{3, -1}}, {{3, 0}}, {{3, 1}}, {{1, 3}}, {{0, 3}}, {{-1, 3}}, {{-3, 1}}, {{-3, 0}}, {{-3, -1}}, {{-1, -3}}};
	std::reverse(pts.begin(), pts.end());
	std::sort(pts.begin(), pts.end(), compare_by_angle);

	if (init({".#..#", ".....", "#####", "....#", "...##"})) {
		result = find_maximum_asteroids(pos); // 8 @ [3,4]
	}

	if (init({"......#.#.", "#..#.#....", "..#######.", ".#.#.###..", ".#..#.....", "..#....#.#", "#..#....#.", ".##.#..###", "##...#..#.", ".#....####"})) {
		result = find_maximum_asteroids(pos); // 33 @ [5,8]
	}

	if (init({"#.#...#.#.", ".###....#.", ".#....#...", "##.#.#.#.#", "....#.#.#.", ".##..###.#", "..#...##..", "..##....##", "......#...", ".####.###."})) {
		result = find_maximum_asteroids(pos); // 35 @ [1,2]
	}

	if (init({".#..#..###", "####.###.#", "....###.#.", "..###.##.#", "##.##.#.#.", "....###..#", "..#.#..#.#", "#..#.#.###", ".##...##.#", ".....#.#.."})) {
		result = find_maximum_asteroids(pos); // 41 @ [6,3]
	}

	if (init({".#....#####...#..", "##...##.#####..##", "##...#...#.#####.", "..#.....#...###..", "..#.#.....#....##"})) {
		result = get_vaporized_coord(200, {8, 3});
	}

	if (init({".#..##.###...#######", "##.############..##.", ".#.######.########.#", ".###.#######.####.#.", "#####.##.#.##.###.##",
			  "..#####..#.#########", "####################", "#.####....###.#.#.##", "##.#################", "#####.##.###..####..",
			  "..######..##.#######", "####.##.####...##..#", ".#####..#.######.###", "##...#.##########...", "#.##########.#######",
			  ".####.#.###.###.#.##", "....##.##.###..#####", ".#.#.###########.###", "#.#.#.#####.####.###", "###.##.####.##.#..##"})) {
		result = find_maximum_asteroids(pos); // 210 @ [11,13]

		result = get_vaporized_coord(200, pos); // 802
	}
}

bool AoC2019_day10::part1() {
	int32_t result = 0;

	result = find_maximum_asteroids(pos_);

	result1_ = std::to_string(result);
	return true;
}

bool AoC2019_day10::part2() {
	int32_t result = 0;

	result = get_vaporized_coord(200, pos_);

	result2_ = std::to_string(result);

	return true;
}

int main(void) {
	AoC2019_day10 day10;

	return day10.main_execution();
}
