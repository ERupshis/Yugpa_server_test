#pragma once

struct Config {
	explicit Config(int port, int threads_num)
		:port(port), threads_num(threads_num)
	{}

	int port = 8080;
	int threads_num = 1;
	int cache_interval = 1;

	void parse();
};