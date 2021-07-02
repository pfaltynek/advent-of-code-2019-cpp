#include "./../common/aoc.hpp"
#include "./../common/coord.hpp"
#include "./../common/string-utils.hpp"
#include <array>
#include <regex>

typedef __uint128_t uint128_t;
typedef __int128_t int128_t;

const std::regex C_DEAL_NEW_STACK("^deal into new stack$");
const std::regex C_CUT_CARDS("^cut (-?\\d+)$");
const std::regex C_DEAL_INCREMENT("deal with increment (\\d+)$");
const uint128_t C_MAX_CARDS_PART1 = 10007;
const uint128_t C_MAX_CARDS_PART2 = 119315717514047;
const uint128_t C_REPETITIONS_PART2 = 101741582076661;

typedef enum DEAL_TYPE { DEAL_NEW, DEAL_INCREMENT, DEAL_CUT } deal_type_t;
typedef std::pair<int128_t, int128_t> koef_pair_t;
class AoC2019_day22 : public AoC {
  protected:
	bool init(const std::vector<std::string> lines);
	bool part1();
	bool part2();
	void tests();
	int32_t get_aoc_day();
	int32_t get_aoc_year();

  private:
	std::vector<std::pair<deal_type_t, int128_t>> deals_;
	std::vector<koef_pair_t> koefs_;
	int128_t a_, b_;

	void find_linear_transformation_koefs(const int128_t deck_size);
	int128_t get_new_card_position(const int128_t card, const int128_t deck_size);
	int128_t calculate_card_pos(const int128_t card, const int128_t deck_size);
	int128_t get_card_number_of_position(const int128_t position, const int128_t repetitions, const int128_t deck_size);
	int128_t normalize(const int128_t result, const int128_t deck_size);
	int128_t normalize_fragment(const int128_t numerator, const int128_t denominator, const int128_t deck_size);
	void find_composite_function_koefs(const int128_t deck_size);
	void invert_function_koefs(const int128_t deck_size);
	void reduce_repetitions(const int128_t repeat_cnt, const int128_t deck_size);
	koef_pair_t compose(const int128_t a1, const int128_t b1, const int128_t a2, const int128_t b2, const int128_t deck_size);
	void prepare_koefs_part2(const int128_t repeat_cnt, const int128_t deck_size);
};

bool AoC2019_day22::init(const std::vector<std::string> lines) {
	int128_t value;
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

int128_t AoC2019_day22::get_new_card_position(const int128_t card, const int128_t deck_size) {
	find_linear_transformation_koefs(deck_size);
	find_composite_function_koefs(deck_size);
	return calculate_card_pos(card, deck_size);
}

void AoC2019_day22::find_linear_transformation_koefs(const int128_t deck_size) {
	int128_t value, a, b;

	koefs_.clear();

	for (size_t i = 0; i < deals_.size(); i++) {
		value = deals_[i].second;

		a = b = 0;

		switch (deals_[i].first) {
			case DEAL_NEW:
				a = -1;
				b = deck_size - 1;
				break;
			case DEAL_INCREMENT:
				a = value;
				b = 0;
				break;
			case DEAL_CUT:
				a = 1;
				b = -value;
				break;
			default:
				int z = 13;
				break;
		}
		koefs_.push_back({a, b});
	}
}

void AoC2019_day22::find_composite_function_koefs(const int128_t deck_size) {
	int128_t a, b, c, d;

	if (koefs_.size()) {
		a = koefs_[0].first;
		b = koefs_[0].second;

		for (size_t i = 1; i < koefs_.size(); i++) {
			c = koefs_[i].first;
			d = koefs_[i].second;
			auto p = compose(a, b, c, d, deck_size);
			a = p.first;
			b = p.second;
		}
		a_ = a;
		b_ = b;
	}
}

int128_t AoC2019_day22::calculate_card_pos(const int128_t card, const int128_t deck_size) {
	return normalize(a_ * card + b_, deck_size);
}

int128_t AoC2019_day22::normalize(const int128_t result, const int128_t deck_size) {
	if (result >= 0) {
		return result % deck_size;
	} else {
		return deck_size - (-result % deck_size);
	}
}

int128_t AoC2019_day22::normalize_fragment(const int128_t numerator, const int128_t denominator, const int128_t deck_size) {
	int128_t value = numerator;

	while (value % denominator) {
		value += deck_size;
	}

	return value / denominator;
}

koef_pair_t AoC2019_day22::compose(const int128_t a1, const int128_t b1, const int128_t a2, const int128_t b2, const int128_t deck_size) {
	return {normalize(a1 * a2, deck_size), normalize(a2 * b1 + b2, deck_size)};
}

void AoC2019_day22::invert_function_koefs(const int128_t deck_size) {
	std::vector<koef_pair_t> inv = {};
	int128_t a, b, ai, bi;

	for (size_t i = 0; i < koefs_.size(); i++) {
		a = koefs_[i].first;
		b = koefs_[i].second;
		ai = normalize_fragment(1, a, deck_size);
		bi = normalize_fragment(-b, a, deck_size);
		inv.push_back({ai, bi});
	}

	koefs_.swap(inv);
}

void AoC2019_day22::reduce_repetitions(const int128_t repeat_cnt, const int128_t deck_size) {
	int128_t a, b, aa, bb, pos = 1;
	bool found = (repeat_cnt % 2) != 0;

	a = aa = a_;
	b = bb = b_;

	while (pos <= repeat_cnt) {
		auto p = compose(a, b, a, b, deck_size);
		a = p.first;
		b = p.second;
		pos = pos << 1;

		if ((pos & repeat_cnt) != 0) {
			if (!found) {
				aa = a;
				bb = b;
				found = true;
			} else {
				auto p = compose(a, b, aa, bb, deck_size);
				aa = p.first;
				bb = p.second;
			}
		}
	}

	a_ = aa;
	b_ = bb;
}

void AoC2019_day22::prepare_koefs_part2(const int128_t repeat_cnt, const int128_t deck_size) {

	find_linear_transformation_koefs(deck_size);
	std::reverse(koefs_.begin(), koefs_.end());
	invert_function_koefs(deck_size);
	find_composite_function_koefs(deck_size);

	if (repeat_cnt > 1) {
		reduce_repetitions(repeat_cnt, deck_size);
	}
}

int128_t AoC2019_day22::get_card_number_of_position(const int128_t position, const int128_t repetitions, const int128_t deck_size) {
	prepare_koefs_part2(repetitions, deck_size);
	return calculate_card_pos(position, deck_size);
}

void AoC2019_day22::tests() {
	std::vector<int128_t> result1(10), result2(10);

	if (init({"deal into new stack"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"cut 3"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"cut -4"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"deal with increment 3"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"deal with increment 7", "deal into new stack", "deal into new stack"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"cut 6", "deal with increment 7", "deal into new stack"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"deal with increment 7", "deal with increment 9", "cut -2"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}

	if (init({"deal into new stack", "cut -2", "deal with increment 7", "cut 8", "cut -4", "deal with increment 7", "cut 3", "deal with increment 9",
			  "deal with increment 3", "cut -1"})) {
		find_linear_transformation_koefs(10);
		find_composite_function_koefs(10);

		for (size_t i = 0; i < 10; i++) {
			result1[calculate_card_pos(i, 10)] = i;
		}

		prepare_koefs_part2(1, 10);

		for (size_t i = 0; i < 10; i++) {
			result2[result1[i]] = calculate_card_pos(i, 10);
		}
	}
}

bool AoC2019_day22::part1() {

	result1_ = std::to_string((uint64_t)get_new_card_position(2019, C_MAX_CARDS_PART1));

	return true;
}

bool AoC2019_day22::part2() {

	result2_ = std::to_string((uint64_t)get_card_number_of_position(2020, C_REPETITIONS_PART2, C_MAX_CARDS_PART2));

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
