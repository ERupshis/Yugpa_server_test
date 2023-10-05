#include "Encoder.h"
#include <iostream>

using namespace messages;


// Response.
void Encoder::Serialize(const Response& data, std::ostream& stream)
{
    // response status.
    stream.write(reinterpret_cast<const char*>(&data.status), sizeof(data.status));

    // directory's entries.
    size_t entries_count = data.directory_entries.size();
    stream.write(reinterpret_cast<const char*>(&entries_count), sizeof(entries_count));

    for (const auto& [entry_name, entry_type] : data.directory_entries) {
        // entry's name.
        size_t name_size = entry_name.size();
        stream.write(reinterpret_cast<const char*>(&name_size), sizeof(name_size));
        stream.write(entry_name.c_str(), name_size);
        // entry's type.
        stream.write(reinterpret_cast<const char*>(&entry_type), sizeof(entry_type));
    }
}

void Encoder::Deserialize(Response& data, std::istream& stream)
{
    // response status.
    stream.read(reinterpret_cast<char*>(&data.status), sizeof(data.status));

    // directory's elements.
    size_t entries_count = 0;
    stream.read(reinterpret_cast<char*>(&entries_count), sizeof(entries_count));

    for (size_t i = 0; i < entries_count; ++i) {
        // entry's name.
        size_t name_size = 0;
        stream.read(reinterpret_cast<char*>(&name_size), sizeof(name_size));
        messages::EntryName name;
        name.resize(name_size);
        stream.read(&name[0], name_size);
        //entrys's type.
        messages::EntryType type = messages::EntryType::Undefined;
        stream.read(reinterpret_cast<char*>(&type), sizeof(type));

        data.directory_entries[name] = type;
    }
}


// Request.
void Encoder::Serialize(const messages::Request& data, std::ostream& stream)
{
    size_t req_size = data.path.size();
    stream.write(reinterpret_cast<const char*>(&req_size), sizeof(req_size));
    stream.write(data.path.c_str(), req_size);
}

void Encoder::Deserialize(messages::Request& data, std::istream& stream)
{
    size_t req_size = 0;
    stream.read(reinterpret_cast<char*>(&req_size), sizeof(req_size));
    data.path.resize(req_size);
    stream.read(&data.path[0], req_size);
}
