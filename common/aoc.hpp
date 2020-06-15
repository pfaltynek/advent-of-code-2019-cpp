#include <fstream>
#include <iostream>
#include <stdint.h>
#include <time.h>
#include <vector>

class AoC {
  private:
	time_t start_, end_;

	void start_time_measurement() {
		time(&start_);
	}

	int32_t end_time_measurement() {
		time(&end_);
		return static_cast<int32_t>(difftime(end_, start_));
	}

	virtual bool init() {
		std::ifstream input;
		std::string line;
		std::vector<std::string> lines;

		input.open("input.txt", std::ifstream::in);

		if (input.fail()) {
			std::cout << "Error opening input file.\n";
			return false;
		}

		lines.clear();

		while (std::getline(input, line)) {
			lines.push_back(line);
		}

		if (input.is_open()) {
			input.close();
		}

		return init(lines);
	};

  protected:
	virtual bool init(const std::vector<std::string> lines) = 0;
	virtual bool part1() = 0;
	virtual bool part2() = 0;
	virtual void tests() = 0;
	virtual int32_t get_aoc_day() = 0;
	virtual int32_t get_aoc_year() = 0;
	virtual void clean_up() {
	}

	virtual void print_title() {
		std::cout << "=== Advent of Code " << get_aoc_year() << " - day " << get_aoc_day() << " ====" << std::endl;
	}

	virtual void print_part1_title() {
		std::cout << "--- part 1 ---" << std::endl;
	}

	virtual void print_part2_title() {
		std::cout << "--- part 2 ---" << std::endl;
	}

	virtual void print_part1_result() {
		std::cout << "Result is " << result1_ << std::endl;
	}

	virtual void print_part2_result() {
		std::cout << "Result is " << result2_ << std::endl;
	}

	virtual void print_time(int32_t time) {
		if (time) {
			std::cout << "Execution time " << time << " sec." << std::endl;
		} else {
			std::cout << "Execution time less than 1 sec." << std::endl;
		}
	}

	std::string result1_, result2_;
	bool in_testing;

  public:
	int main_execution() {
		int32_t time;

		in_testing = true;

		tests();

		in_testing = false;

		start_time_measurement();

		if (!init()) {
			std::cout << "Input decoding failed" << std::endl;
			return -1;
		}

		print_title();
		print_part1_title();

		if (!part1()) {
			std::cout << "Part 1 failed" << std::endl;
			return -1;
		}

		print_part1_result();

		print_part2_title();

		if (!part2()) {
			std::cout << "Part 2 failed" << std::endl;
			return -1;
		}

		print_part2_result();

		time = end_time_measurement();

		clean_up();

		print_time(time);

		return 0;
	}
};