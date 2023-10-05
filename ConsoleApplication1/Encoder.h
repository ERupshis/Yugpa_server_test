#pragma once

#include "Messages.h"

class Encoder {
public:
	static void Serialize(const messages::Response& data, std::ostream& stream);
	static void Deserialize(messages::Response& data, std::istream& stream);

	static void Serialize(const messages::Request& data, std::ostream& stream);
	static void Deserialize(messages::Request& data, std::istream& stream);

private:
	Encoder() = default;

};
