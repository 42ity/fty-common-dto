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

#include "fty_common_dto_classes.h"

#include <map>

namespace dto 
{
    namespace srr 
    {
        /**
         * Config request object
         * @param data
         * @param object
         */
        static const std::map<Action, std::string> actionInString =
        {
            {Action::SAVE,    "save"},
            {Action::RESET,   "reset"},
            {Action::RESTORE, "restore"},
            {Action::UNKNOWN, "unknown"}
        };

        std::string actionToString(Action action)
        {
            const auto it = actionInString.find(action);

            if(it != actionInString.end())
            {
                return it->second;
            }
            else
            {
                return "unknown";
            }
        }

        Action stringToAction(const std::string & actionStr)
        {
            for(const auto & item : actionInString)
            {
                if(item.second == actionStr)
                {
                    return item.first;
                }
            }

            return Action::UNKNOWN;
        }
                
        void operator<<(UserData& data, const ConfigQueryDto &object)
        {
            data.push_back(actionToString(object.action));
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
            object = ConfigQueryDto(stringToAction(action), passPhrase, listTemp, data);
        }

        static const std::map<Status, std::string> statusInString =
        {
            {Status::SUCCESS,           "success"},
            {Status::PARTIAL_SUCCESS,   "partialSuccess"},
            {Status::FAILED,            "failed"},
            {Status::UNKNOWN,           "unknown"}
        };

        std::string statusToString(Status status)
        {
            const auto it = statusInString.find(status);

            if(it != statusInString.end())
            {
                return it->second;
            }
            else
            {
                return "unknown";
            }
        }

        Status stringToStatus(const std::string & statusStr)
        {
            for(const auto & item : statusInString)
            {
                if(item.second == statusStr)
                {
                    return item.first;
                }
            }

            return Status::UNKNOWN;
        }

        /**
         * Config response object
         * @param data
         * @param object
         */
        void operator<< (UserData& data, const ConfigResponseDto& object)
        {
            data.push_back(object.featureName);
            data.push_back(statusToString(object.status));
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
            object = ConfigResponseDto(featureName, stringToStatus(status), data, error);
        }

        /**
         * SRR request object
         * @param data
         * @param object
         */
        void operator<<(UserData& data, const SrrQueryDto& object)
        {
            data.push_back(actionToString(object.action));
            data.push_back(object.data);
        }

        void operator>>(UserData& inputData, SrrQueryDto& object)
        {
            auto action = inputData.front();
            inputData.pop_front();
            auto data = inputData.front();
            inputData.pop_front();
            object = SrrQueryDto(stringToAction(action), data);
        }
        
        /**
         * SrrFeatureDto object
         * @param data
         * @param object
         */
        void operator<<(UserData& data, const SrrFeatureDto& object)
        {
            data.push_back(object.name);
            data.push_back(object.dependencies);
        }

        void operator>>(UserData& inputData, SrrFeatureDto& object)
        {
            object.name = inputData.front();
            inputData.pop_front();
            object.dependencies = inputData.front();
            inputData.pop_front();
        }
        
        /**
         * All features list object threat by SRR
         * @param data
         * @param object
         */
        void operator<<(UserData& data, const SrrFeaturesListDto& object)
        {
            for (auto &srrFeatureDto : object.featuresList)
            {
                data << srrFeatureDto;
            }
        }

        void operator>> (UserData& inputData, SrrFeaturesListDto& object) 
        {    
            while(!inputData.empty())
            {
                SrrFeatureDto currentDto;
                inputData >> currentDto;
                object.featuresList.push_back(currentDto);
            }
        }
        
        /**
         * SRR save object
         * @param data
         * @param object
         */
        void operator<<(UserData& data, const SrrSaveDto& object)
        {
            data.push_back(statusToString(object.status));
            data.push_back(object.config);
        }

        void operator>>(UserData& inputData, SrrSaveDto& object)
        {
            auto status = inputData.front();
            inputData.pop_front();
            auto config = inputData.front();
            inputData.pop_front();
            object = SrrSaveDto(stringToStatus(status), config);
        }

        /**
         * SRR restore object
         * @param data
         * @param object
         */
        void operator<<(UserData& data, const SrrRestoreDto& object)
        {
            data.push_back(object.name);
            data.push_back(statusToString(object.status));
            data.push_back(object.error);
        }

        void operator>>(UserData& inputData, SrrRestoreDto& object)
        {
            auto name = inputData.front();
            inputData.pop_front();
            auto status = inputData.front();
            inputData.pop_front();
            auto error = inputData.front();
            inputData.pop_front();
            object = SrrRestoreDto(name, stringToStatus(status), error);
        }

        /**
         * List of SrrRestoreDto object with a global status
         * @param data
         * @param object
         */
        void operator<< (UserData& data, const SrrRestoreDtoList& object) 
        {
            data.push_back(statusToString(object.status));
            for (auto &srrResponseDto : object.responseList)
            {
                data << srrResponseDto;
            }
        }

        void operator>> (UserData& inputData, SrrRestoreDtoList& object) {
            object.status = stringToStatus(inputData.front());
            inputData.pop_front();
            while(!inputData.empty())
            {
                SrrRestoreDto currentDto;
                inputData >> currentDto;
                object.responseList.push_back(currentDto);
            }
        }
    }
}
