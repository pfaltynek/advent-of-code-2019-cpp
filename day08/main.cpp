#include "./../common/aoc.hpp"
#include <algorithm>

const uint32_t img_width_px = 25;
const uint32_t img_height_px = 6;

typedef struct LEAVE {
	std::string name;
	std::vector<LEAVE> leaves;
} leave_str;

class AoC2019_day08 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	int32_t get_digits_count();
	std::string decode_image();
	std::vector<std::string> layers_;
	uint32_t w_, h_;
};

bool AoC2019_day08::init(const std::vector<std::string> lines) {
	std::string all;
	uint32_t pos = 0, size = h_ * w_;

	all.clear();
	layers_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {

		size_t pos = lines[i].find_first_not_of("0123456789");
		if (pos != std::string::npos) {
			std::cout << "Invalid input format at line " << i + 1 << " position " << pos << std::endl;
			return false;
		}

		all.append(lines[i]);
	}

	if (all.size() % size) {
		std::cout << "Invalid input size for " << w_ << "x" << h_ << " layers" << std::endl;
		return false;
	}

	while (pos < all.size()) {
		layers_.push_back(all.substr(pos, size));
		pos += size;
	}

	return true;
}

int32_t AoC2019_day08::get_digits_count() {
	int32_t result = 0, min = INT32_MAX, tmp;

	for (uint32_t i = 0; i < layers_.size(); i++) {
		tmp = std::count(layers_[i].begin(), layers_[i].end(), '0');
		if (tmp < min) {
			min = tmp;
			result = std::count(layers_[i].begin(), layers_[i].end(), '1') * std::count(layers_[i].begin(), layers_[i].end(), '2');
		}
	}

	return result;
}

std::string AoC2019_day08::decode_image() {
	std::string tmp = {}, result;

	for (uint32_t i = 0; i < (w_ * h_); i++) {
		char c = ' ', x;
		for (uint32_t l = 0; l < layers_.size(); l++) {
			x = layers_[l][i];
			if (x != '2') {
				if (x == '1') {
					c = '@';
				}
				break;
			}
		}
		tmp += c;
	}
	result.clear();

	for (uint32_t i = 0; i < h_; ++i) {
		result.append("\n");
		result.append(tmp.substr(i * w_, w_));
	}
	result.append("\n");

	return result;
}

int32_t AoC2019_day08::get_aoc_day() {
	return 8;
}

int32_t AoC2019_day08::get_aoc_year() {
	return 2019;
}

void AoC2019_day08::tests() {
	h_ = 2;
	w_ = 2;

	std::string result;

	if (init({"0222112222120000"})) {
		result = decode_image();
	}

	w_ = img_width_px;
	h_ = img_height_px;
}

bool AoC2019_day08::part1() {
	int32_t result;

	result = get_digits_count();

	result1_ = std::to_string(result);
	return true;
}

bool AoC2019_day08::part2() {
	result2_ = decode_image();

	return true;
}

int main(void) {
	AoC2019_day08 day08;

	return day08.main_execution();
}
