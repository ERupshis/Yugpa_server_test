#include "Messages.h"


bool messages::operator==(const Response& lhs, const Response& rhs) {
	return lhs.status == rhs.status && lhs.directory_entries == rhs.directory_entries;
}