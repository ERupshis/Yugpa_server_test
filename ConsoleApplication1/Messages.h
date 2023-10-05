#pragma once

#include <string>
#include <map>

namespace messages {

	enum class EntryType {
		Undefined,
		DIRECTORY,
		FILE,
	};

	// Response message.
	using EntryName = std::string;
	struct Response {
		int32_t status = -1; // handling message status.
		std::map<EntryName, EntryType> directory_entries; // directory's contents. (Can be empty in case of error).
	};

	bool operator==(const Response& lhs, const Response& rhs);


	// Request message.
	struct Request {
		std::string path;
	};

}
