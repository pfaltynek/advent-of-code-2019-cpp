#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include "intcode.hpp"
#include <map>

class AoC2019_day23 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	const size_t C_NETWORK_SIZE = 50;
	std::vector<int64_t> ints_;
	int64_t run_network();
};

bool AoC2019_day23::init(const std::vector<std::string> lines) {
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

	return true;
}

void AoC2019_day23::tests() {

	if (init({"109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99"})) {
		int_code ic(0, ints_);
		while (!ic.simulate_intcode()) {
			// copies itself into output
		}
	}

	if (init({"1102,34915192,34915192,7,4,7,99,0"})) {
		int_code ic(0, ints_);
		while (!ic.simulate_intcode()) {
			// outputs 16-digit number 1 219 070 632 396 864
		}
	}

	if (init({"104,1125899906842624,99"})) {
		int_code ic(0, ints_);
		while (!ic.simulate_intcode()) {
			// outputs number 1125899906842624
		}
	}
}

int64_t AoC2019_day23::run_network() {
	std::map<int64_t, int_code> network;
	bool finished = false;
	int64_t result = 0, address, x, y;

	for (size_t i = 0; i < C_NETWORK_SIZE; i++) {
		int_code ic(i, ints_);
		network[i] = ic;
	}

	while (!finished) {
		for (size_t i = 0; i < C_NETWORK_SIZE; i++) {
			network[i].simulate_intcode();
			if (network[i].get_outgoing_packet(address, x, y)) {
				if (address == 255) {
					result = y;
					finished = true;
					break;
				} else if ((address >= 0) && (static_cast<size_t>(address) < C_NETWORK_SIZE)) {
					network[address].set_incoming_packet(x, y);
				} else {
					int z = 13;
				}
			}
		}
	}

	return result;
}

bool AoC2019_day23::part1() {
	result1_ = std::to_string(run_network());

	return true;
}

bool AoC2019_day23::part2() {
	result2_ = std::to_string(0);

	return true;
}

int32_t AoC2019_day23::get_aoc_day() {
	return 23;
}

int32_t AoC2019_day23::get_aoc_year() {
	return 2019;
}

int main(void) {
	AoC2019_day23 day23;

	return day23.main_execution();
}
