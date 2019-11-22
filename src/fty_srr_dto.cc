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

#include <cxxtools/jsonserializer.h>
#include <cxxtools/jsondeserializer.h>

#include <iostream>

namespace dto 
{
    namespace srr 
    {
        static cxxtools::SerializationInfo deserializeJson(const std::string & json);
        static std::string serializeJson(const cxxtools::SerializationInfo & si, bool beautify = false);
        /**
         * SRRQuery
         * 
         */
        SrrQuery SrrQuery::createSave(const std::set<FeatureName> & features, const std::string & passpharse)
        {
            SrrQuery query;
            query.m_params = SrrQueryParamsPtr(new SrrSaveQuery());

            SrrSaveQuery * saveParams = dynamic_cast<SrrSaveQuery*>(query.m_params.get());
            saveParams->passphrase = passpharse;
            saveParams->features = features;

            return query;
        }

        SrrQuery SrrQuery::createRestore(const std::map<FeatureName, Feature> & restoreData, const std::string & passpharse)
        {
            SrrQuery query;
            query.m_params = SrrQueryParamsPtr(new SrrRestoreQuery());

            SrrRestoreQuery * restoreParams = dynamic_cast<SrrRestoreQuery*>(query.m_params.get());
            restoreParams->passphrase = passpharse;
            restoreParams->mapFeaturesData = restoreData;

            return query;
        }

        SrrQuery SrrQuery::createReset(const std::set<FeatureName> & features)
        {
            SrrQuery query;
            query.m_params = SrrQueryParamsPtr(new SrrResetQuery());

            SrrResetQuery * resetParams = dynamic_cast<SrrResetQuery*>(query.m_params.get());
            resetParams->features = features;

            return query;
        }

        SrrQuery SrrQuery::createGetListFeature()
        {
            SrrQuery query;
            query.m_params = SrrQueryParamsPtr(new SrrListFeatureQuery());

            return query;
        }

        Action SrrQuery::getAction() const
        {
            if(m_params != nullptr)
            {
                return m_params->getAction();
            }
            else
            {
                return Action::UNKNOWN;
            }
            
        }

        bool SrrQuery::isEqual(const SrrQuery & query) const
        {
            bool isEqualValue = false;
            if(getAction() == query.getAction())
            {
                if( (m_params != nullptr) && (query.m_params != nullptr))
                {
                    isEqualValue =  m_params->isEqual(query.m_params);
                }
            }

            return isEqualValue;
        }

        void SrrQuery::fromUserData(UserData & data)
        {
            Action action = stringToAction(data.front());
            data.pop_front();

            switch(action)
            {
                case Action::SAVE:
                    m_params = SrrQueryParamsPtr(new SrrSaveQuery());
                    break;

                case Action::RESTORE:
                    m_params = SrrQueryParamsPtr(new SrrRestoreQuery());
                    break;

                case Action::RESET:
                    m_params = SrrQueryParamsPtr(new SrrResetQuery());
                    break;

                case Action::GET_FEATURE_LIST:
                    m_params = SrrQueryParamsPtr(new SrrListFeatureQuery());
                    break;
                
                default:
                    m_params = nullptr;
            }

            if(m_params != nullptr)
            {
                m_params->deserialize(deserializeJson(data.front()));
                data.pop_front();
            }
        }

        void SrrQuery::toUserData(UserData & data) const
        {
            data.push_back(actionToString(getAction()));
            
            if(getAction() != Action::UNKNOWN)
            {
                cxxtools::SerializationInfo si;
                m_params->serialize(si);
                data.push_back(serializeJson(si));
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

        std::ostream& operator<< (std::ostream& os, const SrrQuery& q)
        {
            os << actionToString(q.getAction()) << std::endl;
            if(q.getAction() != Action::UNKNOWN)
            {
                cxxtools::SerializationInfo si;
                q.getParams()->serialize(si);
                os << serializeJson(si, true) << std::endl;
            }
            return os;
        }

        /**
         * SrrQuery Params
         * 
         */

        //save
        void SrrSaveQuery::deserialize(const cxxtools::SerializationInfo& si)
        {
            si.getMember("passphrase") >>= passphrase;
            const cxxtools::SerializationInfo & featuresSi = si.getMember("featuresList");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);

                FeatureName featureName;

                featureSi.getMember("name") >>= featureName;

                features.insert(featureName);
            }           
        }

        void SrrSaveQuery::serialize(cxxtools::SerializationInfo& si) const
        {
            si.addMember("passphrase") <<= passphrase;

            cxxtools::SerializationInfo & featuresSi = si.addMember("featuresList");
            for( const auto & feature : features)
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");
                
                featureSi.addMember("name") <<= feature;
            }
            
            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        bool SrrSaveQuery::isEqual(const SrrQueryParamsPtr & params) const
        {
            SrrSaveQuery * saveParams = dynamic_cast<SrrSaveQuery*>(params.get());

            bool equal = false;
            if(saveParams != nullptr)
            {
                equal = ((features == saveParams->features) && (passphrase == saveParams->passphrase));
            }

            return equal;
        }

        //restore
        void SrrRestoreQuery::serialize(cxxtools::SerializationInfo& si) const 
        {
            si.addMember("passphrase") <<= passphrase;
            cxxtools::SerializationInfo & featuresSi = si.addMember("features");
            for( const auto & item : mapFeaturesData)
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember(item.first);
                const Feature & f = mapFeaturesData.at(item.first);
                featureSi.addMember("version") <<= f.version;
                featureSi.addMember("data") <<= f.data;
            }
        }

        void SrrRestoreQuery::deserialize(const cxxtools::SerializationInfo& si)
        {
            si.getMember("passphrase") >>= passphrase;
            const cxxtools::SerializationInfo & featuresSi = si.getMember("features");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);

                std::string featureName = featureSi.name();

                Feature f;

                featureSi.getMember("data") >>= f.data;
                featureSi.getMember("version") >>= f.version;

                mapFeaturesData[featureName] = f;
            }  
        }

        bool SrrRestoreQuery::isEqual(const SrrQueryParamsPtr & params) const
        {
            SrrRestoreQuery * restoreParams = dynamic_cast<SrrRestoreQuery*>(params.get());

            bool equal = false;
            if(restoreParams != nullptr)
            {
                equal = ((mapFeaturesData == restoreParams->mapFeaturesData) && (passphrase ==  restoreParams->passphrase));
            }
            return equal;
        }

        //reset
        void SrrResetQuery::serialize(cxxtools::SerializationInfo& si) const
        {
            //si.addMember("passphrase") <<= passphrase;

            cxxtools::SerializationInfo & featuresSi = si.addMember("featuresList");
            for( const auto & feature : features)
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");
                
                featureSi.addMember("name") <<= feature;
            }
            
            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void SrrResetQuery::deserialize(const cxxtools::SerializationInfo& si)        
        {
            //si.getMember("passphrase") >>= passphrase;
            const cxxtools::SerializationInfo & featuresSi = si.getMember("featuresList");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);

                FeatureName featureName;

                featureSi.getMember("name") >>= featureName;

                features.insert(featureName);
            }        
        }

        bool SrrResetQuery::isEqual(const SrrQueryParamsPtr & params) const
        {
            SrrResetQuery * resetParams = dynamic_cast<SrrResetQuery*>(params.get());

            bool equal = false;
            if(resetParams != nullptr)
            {
                equal = ((features == resetParams->features));
            }
            return equal;
        }

        //getListFeature
        bool SrrListFeatureQuery::isEqual(const SrrQueryParamsPtr & params) const
        {
            SrrListFeatureQuery * listFeatureParams = dynamic_cast<SrrListFeatureQuery*>(params.get());

            return (listFeatureParams != nullptr);
        }

        /**
         * Response
         * 
         */
        SrrResponse SrrResponse::createSave( const std::map<FeatureName, std::pair<FeatureStatus,Feature>> & mapFeaturesData)
        {
            SrrResponse response;
            response.m_params = SrrResponseParamsPtr(new SrrSaveResponse());

            SrrSaveResponse * saveParams = dynamic_cast<SrrSaveResponse*>(response.m_params.get());
            saveParams->mapFeaturesData = mapFeaturesData;
            
            return response;
        }

        SrrResponse SrrResponse::createRestore(const std::map<FeatureName, FeatureStatus> & mapStatus)
        {
            SrrResponse response;
            response.m_params = SrrResponseParamsPtr(new SrrRestoreResponse());

            SrrRestoreResponse * restoreParams = dynamic_cast<SrrRestoreResponse*>(response.m_params.get());
            restoreParams->mapFeaturesStatus = mapStatus;

            return response;
        }

        /*SrrResponse SrrResponse::createReset()
        {
            SrrResponse response;
            response.m_params = SrrResponseParamsPtr(new SrrResetResponse());

            return response;
        }

        SrrResponse SrrResponse::createGetListFeature()
        {
            SrrResponse response;
            response.m_params = SrrResponseParamsPtr(new SrrListFeatureResponse());

            return response;
        }*/

        void SrrResponse::fromUserData(UserData & data)
        {
            auto actionStr = data.front();
            data.pop_front();
            Action action = stringToAction(actionStr);

            switch(action)
            {
                case Action::SAVE:
                    m_params = SrrResponseParamsPtr(new SrrSaveResponse());
                    break;

                case Action::RESTORE:
                    m_params = SrrResponseParamsPtr(new SrrRestoreResponse());
                    break;

                /*case Action::RESET:
                    m_params = SrrResponseParamsPtr(new SrrResetResponse());
                    break;

                case Action::GET_FEATURE_LIST:
                    m_params = SrrResponseParamsPtr(new SrrListFeatureResponse());
                    break;*/
                
                default:
                    m_params = nullptr;
            }

            if(m_params != nullptr)
            {
                m_params->deserialize(deserializeJson(data.front()));
                data.pop_front();
            }
        }

        void SrrResponse::toUserData(UserData & data) const
        {
            data.push_back(actionToString(getAction()));
            
            if(getAction() != Action::UNKNOWN)
            {
                cxxtools::SerializationInfo si;
                m_params->serialize(si);
                data.push_back(serializeJson(si));
            } 
        }

        Action SrrResponse::getAction() const
        {
            if(m_params != nullptr)
            {
                return m_params->getAction();
            }
            else
            {
                return Action::UNKNOWN;
            }
            
        }

        bool SrrResponse::isEqual(const SrrResponse & response) const
        {
            bool isEqualValue = false;
            if(getAction() == response.getAction())
            {
                if( (m_params != nullptr) && (response.m_params != nullptr))
                {
                    isEqualValue =  m_params->isEqual(response.m_params);
                }
            }

            return isEqualValue;
        }

        void operator>> (UserData & data, SrrResponse & response)
        {
            response.fromUserData(data);
        }

        void operator<< (UserData & data, const SrrResponse & response)
        {
            response.toUserData(data);
        }

        std::ostream& operator<< (std::ostream& os, const SrrResponse& r)
        {
            os << actionToString(r.getAction()) << std::endl;
            if(r.getAction() != Action::UNKNOWN)
            {
                cxxtools::SerializationInfo si;
                r.getParams()->serialize(si);
                os << serializeJson(si, true) << std::endl;
            }
            return os;
        }

        //save response
        bool SrrSaveResponse::isEqual(const SrrResponseParamsPtr & params) const
        {
            SrrSaveResponse * saveParams = dynamic_cast<SrrSaveResponse*>(params.get());

            bool equal = false;
            if(saveParams != nullptr)
            {
                equal = ((mapFeaturesData == saveParams->mapFeaturesData));
            }

            return equal;
        }

        Status SrrSaveResponse::getGlobalStatus() const
        {
            Status globalStatus = Status::UNKNOWN;

            int nbSuccess = 0;
            for( const auto & item : mapFeaturesData)
            {
                if(item.second.first.status == Status::SUCCESS)
                {
                    nbSuccess++;
                }
            }

            if(nbSuccess == 0)
            {
                globalStatus = Status::FAILED;
            }
            else if(nbSuccess == mapFeaturesData.size())
            {
                globalStatus = Status::SUCCESS;
            }
            else
            {
                globalStatus = Status::PARTIAL_SUCCESS;
            }

            return globalStatus;
        }

        void SrrSaveResponse::deserialize(const cxxtools::SerializationInfo& si)
        {
            const cxxtools::SerializationInfo & featuresSi = si.getMember("features");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {

                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);

                std::string featureName = featureSi.name();

                std::string statusStr, errorStr, versionStr, dataStr;
                featureSi.getMember("status") >>= statusStr;
                featureSi.getMember("error") >>= errorStr;
                featureSi.getMember("version") >>= versionStr;
                featureSi.getMember("data") >>= dataStr;

                mapFeaturesData[featureName] = {{stringToStatus(statusStr),errorStr},{versionStr,dataStr}};
            }  
        }

        void SrrSaveResponse::serialize(cxxtools::SerializationInfo& si) const
        {
            si.addMember("status") <<= statusToString(getGlobalStatus());
            cxxtools::SerializationInfo & featuresSi = si.addMember("features");
            
            for( const auto & item : mapFeaturesData)
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember(item.first);
                const auto & pair = item.second;

                featureSi.addMember("status") <<= statusToString(pair.first.status);
                featureSi.addMember("error") <<= pair.first.errorMsg;
                featureSi.addMember("version") <<= pair.second.version;
                featureSi.addMember("data") <<= pair.second.data;
            }
        }

        //restore response
        bool SrrRestoreResponse::isEqual(const SrrResponseParamsPtr & params) const
        {
            SrrRestoreResponse * restoreParams = dynamic_cast<SrrRestoreResponse*>(params.get());

            bool equal = false;
            if(restoreParams != nullptr)
            {
                equal = ((mapFeaturesStatus == restoreParams->mapFeaturesStatus));
            }

            return equal;
        }

        Status SrrRestoreResponse::getGlobalStatus() const
        {
            Status globalStatus = Status::UNKNOWN;

            int nbSuccess = 0;
            for( const auto & item : mapFeaturesStatus)
            {
                if(item.second.status == Status::SUCCESS)
                {
                    nbSuccess++;
                }
            }

            if(nbSuccess == 0)
            {
                globalStatus = Status::FAILED;
            }
            else if(nbSuccess == mapFeaturesStatus.size())
            {
                globalStatus = Status::SUCCESS;
            }
            else
            {
                globalStatus = Status::PARTIAL_SUCCESS;
            }

            return globalStatus;
        }

        void SrrRestoreResponse::deserialize(const cxxtools::SerializationInfo& si)
        {
            const cxxtools::SerializationInfo & featuresSi = si.getMember("statusList");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);

                std::string statusStr, errorStr, featureName;
                featureSi.getMember("status") >>= statusStr;
                featureSi.getMember("error") >>= errorStr;
                featureSi.getMember("name") >>= featureName;

                mapFeaturesStatus[featureName] = {stringToStatus(statusStr),errorStr};
            }  
        }

        void SrrRestoreResponse::serialize(cxxtools::SerializationInfo& si) const
        {
            si.addMember("status") <<= statusToString(getGlobalStatus());
            cxxtools::SerializationInfo & featuresSi = si.addMember("statusList");
            
            for( const auto & item : mapFeaturesStatus)
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");

                featureSi.addMember("name") <<= item.first;
                featureSi.addMember("status") <<= statusToString(item.second.status);
                featureSi.addMember("error") <<= item.second.errorMsg;
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }
        /*void SrrRestoreResponse::fromUserData(UserData & data)
        {
            while (!data.empty())
            {
                FeatureName featureName = data.front();
                data.pop_front();

                std::string statusStr = data.front();
                data.pop_front();

                mapFeaturesStatus[featureName] = stringToStatus(statusStr);
            }
        }

        void SrrRestoreResponse::toUserData(UserData & data) const
        {
            for(const auto & item : mapFeaturesStatus)
            {
                data.push_back(item.first);
                data.push_back(statusToString(item.second));
            }
        }

        //reset response
        void SrrResetResponse::fromUserData(UserData & data)
        {
            while (!data.empty())
            {
                FeatureName featureName = data.front();
                data.pop_front();

                std::string statusStr = data.front();
                data.pop_front();

                mapFeaturesStatus[featureName] = stringToStatus(statusStr);
            }
        }

        void SrrResetResponse::toUserData(UserData & data) const
        {
            for(const auto & item : mapFeaturesStatus)
            {
                data.push_back(item.first);
                data.push_back(statusToString(item.second));
            }
        }

        //get feature list response
        void SrrListFeatureResponse::fromUserData(UserData & data)
        {
            //feature list
            for (const auto &feature : data)
            {
                features.push_back(feature);
            }
            data.clear();
        }

        void SrrListFeatureResponse::toUserData(UserData & data) const
        {
            for(const auto & feature : features)
            {
                data.push_back(feature);
            }
        }*/

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

        static cxxtools::SerializationInfo deserializeJson(const std::string & json)
        {
            cxxtools::SerializationInfo si;

            try
            {
                std::stringstream input;
                input << json;
                cxxtools::JsonDeserializer deserializer(input);
                deserializer.deserialize(si);
            }
            catch(const std::exception& e)
            {
                throw std::runtime_error("Error in the json from server: "+std::string(e.what()));
            }

            return si;

        }

        static std::string serializeJson(const cxxtools::SerializationInfo & si, bool beautify)
        {
            std::string returnData("");

            try
            {
                std::stringstream output;
                cxxtools::JsonSerializer serializer(output);
                serializer.beautify(beautify);
                serializer.serialize(si);

                returnData = output.str();
            }
            catch(const std::exception& e)
            {
                throw std::runtime_error("Error while creating json "+std::string(e.what()));
            }

            return returnData;
        }
    } // srr
} // dto


#ifdef FTY_COMMON_DTO_BUILD_DRAFT_API
//  --------------------------------------------------------------------------
//  Self tests
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

// color output definition for test function
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

void fty_srr_dto_test (bool verbose)
{
    using namespace dto;
    using namespace dto::srr;
    printf (" * fty_srr_dto_test: ");
    std::vector<std::pair<std::string, bool>> testsResults;

    std::string testNumber;
    std::string testName;

//Next test
    testNumber = "1.1";
    testName = "Check Save Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            SrrQuery query1 = SrrQuery::createSave({"test"},"myPassphrase");
            std::cout << query1 << std::endl;

            //test ==
            SrrQuery query2 = SrrQuery::createSave({"test"},"myPassphrase");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            SrrQuery query3 = SrrQuery::createSave({"testgg"},"hsGH<hkherjg");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            SrrQuery query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

//Next test
    testNumber = "1.2";
    testName = "Check Restore Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            SrrQuery query1 = SrrQuery::createRestore({{"test", {"1.0","data"}}},"myPassphrase");
            std::cout << query1 << std::endl;

            //test ==
            SrrQuery query2 = SrrQuery::createRestore({{"test", {"1.0","data"}}},"myPassphrase");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            SrrQuery query3 = SrrQuery::createRestore({{"test-1", {"1.0","data2"}}},"hsGH<hkherjg");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            SrrQuery query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

//Next test
    testNumber = "1.3";
    testName = "Check Reset Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            SrrQuery query1 = SrrQuery::createReset({"test"});
            std::cout << query1 << std::endl;

            //test ==
            SrrQuery query2 = SrrQuery::createReset({"test"});
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            SrrQuery query3 = SrrQuery::createReset({"testgg"});
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> unserialize
            UserData userdata;
            userdata << query1;

            SrrQuery query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

//Next test
    testNumber = "1.4";
    testName = "Check List FeatureName Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            SrrQuery query1 = SrrQuery::createGetListFeature();
            std::cout << query1 << std::endl;

            //test ==
            SrrQuery query2 = SrrQuery::createGetListFeature();
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            SrrQuery query3 = SrrQuery::createReset({"testgg"});
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            SrrQuery query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

//Next test
    testNumber = "2.1";
    testName = "Check Save Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            FeatureStatus s1 = {Status::SUCCESS, ""};
            Feature f1 = {"1.0", "data"};
            
            std::map<FeatureName, std::pair<FeatureStatus,Feature>> map1;
            map1["test"] = {s1,f1};
            
            SrrResponse r1 = SrrResponse::createSave(map1);
            std::cout << r1 << std::endl;

            //test ==
            SrrResponse r2 = SrrResponse::createSave(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, std::pair<FeatureStatus,Feature>> map2;
            map2["test2"] = {s1,f1};
            
            SrrResponse r3 = SrrResponse::createSave(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            SrrResponse r4;
            userdata >> r4;

            if(r1 != r4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

//Next test
    testNumber = "2.2";
    testName = "Check Restore Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            FeatureStatus s1 = {Status::SUCCESS, ""};
            
            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = s1;
            
            SrrResponse r1 = SrrResponse::createRestore(map1);
            std::cout << r1 << std::endl;

            //test ==
            SrrResponse r2 = SrrResponse::createRestore(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = s1;
            
            SrrResponse r3 = SrrResponse::createRestore(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            SrrResponse r4;
            userdata >> r4;

            if(r1 != r4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

//Collect results

    printf ("\n-------------------------------------------------------------\n");

	uint32_t testsPassed = 0;
	uint32_t testsFailed = 0;


	printf("\tSummary tests from fty_srr_dto_test\n");
	for(const auto & result : testsResults)
	{
		if(result.second)
		{
			printf(ANSI_COLOR_GREEN"\tOK " ANSI_COLOR_RESET "\t%s\n",result.first.c_str());
			testsPassed++;
		}
		else
		{
			printf(ANSI_COLOR_RED"\tNOK" ANSI_COLOR_RESET "\t%s\n",result.first.c_str());
			testsFailed++;
		}
	}

	printf ("\n-------------------------------------------------------------\n");

	if(testsFailed == 0)
	{
		printf(ANSI_COLOR_GREEN"\n %i tests passed, everything is ok\n" ANSI_COLOR_RESET "\n",testsPassed);
	}
	else
	{
		printf(ANSI_COLOR_RED"\n!!!!!!!! %i/%i tests did not pass !!!!!!!! \n" ANSI_COLOR_RESET "\n",testsFailed,(testsPassed+testsFailed));

		assert(false);
	}

    printf ("OK\n");
}


#endif // FTY_COMMON_DTO_BUILD_DRAFT_API
