#include "./../common/aoc.hpp"
#include "./../common/string-utils.hpp"

class AoC2019_day16 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	std::string get_output_of_phases(const uint32_t phases_count);
	std::string get_output_of_phases_part2(const uint32_t phases_count, const uint32_t repeated);
	std::vector<int8_t> signal_;
	const std::vector<int8_t> base_pattern_ = {0, 1, 0, -1};
};

bool AoC2019_day16::init(const std::vector<std::string> lines) {

	signal_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		for (uint32_t j = 0; j < lines[i].size(); j++) {
			if ((lines[i][j] >= '0') && (lines[i][j] <= '9')) {
				signal_.push_back(static_cast<uint8_t>(lines[i][j] - '0'));
			} else {
				std::cout << "Non numeric character in input at " << signal_.size() + 1 << std::endl;
				return false;
			}
		}
	}

	return true;
}

std::string AoC2019_day16::get_output_of_phases(const uint32_t phases_count) {
	std::string result = "";
	std::vector<std::vector<int8_t>> patterns = {};
	std::vector<int8_t> input, output = {}, pattern;
	int64_t sum, repeat, count;
	uint8_t base_pattern_idx;

	output = signal_;

	for (uint32_t i = 0; i < phases_count; i++) {
		input = output;
		output.clear();

		for (uint64_t j = 0; j < input.size(); j++) {
			if (patterns.size() <= j) {
				pattern.clear();
				repeat = j + 1;
				count = 1;
				base_pattern_idx = 0;
				while (pattern.size() < input.size()) {
					pattern.insert(pattern.end(), repeat - count, base_pattern_[base_pattern_idx]);
					count = 0;
					base_pattern_idx = (base_pattern_idx + 1) % base_pattern_.size();
				}
				patterns.push_back(pattern);
			}
			sum = 0;
			for (uint64_t k = 0; k < input.size(); k++) {
				sum += (input[k] * patterns[j][k]);
			}
			output.push_back(abs(sum) % 10);
		}
	}

	for (uint8_t i = 0; i < 8; ++i) {
		if (i >= output.size()) {
			break;
		}
		result += (output[i] + '0');
	}

	return result;
}

// solution idea from https://elixirforum.com/t/advent-of-code-2019-day-16/27605/2
// and https://www.reddit.com/r/adventofcode/comments/ebf5cy/2019_day_16_part_2_understanding_how_to_come_up/
std::string AoC2019_day16::get_output_of_phases_part2(const uint32_t phases_count, const uint32_t repeated) {
	std::string result = "";
	std::vector<int8_t> input = {}, output = {};
	uint64_t offset;

	offset = 0;
	for (uint8_t i = 0; i < 7; i++) {
		offset *= 10;
		offset += signal_[i];
	}

	for (uint32_t i = 0; i < repeated; i++) {
		output.insert(output.end(), signal_.begin(), signal_.end());
	}

	input.insert(input.begin(), output.begin() + offset - 1, output.end());
	output = input;

	for (uint32_t i = 0; i < phases_count; i++) {
		for (int32_t j = output.size() - 2; j >= 0; j--)
		{
			output[j] = (input[j] + output[j + 1]) % 10;
		}
		input = output;
	}

	for (uint8_t i = 1; i < 9; ++i) {
		if (i >= output.size()) {
			break;
		}
		result += (output[i] + '0');
	}

	return result;
}

int32_t AoC2019_day16::get_aoc_day() {
	return 16;
}

int32_t AoC2019_day16::get_aoc_year() {
	return 2019;
}

void AoC2019_day16::tests() {
	std::string result;

	if (init({"12345678"})) {
		result = get_output_of_phases(4); // 01029498
	}

	if (init({"80871224585914546619083218645595"})) {
		result = get_output_of_phases(100); // 24176176
	}

	if (init({"19617804207202209144916044189917"})) {
		result = get_output_of_phases(100); // 73745418
	}

	if (init({"69317163492948606335995924319873"})) {
		result = get_output_of_phases(100); // 52432133
	}

	if (init({"03036732577212944063491565474664"})) {
		result = get_output_of_phases_part2(100, 10000); // 84462026
	}

	if (init({"02935109699940807407585447034323"})) {
		result = get_output_of_phases_part2(100, 10000); // 78725270
	}

	if (init({"03081770884921959731165446850517"})) {
		result = get_output_of_phases_part2(100, 10000); // 53553731
	}
}

bool AoC2019_day16::part1() {

	result1_ = get_output_of_phases(100);

	return true;
}

bool AoC2019_day16::part2() {

	result2_ = get_output_of_phases_part2(100, 10000);

	return true;
}

int main(void) {
	AoC2019_day16 day16;

	return day16.main_execution();
}
