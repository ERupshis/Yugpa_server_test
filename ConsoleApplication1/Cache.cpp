#include "Cache.h"


messages::Response Cache::getValueAndRefreshIfNeed(const std::string& key, std::function<messages::Response()>& callback) {
	if (hasValue(key)) {
		log_->info("[Cache::getValueAndRefreshIfNeed] found valid value in cache for key: ", key);
		return getValue(key, nullptr);
	}

	const std::lock_guard<std::mutex> lock(mu_);
	cache_[key];
	log_->info("[Cache::getValueAndRefreshIfNeed] new key was added: ", key);
	return getValue(key, &callback);
}

messages::Response Cache::getValue(const std::string& key, std::function<messages::Response()>* callback) {
	const std::lock_guard<std::mutex> lock(cache_[key].mu);

	if (callback) {
		cache_[key].val = (*callback)();
		cache_[key].timestamp = std::chrono::system_clock::now();
		log_->info("[Cache::getValue] value was refreshed for key: ", key);
	}

	return cache_.at(key).val;
}

bool Cache::hasValue(const std::string& key) {
	if (!cache_.count(key)) {
		return false;
	}

	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - cache_.at(key).timestamp;
	bool is_actual = (cache_interval_ >= elapsed_seconds);
	return is_actual;
}
