#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"
#include <regex>

const std::regex recipe_regex("^(.+) => (.+)$");
const std::regex ingredience_regex("^(\\d+) ([A-Z]+)$");
const std::string fuel = "FUEL";
const std::string base = "ORE";

typedef struct RECIPE {
	std::string name;
	uint64_t amount;
	std::map<std::string, uint64_t> inputs;
} recipe_str;

class AoC2019_day14 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool decode_recipe(std::string inputs, std::string output, recipe_str& recipe);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	bool get_ores_required_for_given_fuel_amount(uint64_t fuel_target, uint64_t& ore_amount);
	bool get_fuel_amount_to_use_given_ores(uint64_t ores, uint64_t& fuel_produced);
	std::map<std::string, recipe_str> recipes_;
};

bool AoC2019_day14::init(const std::vector<std::string> lines) {
	std::smatch sm;
	recipe_str recipe;

	recipes_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {

		if (std::regex_match(lines[i], sm, recipe_regex)) {
			if (decode_recipe(sm.str(1), sm.str(2), recipe)) {
				recipes_[recipe.name] = recipe;
			} else {
				std::cout << "Invalid recipe format at line " << i + 1 << std::endl;
				return false;
			}
		} else {
			std::cout << "Invalid recipe at line " << i + 1 << std::endl;
			return false;
		}
	}

	if (recipes_.count(fuel)) {
		return true;
	} else {
		std::cout << "FUEL recipe not found" << std::endl;
		return false;
	}

	return true;
}

bool AoC2019_day14::decode_recipe(std::string inputs, std::string output, recipe_str& recipe) {
	std::smatch sm;
	std::vector<std::string> inps;

	if (std::regex_match(output, sm, ingredience_regex)) {
		recipe.amount = stoi(sm.str(1));
		recipe.name = sm.str(2);
	} else {
		return false;
	}

	inps = split(inputs, ", ");
	recipe.inputs.clear();

	for (uint32_t i = 0; i < inps.size(); i++) {
		if (std::regex_match(inps[i], sm, ingredience_regex)) {
			recipe.inputs[sm.str(2)] = stoi(sm.str(1));
		} else {
			return false;
		}
	}

	return true;
}

bool AoC2019_day14::get_ores_required_for_given_fuel_amount(uint64_t fuel_target, uint64_t& ore_amount) {
	std::map<std::string, uint64_t> required{}, remaining{};
	bool not_found = false;
	std::string name;
	uint64_t amount, multiply, remainer;

	ore_amount = 0;
	required[fuel] = fuel_target;

	while (required.size()) {
		not_found = true;
		for (auto it = required.begin(); it != required.end(); it++) {
			if (recipes_.count(it->first)) {
				name = it->first;
				amount = it->second;
				if (remaining.count(name)) {
					if (remaining[name] > amount) {
						remaining[name] -= amount;
						amount = 0;
					} else {
						amount -= remaining[name];
						remaining.erase(name);
					}
				}

				not_found = false;
				required.erase(it);
				break;
			}
		}

		if (not_found) {
			break;
		} else {
			if (amount) {
				multiply = 1;
				if (amount > recipes_[name].amount) {
					multiply = amount / recipes_[name].amount;
					while ((recipes_[name].amount * multiply) < amount) {
						multiply++;
					}
				}
				remainer = (recipes_[name].amount * multiply) - amount;

				if (remainer) {
					remaining[name] += remainer;
				}

				if ((recipes_[name].inputs.size() == 1) && (recipes_[name].inputs.count(base))) {
					ore_amount += recipes_[name].inputs[base] * multiply;
				} else {
					for (auto it = recipes_[name].inputs.begin(); it != recipes_[name].inputs.end(); it++) {
						required[it->first] += it->second * multiply;
					}
				}
			}
		}
	}

	if (not_found) {
		return false;
	}

	return true;
}

bool AoC2019_day14::get_fuel_amount_to_use_given_ores(uint64_t ores, uint64_t& fuel_produced) {
	uint64_t o, fuel_max, fuel_min, fuel_mid, fuel_mid_prev = 0;

	fuel_produced = 0;

	if (!get_ores_required_for_given_fuel_amount(1, o)) {
		return false;
	}
	fuel_max = ores / o;
	fuel_min = 1;

	if (!get_ores_required_for_given_fuel_amount(fuel_max, o)) {
		return false;
	}

	if (o < ores) {
		do {
			fuel_min = fuel_max;
			fuel_max *= 2;

			if (!get_ores_required_for_given_fuel_amount(fuel_max, o)) {
				return false;
			}
		} while (o < ores);
	}

	fuel_mid = (fuel_max + fuel_min) / 2;

	do {
		if (!get_ores_required_for_given_fuel_amount(fuel_mid, o)) {
			return false;
		}

		if (o > ores) {
			fuel_max = fuel_mid;
		} else if (o < ores) {
			fuel_min = fuel_mid;
		} else {
			fuel_produced = fuel_mid;
			return true;
		}

		fuel_mid_prev = fuel_mid;
		fuel_mid = (fuel_max + fuel_min) / 2;
	} while (fuel_mid_prev != fuel_mid);

	fuel_produced = fuel_mid;

	return true;
}

int32_t AoC2019_day14::get_aoc_day() {
	return 14;
}

int32_t AoC2019_day14::get_aoc_year() {
	return 2019;
}

void AoC2019_day14::tests() {
	uint64_t result;

	if (init({"10 ORE => 10 A", "1 ORE => 1 B", "7 A, 1 B => 1 C", "7 A, 1 C => 1 D", "7 A, 1 D => 1 E", "7 A, 1 E => 1 FUEL"})) {
		if (get_ores_required_for_given_fuel_amount(1, result)) {
			// 31
		}
	}

	if (init({"9 ORE => 2 A", "8 ORE => 3 B", "7 ORE => 5 C", "3 A, 4 B => 1 AB", "5 B, 7 C => 1 BC", "4 C, 1 A => 1 CA", "2 AB, 3 BC, 4 CA => 1 FUEL"})) {
		if (get_ores_required_for_given_fuel_amount(1, result)) {
			// 165
		}
	}

	if (init({"157 ORE => 5 NZVS", "165 ORE => 6 DCFZ", "44 XJWVT, 5 KHKGT, 1 QDVJ, 29 NZVS, 9 GPVTF, 48 HKGWZ => 1 FUEL",
			  "12 HKGWZ, 1 GPVTF, 8 PSHF => 9 QDVJ", "179 ORE => 7 PSHF", "177 ORE => 5 HKGWZ", "7 DCFZ, 7 PSHF => 2 XJWVT", "165 ORE => 2 GPVTF",
			  "3 DCFZ, 7 NZVS, 5 HKGWZ, 10 PSHF => 8 KHKGT"})) {
		if (get_ores_required_for_given_fuel_amount(1, result)) {
			// 13312
			if (get_fuel_amount_to_use_given_ores(1000000000000, result)) {
				// 82892753
			}
		}
	}

	if (init({"2 VPVL, 7 FWMGM, 2 CXFTF, 11 MNCFX => 1 STKFG", "17 NVRVD, 3 JNWZP => 8 VPVL",
			  "53 STKFG, 6 MNCFX, 46 VJHF, 81 HVMC, 68 CXFTF, 25 GNMV => 1 FUEL", "22 VJHF, 37 MNCFX => 5 FWMGM", "139 ORE => 4 NVRVD", "144 ORE => 7 JNWZP",
			  "5 MNCFX, 7 RFSQX, 2 FWMGM, 2 VPVL, 19 CXFTF => 3 HVMC", "5 VJHF, 7 MNCFX, 9 VPVL, 37 CXFTF => 6 GNMV", "145 ORE => 6 MNCFX",
			  "1 NVRVD => 8 CXFTF", "1 VJHF, 6 MNCFX => 4 RFSQX", "176 ORE => 6 VJHF"})) {
		if (get_ores_required_for_given_fuel_amount(1, result)) {
			// 180697
			if (get_fuel_amount_to_use_given_ores(1000000000000, result)) {
				// 5586022
			}
		}
	}

	if (init({"171 ORE => 8 CNZTR", "7 ZLQW, 3 BMBT, 9 XCVML, 26 XMNCP, 1 WPTQ, 2 MZWV, 1 RJRHP => 4 PLWSL", "114 ORE => 4 BHXH", "14 VRPVC => 6 BMBT",
			  "6 BHXH, 18 KTJDG, 12 WPTQ, 7 PLWSL, 31 FHTLT, 37 ZDVW => 1 FUEL", "6 WPTQ, 2 BMBT, 8 ZLQW, 18 KTJDG, 1 XMNCP, 6 MZWV, 1 RJRHP => 6 FHTLT",
			  "15 XDBXC, 2 LTCX, 1 VRPVC => 6 ZLQW", "13 WPTQ, 10 LTCX, 3 RJRHP, 14 XMNCP, 2 MZWV, 1 ZLQW => 1 ZDVW", "5 BMBT => 4 WPTQ", "189 ORE => 9 KTJDG",
			  "1 MZWV, 17 XDBXC, 3 XCVML => 2 XMNCP", "12 VRPVC, 27 CNZTR => 2 XDBXC", "15 KTJDG, 12 BHXH => 5 XCVML", "3 BHXH, 2 VRPVC => 7 MZWV",
			  "121 ORE => 7 VRPVC", "7 XCVML => 6 RJRHP", "5 BHXH, 4 VRPVC => 5 LTCX"})) {
		if (get_ores_required_for_given_fuel_amount(1, result)) {
			// 2210736
			if (get_fuel_amount_to_use_given_ores(1000000000000, result)) {
				// 460664
			}
		}
	}
}

bool AoC2019_day14::part1() {
	uint64_t result = 0;

	if (get_ores_required_for_given_fuel_amount(1, result)) {
		result1_ = std::to_string(result);

		return true;
	} else {
		return false;
	}
}

bool AoC2019_day14::part2() {
	uint64_t result = 0;

	if (get_fuel_amount_to_use_given_ores(1000000000000, result)) {
		result2_ = std::to_string(result);

		return true;
	} else {
		return false;
	}
}

int main(void) {
	AoC2019_day14 day14;

	return day14.main_execution();
}
