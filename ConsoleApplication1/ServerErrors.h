#pragma once

enum class ServerError {
	Undefined = 0,
	OK = 200,
	BAD_REQUEST = 400,
	INTERNAL_ERROR = 500,
};

int32_t getErrorCode(ServerError error) {
	return static_cast<int32_t>(error);
}
