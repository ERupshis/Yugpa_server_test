#include "Config.h"

#include <iostream>
#include <cstdlib>
#include <string>

int parseInteger(const char* name) {
	int res = 0;

	char* value = nullptr;
	if (_dupenv_s(&value, NULL, name) == 0 && value != nullptr) {
		int value_parsed = std::atoi(value);
		if (value_parsed <= 0) {
			std::cout << "probably incorrect env's value for: " << name << std::endl;
		}

		res = value_parsed;

		free(value);
	}

	return res;
}

void Config::parse() {
	int port_val = parseInteger("PORT");
	port = (port_val == 0) ? port : port_val;

	int thread_num_val = parseInteger("THREADS_NUM");
	threads_num = (thread_num_val == 0) ? threads_num : thread_num_val;

	int cache_interval_val = parseInteger("CACHE_INTERVAL");
	cache_interval = (cache_interval_val == 0) ? cache_interval : cache_interval_val;
}

