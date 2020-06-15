#include <vector>
#include <string>

std::vector<std::string> split(std::string to_split, const std::string &delimiter) {
	std::vector<std::string> result;
	size_t pos;

	result.clear();

	pos = to_split.find(delimiter, 0);
	while (pos != std::string::npos) {
		if (pos) {
			result.push_back(to_split.substr(0, pos));
		}
		to_split = to_split.substr(pos + delimiter.size(), std::string::npos);

		pos = to_split.find(delimiter, 0);
	}

	if (to_split.size()) {
		result.push_back(to_split);
	}

	return result;
}