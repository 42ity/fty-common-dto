/*  =========================================================================
    pb_helper - Helper function for protobuf

    Copyright (C) 2014 - 2018 Eaton

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

/*
@header
    pb_helper - Helper function for protobuf
@discuss
@end
*/

#include "pb_helper.h"

namespace dto
{
    
    void operator<<=(std::string& str, const google::protobuf::Message & message)
    {
        std::string dataOut;
        google::protobuf::util::JsonPrintOptions options;
        options.preserve_proto_field_names=true;
        options.always_print_primitive_fields=true;
        options.add_whitespace=true;

        if ( google::protobuf::util::MessageToJsonString(message, &dataOut, options) != google::protobuf::util::Status::OK )
        {
            std::runtime_error("Impossible to convert from protobuf to Json.");
        }

        str = dataOut;
    }

    void operator>>=(const std::string& str, google::protobuf::Message & message)
    {
        google::protobuf::util::JsonParseOptions options;
        options.ignore_unknown_fields=true;

        if ( google::protobuf::util::JsonStringToMessage(str, &message, options) != google::protobuf::util::Status::OK )
        {
            std::runtime_error("Impossible to convert from Json to protobuf.");
        }
    }

    std::ostream& operator<< (std::ostream& os, const google::protobuf::Message & message)
    {
        std::string str;
        str <<= message;
        os << str;
        return os;
    }

} //namespace dto
        
