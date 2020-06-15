#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include <algorithm>
#include <numeric>
#include <regex>

const std::regex moon_pos_regex("^<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>$");

class AoC2019_day12 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	int32_t get_gravity_change(const int32_t dim1, const int32_t dim2);
	int32_t get_total_energy(const uint32_t time_steps);
	void moons_step(std::vector<coord_3d_str>& positions, std::vector<coord_3d_str>& velocities);
	int64_t get_universe_period();

	std::vector<coord_3d_str> moons_;
};

bool AoC2019_day12::init(const std::vector<std::string> lines) {
	std::smatch sm;
	coord_3d_str moon;

	moons_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {

		if (std::regex_match(lines[i], sm, moon_pos_regex)) {
			moons_.push_back({stoi(sm.str(1)), stoi(sm.str(2)), stoi(sm.str(3))});
		} else {
			std::cout << "Invalid input format at line " << i + 1 << std::endl;
			return false;
		}
	}

	if (moons_.size() < 4) {
		std::cout << "Incomplette input  - at least 4 items required" << std::endl;
		return false;
	}
	return true;
}

int32_t AoC2019_day12::get_gravity_change(const int32_t dim1, const int32_t dim2) {
	if (dim1 == dim2) {
		return 0;
	} else if (dim1 < dim2) {
		return 1;
	} else {
		return -1;
	}
}

void AoC2019_day12::moons_step(std::vector<coord_3d_str>& positions, std::vector<coord_3d_str>& velocities) {
	int32_t diff;

	for (uint32_t j = 0; j < positions.size(); j++) {
		for (uint32_t k = j + 1; k < positions.size(); k++) {
			diff = get_gravity_change(positions[j].x, positions[k].x);
			velocities[j].x += diff;
			velocities[k].x -= diff;
			diff = get_gravity_change(positions[j].y, positions[k].y);
			velocities[j].y += diff;
			velocities[k].y -= diff;
			diff = get_gravity_change(positions[j].z, positions[k].z);
			velocities[j].z += diff;
			velocities[k].z -= diff;
		}
	}
	for (uint32_t j = 0; j < positions.size(); j++) {
		positions[j] = positions[j] + velocities[j];
	}
}

int32_t AoC2019_day12::get_total_energy(const uint32_t time_steps) {
	std::vector<coord_3d_str> pos, vel;
	int32_t result = 0;

	pos = moons_;
	vel.clear();

	for (uint32_t i = 0; i < pos.size(); i++) {
		vel.push_back({});
	}

	for (uint32_t i = 0; i < time_steps; i++) {
		moons_step(pos, vel);
	}

	for (uint32_t i = 0; i < pos.size(); i++) {
		result += pos[i].size() * vel[i].size();
	}

	return result;
}

int64_t AoC2019_day12::get_universe_period() {
	std::vector<coord_3d_str> pos, vel;
	int64_t result = 0, idx = 0;
	bool found = false;
	coord_3d_str axis;

	pos = moons_;
	axis = {};

	for (uint32_t i = 0; i < pos.size(); i++) {
		vel.push_back({});
	}

	while (!found) {
		moons_step(pos, vel);
		idx++;
		found = true;

		if (!axis.x) {
			found = true;
			for (uint32_t i = 0; i < pos.size(); i++) {
				if ((pos[i].x != moons_[i].x) || (vel[i].x)) {
					found = false;
					break;
				}
			}
			if (found) {
				axis.x = idx;
			}
		}

		if (!axis.y) {
			found = true;
			for (uint32_t i = 0; i < pos.size(); i++) {
				if ((pos[i].y != moons_[i].y) || (vel[i].y)) {
					found = false;
					break;
				}
			}
			if (found) {
				axis.y = idx;
			}
		}

		if (!axis.z) {
			found = true;
			for (uint32_t i = 0; i < pos.size(); i++) {
				if ((pos[i].z != moons_[i].z) || (vel[i].z)) {
					found = false;
					break;
				}
			}
			if (found) {
				axis.z = idx;
			}
		}

		found = axis.x && axis.y && axis.z;
	}

	idx = std::lcm((int64_t)axis.x, (int64_t)axis.y);
	result = std::lcm(axis.z, idx);

	return result;
}

int32_t AoC2019_day12::get_aoc_day() {
	return 12;
}

int32_t AoC2019_day12::get_aoc_year() {
	return 2019;
}

void AoC2019_day12::tests() {
	int32_t result;
	int64_t result64;

	if (init({"<x=-1, y=0, z=2>", "<x=2, y=-10, z=-7>", "<x=4, y=-8, z=8>", "<x=3, y=5, z=-1>"})) {
		result = get_total_energy(10);	// 179
		result64 = get_universe_period(); // 2772
	}

	if (init({"<x=-8, y=-10, z=0>", "<x=5, y=5, z=10>", "<x=2, y=-7, z=3>", "<x=9, y=-8, z=-3>"})) {
		result = get_total_energy(100);   // 1940
		result64 = get_universe_period(); // 4 686 774 924
	}
}

bool AoC2019_day12::part1() {
	int32_t result = 0;

	result = get_total_energy(1000);

	result1_ = std::to_string(result);

	return true;
}

bool AoC2019_day12::part2() {
	int64_t result = 0;

	result = get_universe_period();

	result2_ = std::to_string(result);

	return true;
}

int main(void) {
	AoC2019_day12 day12;

	return day12.main_execution();
}
