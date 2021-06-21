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

#pragma once

#include "fty_userdata_dto.h"
#include <string>
#include <vector>

namespace dto::commands {

// =====================================================================================================================
/// Get commands request object
class GetCommandsQueryDto
{
public:
    std::string asset;

    GetCommandsQueryDto() = default;
    explicit GetCommandsQueryDto(const std::string& assetVal)
        : asset(assetVal)
    {
    }
};

void operator<<(UserData& data, const GetCommandsQueryDto& object);
void operator>>(UserData& inputData, GetCommandsQueryDto& object);

// =====================================================================================================================

class CommandDescription
{
public:
    std::string              asset;
    std::string              command;
    std::string              description;
    std::vector<std::string> targets;

    CommandDescription() = default;
};

void operator<<(UserData& data, const CommandDescription& object);
void operator>>(UserData& inputData, CommandDescription& object);

using CommandDescriptions = std::vector<CommandDescription>;

void operator<<(UserData& data, const CommandDescriptions& object);
void operator>>(UserData& inputData, CommandDescriptions& object);

using GetCommandsReplyDto = CommandDescriptions;

// =====================================================================================================================

class Command
{
public:
    std::string asset;
    std::string command;
    std::string target;
    std::string argument;

    Command() = default;
    Command(const std::string& assetVal, const std::string& commandVal, const std::string& argumentVal)
        : asset(assetVal)
        , command(commandVal)
        , argument(argumentVal)
    {
    }
    Command(const std::string& assetVal, const std::string& commandVal, const std::string& targetVal,
        const std::string& argumentVal)
        : asset(assetVal)
        , command(commandVal)
        , target(targetVal)
        , argument(argumentVal)
    {
    }
};

using Commands = std::vector<Command>;

void operator<<(UserData& data, const Command& object);
void operator>>(UserData& inputData, Command& object);

void operator<<(UserData& data, const Commands& object);
void operator>>(UserData& inputData, Commands& object);

// =====================================================================================================================

/// Get commands request object
class PerformCommandsQueryDto
{
public:
    Commands commands;

    PerformCommandsQueryDto() = default;
    explicit PerformCommandsQueryDto(const Commands& commandsVal)
        : commands(commandsVal)
    {
    }
};

void operator<<(UserData& data, const PerformCommandsQueryDto& object);
void operator>>(UserData& inputData, PerformCommandsQueryDto& object);

// =====================================================================================================================

} // namespace dto::commands
