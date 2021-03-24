#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include "./../common/string-utils.hpp"
#include <array>
#include <regex>

const std::regex C_DEAL_NEW_STACK("^deal into new stack$");
const std::regex C_CUT_CARDS("^cut (-?\\d+)$");
const std::regex C_DEAL_INCREMENT("deal with increment (\\d+)$");
const uint64_t C_MAX_CARDS_PART1 = 10007;

typedef enum DEAL_TYPE { DEAL_NEW, DEAL_INCREMENT, DEAL_CUT } deal_type_t;

class AoC2019_day22 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	std::vector<std::pair<deal_type_t, int64_t>> deals_;
	std::array<uint64_t, C_MAX_CARDS_PART1> cards_, tmp_cards_;
	void shuffle_cards(const uint64_t cards_cnt);
};

bool AoC2019_day22::init(const std::vector<std::string> lines) {
	int64_t value;
	std::smatch sm;

	deals_.clear();

	for (uint32_t i = 0; i < lines.size(); i++) {
		if (std::regex_match(lines[i], sm, C_DEAL_NEW_STACK)) {
			deals_.push_back({DEAL_NEW, 0});
		} else if (std::regex_match(lines[i], sm, C_CUT_CARDS)) {
			value = std::stoi(sm.str(1));
			deals_.push_back({DEAL_CUT, value});
		} else if (std::regex_match(lines[i], sm, C_DEAL_INCREMENT)) {
			value = std::stoi(sm.str(1));
			deals_.push_back({DEAL_INCREMENT, value});
		} else {
			std::cout << "Invalid input at line " << i + 1 << std::endl;
			return false;
		}
	}

	return true;
}

void AoC2019_day22::shuffle_cards(const uint64_t cards_count) {
	int64_t value, count, rest_count;
	uint64_t next_position = 0;
	uint64_t *from1, *from2, *to1, *to2;

	for (size_t i = 0; i < cards_count; i++) {
		cards_[i] = i;
	}

	for (size_t i = 0; i < deals_.size(); i++) {
		value = deals_[i].second;

		switch (deals_[i].first) {
			case DEAL_NEW:
				std::reverse(cards_.begin(), cards_.begin() + cards_count);
				break;
			case DEAL_INCREMENT:
				next_position = 0;

				for (size_t j = 0; j < cards_count; j++) {
					tmp_cards_[next_position] = cards_[j];
					next_position = (next_position + value) % cards_count;
				}

				cards_.swap(tmp_cards_);
				break;
			case DEAL_CUT:
				to1 = to2 = tmp_cards_.data();
				from1 = from2 = cards_.data();

				count = abs(value);
				rest_count = cards_count - count;

				if (value > 0) {
					to1 = to1 + rest_count;
					from2 = from2 + count;
				} else if (value < 0) {
					from1 = from1 + rest_count;
					to2 = to2 + count;
				}

				memcpy(to1, from1, count * sizeof(uint64_t));
				memcpy(to2, from2, rest_count * sizeof(uint64_t));

				cards_.swap(tmp_cards_);
				break;
			default:
				int z = 13;
				break;
		}
	}
}

void AoC2019_day22::tests() {

	if (init({"deal into new stack"})) {
		shuffle_cards(10); // 9 8 7 6 5 4 3 2 1 0
	}

	if (init({"cut 3"})) {
		shuffle_cards(10); // 3 4 5 6 7 8 9 0 1 2
	}

	if (init({"cut -4"})) {
		shuffle_cards(10); // 6 7 8 9 0 1 2 3 4 5
	}

	if (init({"deal with increment 3"})) {
		shuffle_cards(10); // 0 7 4 1 8 5 2 9 6 3
	}

	if (init({"deal with increment 7", "deal into new stack", "deal into new stack"})) {
		shuffle_cards(10); // 0 3 6 9 2 5 8 1 4 7
	}

	if (init({"cut 6", "deal with increment 7", "deal into new stack"})) {
		shuffle_cards(10); // 3 0 7 4 1 8 5 2 9 6
	}

	if (init({"deal with increment 7", "deal with increment 9", "cut -2"})) {
		shuffle_cards(10); // 6 3 0 7 4 1 8 5 2 9
	}

	if (init({"deal into new stack", "cut -2", "deal with increment 7", "cut 8", "cut -4", "deal with increment 7", "cut 3", "deal with increment 9",
			  "deal with increment 3", "cut -1"})) {
		shuffle_cards(10); // 9 2 5 8 1 4 7 0 3 6
	}
}

bool AoC2019_day22::part1() {
	std::array<uint64_t, C_MAX_CARDS_PART1>::iterator pos;

	shuffle_cards(C_MAX_CARDS_PART1);

	pos = std::find(cards_.begin(), cards_.end(), 2019);

	if (pos != cards_.end()) {
		result1_ = std::to_string(pos - cards_.begin());
	} else {
		return false;
	}

	return true;
}

bool AoC2019_day22::part2() {

	result2_ = std::to_string(0);

	return true;
}

int32_t AoC2019_day22::get_aoc_day() {
	return 22;
}

int32_t AoC2019_day22::get_aoc_year() {
	return 2019;
}

int main(void) {
	AoC2019_day22 day22;

	return day22.main_execution();
}
