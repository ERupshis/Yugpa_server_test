#pragma once

#include <iostream>
#include <mutex>

class Log {
public:
	Log() = default;

	template<typename... Args>
	void info(Args... args);

	template<typename... Args>
	void error(Args... args);

private:
	std::mutex mu_;

};

template<typename... Args>
void Log::info(Args... args) {
	std::lock_guard<std::mutex> lock(mu_);
	((std::cout << args << " "), ...);
	std::cout << std::endl;
}

template<typename... Args>
void Log::error(Args... args) {
	std::lock_guard<std::mutex> lock(mu_);
	((std::cerr << args << " "), ...);
	std::cerr << std::endl;
}