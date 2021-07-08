#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include "intcode.hpp"
#include <map>

const std::vector<std::string> C_COMMANDS = {"south",
											 "take hologram",
											 "north",
											 "west",
											 "take mutex",
											 "south",
											 "south",
											 "east",
											 "take weather machine",
											 "west",
											 "south",
											 "take polygon",
											 "north",
											 "north",
											 "north",
											 "east",
											 "north",
											 "north",
											 "north",
											 "take semiconductor",
											 "east",
											 "take prime number",
											 "west",
											 "west",
											 "take monolith",
											 "east",
											 "south",
											 "west",
											 "north",
											 "take jam",
											 "west"};
const std::string C_INVERTORY_COMMAND = "inv";
const std::string C_SENSOR_DIRECTION = "north";
const std::string C_DROP_COMMAND = "drop ";
const std::string C_TAKE_COMMAND = "take ";
const std::string C_HEAVIER = "heavier";
const std::string C_LIGHTER = "lighter";
const std::string C_CODE_START = "You should be able to get in by typing ";


class AoC2019_day25 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	std::vector<int64_t> ints_;
	int64_t control_droid();
	std::queue<std::string> path_;
	void get_items_to_drop_combinations(const std::vector<std::string> items, std::vector<std::vector<std::string>>& combs);
	void debug_print(std::string& msg);
};

bool AoC2019_day25::init(const std::vector<std::string> lines) {
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

void AoC2019_day25::tests() {
}

void AoC2019_day25::get_items_to_drop_combinations(const std::vector<std::string> items, std::vector<std::vector<std::string>>& combs) {
	combs.clear();

	// no item to remove
	combs.push_back({});

	// remove one item
	for (size_t i = 0; i < items.size(); i++) {
		combs.push_back({items[i]});
	}

	// remove two items
	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = i + 1; j < items.size(); j++) {
			combs.push_back({items[i], items[j]});
		}
	}

	// remove three items
	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = i + 1; j < items.size(); j++) {
			for (size_t k = j + 1; k < items.size(); k++) {
				combs.push_back({items[i], items[j], items[k]});
			}
		}
	}

	// remove four items
	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = i + 1; j < items.size(); j++) {
			for (size_t k = j + 1; k < items.size(); k++) {
				for (size_t l = k + 1; l < items.size(); l++) {
					combs.push_back({items[i], items[j], items[k], items[l]});
				}
			}
		}
	}
}

void AoC2019_day25::debug_print(std::string& msg) {
#if 0
	std::cout << msg;
#endif
}

int64_t AoC2019_day25::control_droid() {
	bool explored = false, ask_inv = false, get_inv = false, weight = false, finished;
	int64_t result = 0;
	uint16_t idx = 0, idx1, idx2;
	int_code droid(ints_);
	std::string msg, cache;
	std::vector<std::string> invertory;
	uint16_t step;
	std::vector<std::vector<std::string>> items2remove;

	while (!finished) {
		finished = droid.simulate_intcode();

		if (!explored) {
			if (droid.get_message(msg)) {
				debug_print(msg);
			}
			if (droid.get_waiting_input()) {
				if (idx < C_COMMANDS.size()) {
					droid.set_command(C_COMMANDS[idx]);
					idx++;
				} else {
					explored = true;
				}
			}
		} else if (!ask_inv) {
			if (droid.get_waiting_input()) {
				droid.set_command(C_INVERTORY_COMMAND);
				invertory.clear();
				cache.clear();
				ask_inv = true;
			}
		} else if (!get_inv) {
			if (droid.get_message(msg)) {
				if (msg == "\n") {
					if (cache.size() > 2) {
						if ((cache[0] == '-') && (cache[1] == ' ')) {
							invertory.push_back(cache.substr(2, cache.size()));
						}
					}
					cache.clear();
				} else {
					cache.append(msg);
				}
				debug_print(msg);
			}
			if (droid.get_waiting_input()) {
				get_inv = true;
				get_items_to_drop_combinations(invertory, items2remove);
				idx1 = 0;
				idx2 = 0;
				cache.clear();
				step = 0;
			}
		} else if (!weight) {
			switch (step) {
				case 0:
					if (idx1 >= items2remove.size()) {
						weight = true;
					} else {
						if (idx2 < items2remove[idx1].size()) {
							step = 1;
						} else {
							step = 3;
						}
					}
					break;
				case 1:
					if (droid.get_waiting_input()) {
						msg = C_DROP_COMMAND + items2remove[idx1][idx2];
						droid.set_command(msg);
						idx2++;
						step = 2;
					}
					break;
				case 2:
					if (droid.get_message(msg)) {
						debug_print(msg);
					}
					if (droid.get_waiting_input()) {
						if (idx2 < items2remove[idx1].size()) {
							step = 1;
						} else {
							step = 3;
						}
					}
					break;
				case 3:
					if (droid.get_waiting_input()) {
						droid.set_command(C_SENSOR_DIRECTION);
						step = 4;
					}
					break;
				case 4:
					if (droid.get_message(msg)) {
						debug_print(msg);
						cache.append(msg);
					}
					if (droid.get_waiting_input()) {
						if ((cache.find(C_LIGHTER) != std::string::npos) || (cache.find(C_HEAVIER) != std::string::npos)) {
							idx2 = 0;
							if (idx2 < items2remove[idx1].size()) {
								step = 5;
							} else {
								step = 7;
							}
						}
					}
					break;
				case 5:
					if (droid.get_waiting_input()) {
						msg = C_TAKE_COMMAND + items2remove[idx1][idx2];
						droid.set_command(msg);
						idx2++;
						step = 6;
					}
					break;
				case 6:
					if (droid.get_message(msg)) {
						debug_print(msg);
					}
					if (droid.get_waiting_input()) {
						if (idx2 < items2remove[idx1].size()) {
							step = 5;
						} else {
							step = 7;
						}
					}
					break;
				case 7:
					idx1++;
					idx2 = 0;
					step = 0;
					break;
			}
		} 
	}

	size_t pos = cache.find(C_CODE_START);

	if (pos != std::string::npos ) {
		std::string code = cache.substr(pos + C_CODE_START.size());
		pos = code.find(" ");
		if (pos != std::string::npos) {
			code = code.substr(0, pos);
			result = stoll(code);
		}
	}

	return result;
}

bool AoC2019_day25::part1() {
	result1_ = std::to_string(control_droid());

	return true;
}

bool AoC2019_day25::part2() {
	result2_ = std::to_string(0);

	return true;
}

int32_t AoC2019_day25::get_aoc_day() {
	return 25;
}

int32_t AoC2019_day25::get_aoc_year() {
	return 2019;
}

int main(void) {
	AoC2019_day25 day25;

	return day25.main_execution();
}
