/*  =========================================================================
    fty_commands_dto - Fty commands dto

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
    fty_commands_dto - Fty commands dto
@discuss
@end
*/

#include "fty_commands_dto.h"

#include <sstream>
#include <iostream>

namespace dto
{
    namespace commands
    {
        void operator<<(UserData &data, const GetCommandsQueryDto &object) {
            data.push_back(object.asset);
        }

        void operator>>(UserData &inputData, GetCommandsQueryDto &object) {
            object.asset = inputData.front(); inputData.pop_front();
        }

        void operator<<(UserData &data, const CommandDescription &object) {
            data.push_back(object.asset);
            data.push_back(object.command);
            data.push_back(object.description);

            std::stringstream targets;
            for (auto it = object.targets.begin(); it != object.targets.end(); it++) {
                if (it != object.targets.begin()) {
                    targets << ';';
                }
                targets << *it;
            }
            data.push_back(targets.str());
        }

        void operator>>(UserData &inputData, CommandDescription &object) {
            object.asset = inputData.front(); inputData.pop_front();
            object.command = inputData.front(); inputData.pop_front();
            object.description = inputData.front(); inputData.pop_front();

            std::stringstream targets(inputData.front()); inputData.pop_front();
            std::string target;
            while (std::getline(targets, target, ';')) {
                object.targets.push_back(target);
            }
        }

        void operator<<(UserData &data, const CommandDescriptions &object) {
            for (const auto &i : object) {
                data << i;
            }
        }

        void operator>>(UserData &inputData, CommandDescriptions &object) {
            while (!inputData.empty()) {
                CommandDescription cmdDesc;
                inputData >> cmdDesc;
                object.push_back(cmdDesc);
            }
        }

        void operator<<(UserData &data, const Command &object) {
            data.push_back(object.asset);
            data.push_back(object.command);
            data.push_back(object.target);
            data.push_back(object.argument);
        }

        void operator>>(UserData &inputData, Command &object) {
            object.asset = inputData.front(); inputData.pop_front();
            object.command = inputData.front(); inputData.pop_front();
            object.target = inputData.front(); inputData.pop_front();
            object.argument = inputData.front(); inputData.pop_front();
        }

        void operator<<(UserData &data, const Commands &object) {
            for (const auto &i : object) {
                data << i;
            }
        }

        void operator>>(UserData &inputData, Commands &object) {
            while (!inputData.empty()) {
                Command command;
                inputData >> command;
                object.push_back(command);
            }
        }

        void operator<<(UserData &data, const PerformCommandsQueryDto &object) {
            data << object.commands;
        }

        void operator>>(UserData &inputData, PerformCommandsQueryDto &object) {
            inputData >> object.commands;
        }
    }
}
