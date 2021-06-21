/*  =========================================================================
    pb_helper - Helper function for protobuf

    Copyright (C) 2014 - 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

#pragma once

#include "fty_userdata_dto.h"

namespace google::protobuf {
class Message;
}

namespace dto {
// Json string serilizer / deserializer
void operator>>=(const std::string& str, google::protobuf::Message& message);
void operator<<=(std::string& str, const google::protobuf::Message& message);

// Stream in Json format
std::ostream& operator<<(std::ostream& os, const google::protobuf::Message& message);

// Userdata serializer / deserializer
void operator<<(UserData& data, const google::protobuf::Message& message);
void operator>>(UserData& data, google::protobuf::Message& message);

// Helper to ensure the memroy of protobuf is cleaned at the end of the program:
//  This class is use to instentiate a global variable, which will destroy at th
//  end of the program. The destruction will trigger a call to
//  google::protobuf::ShutdownProtobufLibrary()
class ProtobufMemoryCleaner
{
public:
    ProtobufMemoryCleaner() = default;
    ~ProtobufMemoryCleaner();
};

extern ProtobufMemoryCleaner gProtobufMemoryCleaner;
} // namespace dto
