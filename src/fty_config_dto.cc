/*  =========================================================================
    fty_srr_dto - Fty srr dto

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
    fty_srr_dto - Fty srr dto
@discuss
@end
*/

#include "fty_config_dto.h"

namespace dto
{
    namespace config
    {
        /**
         * Config request object
         * @param data
         * @param object
         */
        void operator<<(UserData& data, const ConfigQueryDto &object)
        {
            data.push_back(object.action);
            data.push_back(object.passPhrase);
            for (const auto &feature : object.features)
            {
                data.push_back(feature);
            }
            data.push_back(object.data);
        }

        void operator>>(UserData& inputData, ConfigQueryDto &object)
        {
            auto action = inputData.front();
            inputData.pop_front();
            auto passPhrase = inputData.front();
            inputData.pop_front();
            auto data = inputData.back();
            inputData.pop_back();
            std::list<std::string> listTemp;
            for (const auto &data : inputData)
            {
                listTemp.push_back(data);
            }
            inputData.clear();
            object = ConfigQueryDto(action, passPhrase, listTemp, data);
        }

        /**
         * Config response object
         * @param data
         * @param object
         */
        void operator<< (UserData& data, const ConfigResponseDto& object)
        {
            data.push_back(object.featureName);
            data.push_back(object.status);
            data.push_back(object.data);
            data.push_back(object.error);
        }

        void operator>> (UserData& inputData, ConfigResponseDto& object)
        {
            auto featureName = inputData.front();
            inputData.pop_front();
            auto status = inputData.front();
            inputData.pop_front();
            auto data = inputData.front();
            inputData.pop_front();
            auto error = inputData.front();
            inputData.pop_front();
            object = ConfigResponseDto(featureName, status, data, error);
        }
    }
}