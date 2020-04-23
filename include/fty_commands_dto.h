/*  =========================================================================
    fty_nut_dto - Fty nut dto

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

#ifndef FTY_COMMANDS_DTO_H_INCLUDED
#define FTY_COMMANDS_DTO_H_INCLUDED

#include <string>
#include <vector>

#include "fty_userdata_dto.h"

namespace dto
{
    namespace commands
    {
        /**
         * Get commands request object
         */
        struct GetCommandsQueryDto {
            std::string asset;

            GetCommandsQueryDto() = default;
            GetCommandsQueryDto(const std::string& asset) : asset(asset) {}
        };

        void operator<<(UserData &data, const GetCommandsQueryDto &object);
        void operator>>(UserData &inputData, GetCommandsQueryDto &object);

        struct CommandDescription {
            std::string asset;
            std::string command;
            std::string description;
            std::vector<std::string> targets;

            CommandDescription() = default;
        } ;

        void operator<<(UserData &data, const CommandDescription &object);
        void operator>>(UserData &inputData, CommandDescription &object);

        using CommandDescriptions = std::vector<CommandDescription>;

        void operator<<(UserData &data, const CommandDescriptions &object);
        void operator>>(UserData &inputData, CommandDescriptions &object);

        using GetCommandsReplyDto = CommandDescriptions;

        struct Command {
            std::string asset;
            std::string command;
            std::string target;
            std::string argument;

            Command() = default;
            Command(const std::string &asset, const std::string &command, const std::string &argument) : asset(asset), command(command), argument(argument) {}
            Command(const std::string &asset, const std::string &command, const std::string &target, const std::string &argument) : asset(asset), command(command), target(target), argument(argument) {}
        } ;

        using Commands = std::vector<Command>;

        void operator<<(UserData &data, const Command &object);
        void operator>>(UserData &inputData, Command &object);

        void operator<<(UserData &data, const Commands &object);
        void operator>>(UserData &inputData, Commands &object);

        /**
         * Get commands request object
         */
        struct PerformCommandsQueryDto {
            Commands commands;

            PerformCommandsQueryDto() = default;
            PerformCommandsQueryDto(const Commands& commands) : commands(commands) {}
        };

        void operator<<(UserData &data, const PerformCommandsQueryDto &object);
        void operator>>(UserData &inputData, PerformCommandsQueryDto &object);
    } // nut namespace
    
} // dto namespace

#endif
