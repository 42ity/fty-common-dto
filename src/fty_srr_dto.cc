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
         * SRRQuery
         * 
         */
        SrrQuery SrrQuery::createSave(const std::vector<Feature> & features, const std::string & passpharse)
        {
            SrrQuery query;
            query.action = Action::SAVE;
            query.params = SrrQueryParamsPtr(new SrrSaveParams());

            SrrSaveParams * saveParams = dynamic_cast<SrrSaveParams*>(query.params.get());
            saveParams->passphrase = passpharse;
            saveParams->features = features;

            return query;
        }

        SrrQuery SrrQuery::createRestore(const std::map<Feature, std::string> & restoreData, const std::string & passpharse)
        {
            SrrQuery query;
            query.action = Action::RESTORE;
            query.params = SrrQueryParamsPtr(new SrrRestoreParams());

            SrrRestoreParams * restoreParams = dynamic_cast<SrrRestoreParams*>(query.params.get());
            restoreParams->passphrase = passpharse;
            restoreParams->mapFeaturesData = restoreData;

            return query;
        }

        SrrQuery SrrQuery::createReset(const std::vector<Feature> & features)
        {
            SrrQuery query;
            query.action = Action::RESET;
            query.params = SrrQueryParamsPtr(new SrrResetParams());

            SrrResetParams * resetParams = dynamic_cast<SrrResetParams*>(query.params.get());
            resetParams->features = features;

            return query;
        }

        SrrQuery SrrQuery::createGetListFeature(const std::vector<Feature> & features)
        {
            SrrQuery query;
            query.action = Action::GET_FEATURE_LIST;
            query.params = SrrQueryParamsPtr(new SrrListFeatureParams());

            return query;
        }

        void SrrQuery::fromUserData(UserData & data)
        {
            auto actionStr = data.front();
            data.pop_front();

            action = stringToAction(actionStr);

            switch(action)
            {
                case Action::SAVE:
                    params = SrrQueryParamsPtr(new SrrSaveParams());
                    break;

                case Action::RESTORE:
                    params = SrrQueryParamsPtr(new SrrRestoreParams());
                    break;

                case Action::RESET:
                    params = SrrQueryParamsPtr(new SrrResetParams());
                    break;

                case Action::GET_FEATURE_LIST:
                    params = SrrQueryParamsPtr(new SrrListFeatureParams());
                    break;
                
                default:
                    params = nullptr;
            }

            if(params != nullptr)
            {
                params->fromUserData(data);
            }
        }

        void SrrQuery::toUserData(UserData & data) const
        {
            data.push_back(actionToString(action));
            if(action != Action::UNKNOWN)
            {
                params->toUserData(data);
            }
        }

        void operator>> (UserData & data, SrrQuery & query)
        {
            query.fromUserData(data);
        }

        void operator<< (UserData & data, const SrrQuery & query)
        {
            query.toUserData(data);
        }

        /**
         * SrrQuery Params
         * 
         */

        //save
        void SrrSaveParams::fromUserData(UserData & data)
        {
            //passphrase
            passphrase = data.front();
            data.pop_front();

            //feature list
            for (const auto &feature : data)
            {
                features.push_back(feature);
            }
            data.clear();
        }

        void SrrSaveParams::toUserData(UserData & data) const
        {
            data.push_back(passphrase);
            for(const auto & feature : features)
            {
                data.push_back(feature);
            }
        }

        //restore
        void SrrRestoreParams::fromUserData(UserData & data)
        {
            //passphrase
            passphrase = data.front();
            data.pop_front();

            while (!data.empty())
            {
                Feature featureName = data.front();
                data.pop_front();

                std::string dataPayload = data.front();
                data.pop_front();

                mapFeaturesData[featureName] = dataPayload;
            }
        }

        void SrrRestoreParams::toUserData(UserData & data) const
        {
            data.push_back(passphrase);
            for(const auto & item : mapFeaturesData)
            {
                data.push_back(item.first);
                data.push_back(item.second);
            }
        }

        //reset
        void SrrResetParams::fromUserData(UserData & data)
        {
            //feature list
            for (const auto &feature : data)
            {
                features.push_back(feature);
            }
            data.clear();
        }

        void SrrResetParams::toUserData(UserData & data) const
        {
            for(const auto & feature : features)
            {
                data.push_back(feature);
            }
        }

        /**
         * Response
         * 
         */
        SrrResponse SrrResponse::createSave()
        {
            SrrResponse response;
            response.action = Action::SAVE;
            response.params = SrrResponseParamsPtr(new SrrSaveResponseParams());

            return response;
        }

        SrrResponse SrrResponse::createRestore()
        {
            SrrResponse response;
            response.action = Action::RESTORE;
            response.params = SrrResponseParamsPtr(new SrrRestoreResponseParams());

            return response;
        }

        SrrResponse SrrResponse::createReset()
        {
            SrrResponse response;
            response.action = Action::RESET;
            response.params = SrrResponseParamsPtr(new SrrResetResponseParams());

            return response;
        }

        SrrResponse SrrResponse::createGetListFeature()
        {
            SrrResponse response;
            response.action = Action::GET_FEATURE_LIST;
            response.params = SrrResponseParamsPtr(new SrrListFeatureResponseParams());

            return response;
        }

        void SrrResponse::fromUserData(UserData & data)
        {
            auto actionStr = data.front();
            data.pop_front();
            action = stringToAction(actionStr);

            auto statusStr = data.front();
            data.pop_front();
            status = stringToStatus(statusStr);

            error = data.front();
            data.pop_front();

            switch(action)
            {
                case Action::SAVE:
                    params = SrrResponseParamsPtr(new SrrSaveResponseParams());
                    break;

                case Action::RESTORE:
                    params = SrrResponseParamsPtr(new SrrRestoreResponseParams());
                    break;

                case Action::RESET:
                    params = SrrResponseParamsPtr(new SrrResetResponseParams());
                    break;

                case Action::GET_FEATURE_LIST:
                    params = SrrResponseParamsPtr(new SrrListFeatureResponseParams());
                    break;
                
                default:
                    params = nullptr;
            }

            if(params != nullptr)
            {
                params->fromUserData(data);
            }
        }

        void SrrResponse::toUserData(UserData & data) const
        {
            data.push_back(actionToString(action));
            if(action != Action::UNKNOWN)
            {
                params->toUserData(data);
            }
        }

        void operator>> (UserData & data, SrrResponse & response)
        {
            response.fromUserData(data);
        }

        void operator<< (UserData & data, const SrrResponse & response)
        {
            response.toUserData(data);
        }

        //save response
        void SrrSaveResponseParams::fromUserData(UserData & data)
        {
            while (!data.empty())
            {
                Feature featureName = data.front();
                data.pop_front();

                std::string dataPayload = data.front();
                data.pop_front();

                mapFeaturesData[featureName] = dataPayload;
            }
        }

        void SrrSaveResponseParams::toUserData(UserData & data) const
        {
            for(const auto & item : mapFeaturesData)
            {
                data.push_back(item.first);
                data.push_back(item.second);
            }
        }

        //restore response
        void SrrRestoreResponseParams::fromUserData(UserData & data)
        {
            while (!data.empty())
            {
                Feature featureName = data.front();
                data.pop_front();

                std::string statusStr = data.front();
                data.pop_front();

                mapFeaturesStatus[featureName] = stringToStatus(statusStr);
            }
        }

        void SrrRestoreResponseParams::toUserData(UserData & data) const
        {
            for(const auto & item : mapFeaturesStatus)
            {
                data.push_back(item.first);
                data.push_back(statusToString(item.second));
            }
        }

        //reset response
        void SrrResetResponseParams::fromUserData(UserData & data)
        {
            while (!data.empty())
            {
                Feature featureName = data.front();
                data.pop_front();

                std::string statusStr = data.front();
                data.pop_front();

                mapFeaturesStatus[featureName] = stringToStatus(statusStr);
            }
        }

        void SrrResetResponseParams::toUserData(UserData & data) const
        {
            for(const auto & item : mapFeaturesStatus)
            {
                data.push_back(item.first);
                data.push_back(statusToString(item.second));
            }
        }

        //get feature list response
        void SrrListFeatureResponseParams::fromUserData(UserData & data)
        {
            //feature list
            for (const auto &feature : data)
            {
                features.push_back(feature);
            }
            data.clear();
        }

        void SrrListFeatureResponseParams::toUserData(UserData & data) const
        {
            for(const auto & feature : features)
            {
                data.push_back(feature);
            }
        }

        /**
         * Actions
         */
        static const std::map<Action, std::string> actionInString =
        {
            {Action::SAVE,    "save"},
            {Action::RESET,   "reset"},
            {Action::RESTORE, "restore"},
            {Action::GET_FEATURE_LIST, "getFeatureList"},
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

        /**
         * Status
         */

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
    } // srr
} // dto
