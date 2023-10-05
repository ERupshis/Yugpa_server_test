#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "Log.h"
#include "Messages.h"

class Cache {
public:
	Cache(int interval, std::shared_ptr<Log> log)
		:cache_interval_(interval),
		log_(log)
	{}

	messages::Response getValueAndRefreshIfNeed(const std::string& key, std::function<messages::Response()>& callback);

private:
	messages::Response getValue(const std::string& key, std::function<messages::Response()>* callback);
	bool hasValue(const std::string& key);

private:
	using timestamp = std::chrono::system_clock::time_point;
	struct value {
		value() = default;

		std::mutex mu;
		messages::Response val;
		timestamp timestamp = std::chrono::system_clock::now();
	};

	std::chrono::minutes cache_interval_;

	std::mutex mu_;
	std::unordered_map<std::string, value> cache_;

	std::shared_ptr<Log> log_ = nullptr;
};
