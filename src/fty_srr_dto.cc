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

#include <google/protobuf/util/json_util.h>

#include <iostream>

namespace dto 
{
    namespace srr 
    {
        static cxxtools::SerializationInfo deserializeJson(const std::string & json);
        static std::string serializeJson(const cxxtools::SerializationInfo & si, bool beautify = false);

        /**
         * Query wrapper functions
         * 
         */

        Query createSaveQuery(const std::set<FeatureName> & features, const std::string & passpharse)
        {
            Query query;

            SaveQuery & saveQuery = *(query.mutable_save());
            *(saveQuery.mutable_features()) = {features.begin(), features.end()};
            saveQuery.set_passpharse(passpharse);

            return query;
        }

        Query createRestoreQuery(const std::map<FeatureName, Feature> & restoreData, const std::string & passpharse)
        {
            Query query;

            RestoreQuery & restoreQuery = *(query.mutable_restore());
            *(restoreQuery.mutable_map_features_data()) = {restoreData.begin(), restoreData.end()};
            restoreQuery.set_passpharse(passpharse);

            return query;
        }

        Query createResetQuery(const std::set<FeatureName> & features)
        {
            Query query;

            ResetQuery & resetQuery = *(query.mutable_reset());
            *(resetQuery.mutable_features()) = {features.begin(), features.end()};

            return query;
        }

        Query createListFeatureQuery()
        {
            Query query;

            query.mutable_list_feature();

            return query;
        }

        void operator>> (UserData & data, Query & query)
        {
            std::string payload = data.front();
            data.pop_front();

            if(!query.ParseFromString(payload))
            {
                throw std::runtime_error("Impossible to deserialize");
            }
        }

        void operator<< (UserData & data, const Query & query)
        {
            std::string payload;
            if(!query.SerializeToString(&payload))
            {
                throw std::runtime_error("Impossible to serialize");
            }
            data.push_back(payload);
        }

        std::ostream& operator<< (std::ostream& os, const Query& q)
        {
            std::string dataOut;
            google::protobuf::util::JsonPrintOptions options;
            options.preserve_proto_field_names=true;
            options.add_whitespace=true;

            if ( google::protobuf::util::MessageToJsonString(q, &dataOut, options) != google::protobuf::util::Status::OK )
            {
                std::runtime_error("Impossible to convert to string.");
            }

            os << "Query:\n"<< dataOut;
            return os;
        }

        //deserilizer from UI Json
        Query saveQueryFromUiJson(const std::string & json)
        {
            Query query;
            const cxxtools::SerializationInfo si = deserializeJson(json);

            si >>= *(query.mutable_save());

            return query;
        }

        Query restoreQueryFromUiJson(const std::string & json)
        {
            Query query;
            const cxxtools::SerializationInfo si = deserializeJson(json);

            si >>= *(query.mutable_reset());

            return query;
        }

        Query resetQueryFromUiJson(const std::string & json)
        {
            Query query;
            const cxxtools::SerializationInfo si = deserializeJson(json);

            si >>= *(query.mutable_reset());

            return query;
        }
             
        void operator>>= (const cxxtools::SerializationInfo& si, SaveQuery & query)
        {
            si.getMember("passphrase") >>= *(query.mutable_passpharse());

            const cxxtools::SerializationInfo & featuresSi = si.getMember("featuresList");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);
                featureSi.getMember("name") >>= *(query.add_features());
            }           
        }
        
        void operator>>= (const cxxtools::SerializationInfo& si, RestoreQuery & query)
        {
            si.getMember("passphrase") >>= *(query.mutable_passpharse());

            google::protobuf::Map<std::string, Feature > & mapFeaturesData = *(query.mutable_map_features_data());

            const cxxtools::SerializationInfo & featuresSi = si.getMember("features");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);

                std::string featureName = featureSi.name();

                Feature f;

                featureSi.getMember("version") >>= *(f.mutable_version());

                const cxxtools::SerializationInfo & dataSi = featureSi.getMember("data");
                f.set_data(serializeJson(dataSi));
                
                mapFeaturesData[featureName] = std::move(f);
            }  
        }

        void operator>>= (const cxxtools::SerializationInfo& si, ResetQuery & query)        
        {
            const cxxtools::SerializationInfo & featuresSi = si.getMember("featuresList");

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);
                featureSi.getMember("name") >>= *(query.add_features());
            }        
        }
        
        bool operator==(const Query& lhs, const Query& rhs)
        {
            bool areEquals = false;
            if(lhs.parameters_case() ==  rhs.parameters_case())
            {
                switch (lhs.parameters_case())
                {
                case Query::ParametersCase::kSave :
                    areEquals = (lhs.save() == rhs.save());
                    break;

                case Query::ParametersCase::kRestore :
                    areEquals = (lhs.restore() == rhs.restore());
                    break;
                
                case Query::ParametersCase::kReset :
                    areEquals = (lhs.reset() == rhs.reset());
                    break;

                case Query::ParametersCase::kListFeature :
                    areEquals = (lhs.list_feature() == rhs.list_feature());
                    break;
                
                default:
                    break;
                }
            }

            return areEquals;
        }
        
        bool operator==(const SaveQuery& lhs, const SaveQuery& rhs)
        {
            bool areEquals = false;
            if(lhs.passpharse() == rhs.passpharse())
            {
                std::set<FeatureName> set1(lhs.features().begin(), lhs.features().end());
                std::set<FeatureName> set2(rhs.features().begin(), rhs.features().end());
                
                areEquals = (set1 == set2);
            }
            
            return areEquals;
        }
        
        bool operator==(const RestoreQuery& lhs, const RestoreQuery& rhs)
        {
            bool areEquals = false;
            if(lhs.passpharse() == rhs.passpharse())
            {
                std::map<FeatureName, Feature> map1(lhs.map_features_data().begin(), lhs.map_features_data().end());
                std::map<FeatureName, Feature> map2(rhs.map_features_data().begin(), rhs.map_features_data().end());
                
                areEquals = (map1 == map2);
            }

            return areEquals;
        }
        
        bool operator==(const ResetQuery& lhs, const ResetQuery& rhs)
        {
            std::set<FeatureName> set1(lhs.features().begin(), lhs.features().end());
            std::set<FeatureName> set2(rhs.features().begin(), rhs.features().end());
                
            return (set1 == set2);
        }

        /**
         * Response wrapper functions
         * 
         */

        //create functions
        Response createSaveResponse(const std::map<FeatureName, FeatureAndStatus> & mapFeaturesData)
        {
            Response response;

            SaveResponse & saveResponse = *(response.mutable_save());
            *(saveResponse.mutable_map_features_data()) = {mapFeaturesData.begin(), mapFeaturesData.end()};

            return response;
        }

        Response createRestoreResponse(const std::map<FeatureName, FeatureStatus> & mapStatus)
        {
            Response response;

            RestoreResponse & restoreResponse = *(response.mutable_restore());
            *(restoreResponse.mutable_map_features_status()) = {mapStatus.begin(), mapStatus.end()};

            return response;
        }

        Response createResetResponse(const std::map<FeatureName, FeatureStatus> & mapStatus)
        {
            Response response;

            ResetResponse & resetResponse = *(response.mutable_reset());
            *(resetResponse.mutable_map_features_status()) = {mapStatus.begin(), mapStatus.end()};

            return response;
        }

        Response createGetListFeatureResponse(const std::map<FeatureName, FeatureDependencies> & mapFeaturesDependencies)
        {
            Response response;

            ListFeatureResponse & listResponse = *(response.mutable_list_feature());
            *(listResponse.mutable_map_features_dependencies()) = {mapFeaturesDependencies.begin(), mapFeaturesDependencies.end()};

            return response;
        }

        //userdata serializer / deserializer
        void operator>> (UserData & data, Response & response)
        {
            std::string payload = data.front();
            data.pop_front();

            if(!response.ParseFromString(payload))
            {
                throw std::runtime_error("Impossible to deserialize");
            }
        }

        void operator<< (UserData & data, const Response & response)
        {
            std::string payload;
            if(!response.SerializeToString(&payload))
            {
                throw std::runtime_error("Impossible to serialize");
            }
            data.push_back(payload);
        }

        //ostream serializer => for tests mostly
        std::ostream& operator<< (std::ostream& os, const Response& r)
        {
            std::string dataOut;
            google::protobuf::util::JsonPrintOptions options;
            options.preserve_proto_field_names=true;
            options.add_whitespace=true;

            if ( google::protobuf::util::MessageToJsonString(r, &dataOut, options) != google::protobuf::util::Status::OK )
            {
                std::runtime_error("Impossible to convert to string.");
            }

            os << "Response:\n"<< dataOut;
            return os;
        }

        //Comparison operators => for tests mostly      
        bool operator==(const FeatureDependencies& lhs, const FeatureDependencies& rhs)
        {
            std::set<FeatureName> set1(lhs.dependencies().begin(), lhs.dependencies().end());
            std::set<FeatureName> set2(rhs.dependencies().begin(), rhs.dependencies().end());
                
            return (set1 == set2);
        }
        
        bool operator==(const Response& lhs, const Response& rhs)
        {
            bool areEquals = false;
            if(lhs.parameters_case() ==  rhs.parameters_case())
            {
                switch (lhs.parameters_case())
                {
                case Response::ParametersCase::kSave :
                    areEquals = (lhs.save() == rhs.save());
                    break;

                case Response::ParametersCase::kRestore :
                    areEquals = (lhs.restore() == rhs.restore());
                    break;
                
                case Response::ParametersCase::kReset :
                    areEquals = (lhs.reset() == rhs.reset());
                    break;

                case Response::ParametersCase::kListFeature :
                    areEquals = (lhs.list_feature() == rhs.list_feature());
                    break;
                
                default:
                    break;
                }
            }

            return areEquals;
        }

        bool operator==(const SaveResponse& lhs, const SaveResponse& rhs)
        {
            std::map<FeatureName, FeatureAndStatus> map1(lhs.map_features_data().begin(), lhs.map_features_data().end());
            std::map<FeatureName, FeatureAndStatus> map2(rhs.map_features_data().begin(), rhs.map_features_data().end());
                
            return (map1 == map2);
        }
        
        bool operator==(const RestoreResponse& lhs, const RestoreResponse& rhs)
        {
            std::map<FeatureName, FeatureStatus> map1(lhs.map_features_status().begin(), lhs.map_features_status().end());
            std::map<FeatureName, FeatureStatus> map2(rhs.map_features_status().begin(), rhs.map_features_status().end());
                
            return (map1 == map2);
        }
        
        bool operator==(const ResetResponse& lhs, const ResetResponse& rhs)
        {
            std::map<FeatureName, FeatureStatus> map1(lhs.map_features_status().begin(), lhs.map_features_status().end());
            std::map<FeatureName, FeatureStatus> map2(rhs.map_features_status().begin(), rhs.map_features_status().end());
                
            return (map1 == map2);
        }
        
        bool operator==(const ListFeatureResponse& lhs, const ListFeatureResponse& rhs)
        {
            std::map<FeatureName, FeatureDependencies> map1(lhs.map_features_dependencies().begin(), lhs.map_features_dependencies().end());
            std::map<FeatureName, FeatureDependencies> map2(rhs.map_features_dependencies().begin(), rhs.map_features_dependencies().end());
                
            return (map1 == map2);
        }

        /*Response operator+(const Response & r1, const Response & r2)
        {
            Response r = r1;
            r.add(r2);
            return r;
        }

        Response& operator+=(Response & r1, const Response & r2)
        {
            if(r1.parameters_case() != r2.parameters_case())
            {
                throw std::runtime_error("Response with different type cannot be added");
            }
            return r1;
        }*/

        /*void SrrResponse::add(const SrrResponse & r)
        {
            if(getAction() != r.getAction())
            {
                throw std::runtime_error("Impossible to add 2 differents type of response");
            }

            if(getAction() == Action::UNKNOWN)
            {
                throw std::runtime_error("Impossible to add Unknown response");
            }

            m_params->add(r.m_params);
        }


        //save response
        Status SrrSaveResponse::getGlobalStatus() const
        {
            Status globalStatus = Status::UNKNOWN;

            unsigned int nbSuccess = 0;
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

        void SrrSaveResponse::add(const SrrResponseParamsPtr & params)
        {
            SrrSaveResponse * saveParams = dynamic_cast<SrrSaveResponse*>(params.get());
            mapFeaturesData.insert(saveParams->mapFeaturesData.begin(), saveParams->mapFeaturesData.end());
        }

        //restore response
        Status SrrRestoreResponse::getGlobalStatus() const
        {
            Status globalStatus = Status::UNKNOWN;

            unsigned int nbSuccess = 0;
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

        void SrrRestoreResponse::add(const SrrResponseParamsPtr & params)
        {
            SrrRestoreResponse * restoreParams = dynamic_cast<SrrRestoreResponse*>(params.get());
            mapFeaturesStatus.insert(restoreParams->mapFeaturesStatus.begin(), restoreParams->mapFeaturesStatus.end());
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

        //reset response
        Status SrrResetResponse::getGlobalStatus() const
        {
            Status globalStatus = Status::UNKNOWN;

            unsigned int nbSuccess = 0;
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

        void SrrResetResponse::add(const SrrResponseParamsPtr & params)
        {
            SrrResetResponse * resetParams = dynamic_cast<SrrResetResponse*>(params.get());
            mapFeaturesStatus.insert(resetParams->mapFeaturesStatus.begin(), resetParams->mapFeaturesStatus.end());
        }

        void SrrResetResponse::serialize(cxxtools::SerializationInfo& si) const
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
       

        //get feature list response
        Status SrrListFeatureResponse::getGlobalStatus() const
        {
            return status;
        }

        void SrrListFeatureResponse::add(const SrrResponseParamsPtr & params)
        {
            SrrListFeatureResponse * pParams = dynamic_cast<SrrListFeatureResponse*>(params.get());
            mapFeaturesDependencies.insert(pParams->mapFeaturesDependencies.begin(), pParams->mapFeaturesDependencies.end());
        }

        void SrrListFeatureResponse::serialize(cxxtools::SerializationInfo& si) const
        {
            si.addMember("status") <<= statusToString(getGlobalStatus());
            cxxtools::SerializationInfo & featuresSi = si.addMember("featuresList");
            
            for( const auto & item : mapFeaturesDependencies)
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");

                featureSi.addMember("name") <<= item.first;
                featureSi.addMember("dependencies") <<= item.second;
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }


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
        }*/

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
            Query query1 = createSaveQuery({"test"},"myPassphrase");
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createSaveQuery({"test"},"myPassphrase");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createSaveQuery({"testgg"},"hsGH<hkherjg");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
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
            Feature f1;
            f1.set_version("1.0");
            f1.set_data("data");
            
            Query query1 = createRestoreQuery({{"test", f1}},"myPassphrase");
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createRestoreQuery({{"test", f1}},"myPassphrase");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Feature f2;
            f2.set_version("1.0");
            f2.set_data("data2");
            Query query3 = createRestoreQuery({{"test-1", f2}},"hsGH<hkherjg");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
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
            Query query1 = createResetQuery({"test"});
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createResetQuery({"test"});
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createResetQuery({"testgg"});
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> unserialize
            UserData userdata;
            userdata << query1;

            Query query4;
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
            Query query1 = createListFeatureQuery();
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createListFeatureQuery();
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createResetQuery({"testgg"});
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
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
/*    testNumber = "2.1";
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

//Next test
    testNumber = "2.3";
    testName = "Check Reset Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            FeatureStatus s1 = {Status::SUCCESS, ""};
            
            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = s1;
            
            SrrResponse r1 = SrrResponse::createReset(map1);
            std::cout << r1 << std::endl;

            //test ==
            SrrResponse r2 = SrrResponse::createReset(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = s1;
            
            SrrResponse r3 = SrrResponse::createReset(map2);
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
    testNumber = "2.4";
    testName = "Check List Feature Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
           
            SrrResponse r1 = SrrResponse::createGetListFeature({{"test", {"A","B"}}});
            std::cout << r1 << std::endl;

            //test ==
            SrrResponse r2 = SrrResponse::createGetListFeature({{"test", {"A","B"}}});
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            SrrResponse r3 = SrrResponse::createGetListFeature({{"test", {"C","B"}}});
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
    testNumber = "3.1";
    testName = "Check add operation on Save Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            FeatureStatus s1 = {Status::SUCCESS, ""};
            Feature f1 = {"1.0", "data"};
            
            std::map<FeatureName, std::pair<FeatureStatus,Feature>> map1;
            map1["test"] = {s1,f1};

            std::map<FeatureName, std::pair<FeatureStatus,Feature>> map2;
            map2["test2"] = {s1,f1};
            
            SrrResponse r1 = SrrResponse::createSave(map1);
            SrrResponse r2 = SrrResponse::createSave(map2);
            
            SrrResponse r3 = r1 + r2;
            std::cout << r3 << std::endl;

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
    testNumber = "3.2";
    testName = "Check add operation on Restore Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = {Status::SUCCESS, ""};
            
            SrrResponse r1 = SrrResponse::createRestore(map1);
            
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = {Status::FAILED, "I'm a failure"};
            
            SrrResponse r2 = SrrResponse::createRestore(map2);

            SrrResponse r3 = r1 + r2;
            std::cout << r3 << std::endl;

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
    testNumber = "3.3";
    testName = "Check add operation on Reset Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {          
            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = {Status::SUCCESS, ""};

            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = {Status::FAILED, "I'm a failure"};
            
            SrrResponse r1 = SrrResponse::createReset(map1);
            SrrResponse r2 = SrrResponse::createReset(map2);

            SrrResponse r3 = r1 + r2;
            std::cout << r3 << std::endl;

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
    testNumber = "3.4";
    testName = "Check add operation on List Feature Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {          
            SrrResponse r1 = SrrResponse::createGetListFeature({{"test", {"A","B"}}});
            SrrResponse r2 = SrrResponse::createGetListFeature({{"test1", {"C","B"}}});

            SrrResponse r3 = r1 + r2;
            std::cout << r3 << std::endl;

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
*/
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
