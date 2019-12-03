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

#include "fty_srr_dto.h"

#include <cxxtools/jsonserializer.h>
#include <cxxtools/jsondeserializer.h>

#include <google/protobuf/util/json_util.h>

#include <iostream>

namespace dto 
{
    namespace srr 
    {
        Response SrrQueryProcessor::processQuery(const Query & q)
        {
            Response response;

            switch (q.parameters_case())
            {
            case Query::ParametersCase::kSave :
                if(!saveHandler) throw std::runtime_error("No save handler!");
                *(response.mutable_save()) = saveHandler(q.save());
                break;

            case Query::ParametersCase::kRestore :
                if(!restoreHandler) throw std::runtime_error("No restore handler!");
                *(response.mutable_restore()) = restoreHandler(q.restore());
                break;
            
            case Query::ParametersCase::kReset :
                if(!resetHandler) throw std::runtime_error("No reset handler!");
                *(response.mutable_reset()) = resetHandler(q.reset());
                break;

            case Query::ParametersCase::kListFeature :
                if(!listFeatureHandler) throw std::runtime_error("No list feature handler!");
                *(response.mutable_list_feature()) = listFeatureHandler(q.list_feature());
                break;
            
            default:
                throw std::runtime_error("Unknown query!");
                break;
            }

            return response;
        }

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
        
        Query createRestoreListQuery(const std::list<std::map<FeatureName, Feature>>& restoreData, const std::string & passpharse)
        {
            Query query;
            RestoreQuery& restoreQuery = *(query.mutable_restore());
            
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
            options.always_print_primitive_fields=true;
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

            si >>= *(query.mutable_restore());

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
            si.getMember(PASS_PHRASE) >>= *(query.mutable_passpharse());

            const cxxtools::SerializationInfo & featuresSi = si.getMember(FEATURE_LIST);

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);
                featureSi.getMember(FEATURE_NAME) >>= *(query.add_features());
            }           
        }
        
        void operator>>= (const cxxtools::SerializationInfo& si, RestoreQuery & query)
        {
            si.getMember(PASS_PHRASE) >>= *(query.mutable_passpharse());

            google::protobuf::Map<std::string, Feature>& mapFeaturesData = *(query.mutable_map_features_data());

            cxxtools::SerializationInfo featuresSi = si.getMember(DATA);
            cxxtools::SerializationInfo::Iterator it;
            for (it = featuresSi.begin(); it != featuresSi.end(); ++it)
            {
                const cxxtools::SerializationInfo siTemp = (cxxtools::SerializationInfo)*it;
                for (const auto &si : siTemp)
                {
                    std::string featureName = si.name();
                    std::cout << featureName << std::endl;
                    
                    Feature f;
                    si.getMember(SRR_VERSION) >>= *(f.mutable_version());
                    cxxtools::SerializationInfo dataSi = si.getMember(DATA);

                    std::string data;
                    
                    if(dataSi.category() == cxxtools::SerializationInfo::Category::Value)
                    {
                        dataSi >>= data; 
                    }
                    else
                    {
                        dataSi.setName("");
                        data = serializeJson(dataSi);
                    }

                    f.set_data(data);

                    mapFeaturesData[featureName] = std::move(f);
                }
            }
        }

        void operator>>= (const cxxtools::SerializationInfo& si, ResetQuery & query)        
        {
            const cxxtools::SerializationInfo & featuresSi = si.getMember(FEATURE_LIST);

            for(size_t index = 0; index < featuresSi.memberCount(); index++ )
            {
                const cxxtools::SerializationInfo & featureSi = featuresSi.getMember(index);
                featureSi.getMember(FEATURE_NAME) >>= *(query.add_features());
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

        Response createListFeatureResponse(const std::map<FeatureName, FeatureDependencies> & mapFeaturesDependencies)
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

        Response operator+(const Response & r1, const Response & r2)
        {
            Response r = r1;
            r +=r2;
            return r;
        }

        Response& operator+=(Response & r1, const Response & r2)
        {
            if(r1.parameters_case() != r2.parameters_case())
            {
                throw std::runtime_error("Response with different type cannot be added");
            }

            switch (r1.parameters_case())
            {
            case Response::ParametersCase::kSave :
                *(r1.mutable_save()) += r2.save();
                break;

            case Response::ParametersCase::kRestore :
                *(r1.mutable_restore()) += r2.restore();
                break;
            
            case Response::ParametersCase::kReset :
                *(r1.mutable_reset()) += r2.reset();
                break;

            case Response::ParametersCase::kListFeature :
                *(r1.mutable_list_feature()) += r2.list_feature();
                break;
            
            default:
                break;
            }

            return r1;
        }

        SaveResponse& operator+=(SaveResponse & r1, const SaveResponse & r2)
        {
            r1.mutable_map_features_data()->insert(r2.map_features_data().begin(), r2.map_features_data().end());
            return r1;
        }

        RestoreResponse& operator+=(RestoreResponse & r1, const RestoreResponse & r2)
        {
            r1.mutable_map_features_status()->insert(r2.map_features_status().begin(), r2.map_features_status().end());
            return r1;
        }

        ResetResponse& operator+=(ResetResponse & r1, const ResetResponse & r2)
        {
            r1.mutable_map_features_status()->insert(r2.map_features_status().begin(), r2.map_features_status().end());
            return r1;
        }

        ListFeatureResponse& operator+=(ListFeatureResponse & r1, const ListFeatureResponse & r2)
        {
            r1.mutable_map_features_dependencies()->insert(r2.map_features_dependencies().begin(), r2.map_features_dependencies().end());
            return r1;
        }


        //get global status for UI => will be moved in fty-srr-rest
        Status getGlobalStatus(const SaveResponse & r)
        {
            Status globalStatus = Status::UNKNOWN;

            unsigned int nbSuccess = 0;
            for( const auto & item : r.map_features_data())
            {
                const FeatureAndStatus & feature = item.second;

                if(feature.status().status() == Status::SUCCESS)
                {
                    nbSuccess++;
                }
            }

            if(nbSuccess == 0)
            {
                globalStatus = Status::FAILED;
            }
            else if(nbSuccess == r.map_features_data().size())
            {
                globalStatus = Status::SUCCESS;
            }
            else
            {
                globalStatus = Status::PARTIAL_SUCCESS;
            }

            return globalStatus;
        }

        Status getGlobalStatus(const RestoreResponse & r)
        {
            Status globalStatus = Status::UNKNOWN;

            unsigned int nbSuccess = 0;
            for( const auto & item : r.map_features_status())
            {
                if(item.second.status() == Status::SUCCESS)
                {
                    nbSuccess++;
                }
            }

            if(nbSuccess == 0)
            {
                globalStatus = Status::FAILED;
            }
            else if(nbSuccess == r.map_features_status().size())
            {
                globalStatus = Status::SUCCESS;
            }
            else
            {
                globalStatus = Status::PARTIAL_SUCCESS;
            }

            return globalStatus;
        }

        Status getGlobalStatus(const ResetResponse & r)
        {
            Status globalStatus = Status::UNKNOWN;

            unsigned int nbSuccess = 0;
            for( const auto & item : r.map_features_status())
            {
                if(item.second.status() == Status::SUCCESS)
                {
                    nbSuccess++;
                }
            }

            if(nbSuccess == 0)
            {
                globalStatus = Status::FAILED;
            }
            else if(nbSuccess == r.map_features_status().size())
            {
                globalStatus = Status::SUCCESS;
            }
            else
            {
                globalStatus = Status::PARTIAL_SUCCESS;
            }

            return globalStatus;
        }

        Status getGlobalStatus(const ListFeatureResponse & r)
        {
            return Status::SUCCESS;
        }
        
        std::string responseToUiJson(const Response & response, bool beautiful)
        {
            cxxtools::SerializationInfo si; 
            si <<= response;
            return serializeJson(si, beautiful);
        }


        void operator<<= (cxxtools::SerializationInfo& si, const Response & response)
        {
            switch (response.parameters_case())
            {
            case Response::ParametersCase::kSave :
                si <<= response.save();
                break;

            case Response::ParametersCase::kRestore :
                si <<= response.restore();
                break;
            
            case Response::ParametersCase::kReset :
                si <<= response.reset();
                break;

            case Response::ParametersCase::kListFeature :
                si <<= response.list_feature();
                break;
            
            default:
                break;
            }
        }

        void operator<<= (cxxtools::SerializationInfo& si, const SaveResponse & response)
        {
            cxxtools::SerializationInfo & featuresSi = si.addMember(DATA);
            
            for( const auto & item : response.map_features_data())
            {
                cxxtools::SerializationInfo & entrySi = featuresSi.addMember("");
                cxxtools::SerializationInfo & featureSi = entrySi.addMember(item.first);
                const auto & featureAndStatus = item.second;
                featureSi.addMember(SRR_VERSION) <<= featureAndStatus.feature().version();

                cxxtools::SerializationInfo & data = featureSi.addMember(DATA);
                
                try
                {
                    //try to unserialize the data if they are on Json format
                    data = deserializeJson(featureAndStatus.feature().data());
                    
                    if(data.category() == cxxtools::SerializationInfo::Category::Void || data.category() == cxxtools::SerializationInfo::Category::Value)
                    {
                        data <<= featureAndStatus.feature().data();
                    }
                    else
                    {
                        data.setName(DATA);
                        data.setCategory(cxxtools::SerializationInfo::Category::Object);
                    }
                    
                }
                catch(const std::exception& /* e */)
                {
                    //put the data as a string if they are not in Json
                    data <<= featureAndStatus.feature().data();
                }
                
            }
            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const RestoreResponse & response)
        {
            si.addMember(STATUS) <<= statusToString(getGlobalStatus(response));
            cxxtools::SerializationInfo & featuresSi = si.addMember(STATUS_LIST);
            
            for( const auto & item : response.map_features_status())
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");

                featureSi.addMember(FEATURE_NAME) <<= item.first;
                featureSi.addMember(STATUS) <<= statusToString(item.second.status());
                featureSi.addMember(ERROR) <<= item.second.error();
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const ResetResponse & response)
        {
            si.addMember(STATUS) <<= statusToString(getGlobalStatus(response));
            cxxtools::SerializationInfo & featuresSi = si.addMember(STATUS_LIST);
            
            for( const auto & item : response.map_features_status())
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");

                featureSi.addMember(FEATURE_NAME) <<= item.first;
                featureSi.addMember(STATUS) <<= statusToString(item.second.status());
                featureSi.addMember(ERROR) <<= item.second.error();
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const ListFeatureResponse & response)
        {
            si.addMember(STATUS) <<= statusToString(getGlobalStatus(response));
            cxxtools::SerializationInfo & featuresSi = si.addMember(FEATURE_LIST);
            
            for( const auto & item : response.map_features_dependencies())
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");

                featureSi.addMember(FEATURE_NAME) <<= item.first;
                const FeatureDependencies & dep = item.second;

                std::set<FeatureName> dependencies(dep.dependencies().begin(), dep.dependencies().end());

                featureSi.addMember("dependencies") <<= dependencies;
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
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
                throw std::runtime_error("Error in the json: "+std::string(e.what()));
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
            f1.set_data(DATA);
            
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
    testNumber = "2.1";
    testName = "Check Save Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {
            FeatureAndStatus fs1;
            FeatureStatus & s1 = *(fs1.mutable_status());
            s1.set_status(Status::SUCCESS);

            Feature & f1 = *(fs1.mutable_feature());
            f1.set_version("1.0");
            f1.set_data(DATA);
            
            std::map<FeatureName, FeatureAndStatus> map1;
            map1["test"] = fs1;
            
            Response r1 = createSaveResponse(map1);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createSaveResponse(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureAndStatus> map2;
            map2["test2"] = fs1;
            
            Response r3 = createSaveResponse(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
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
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            
            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = s1;
            
            Response r1 = createRestoreResponse(map1);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createRestoreResponse(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = s1;
            
            Response r3 = createRestoreResponse(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
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
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            s1.set_error(ERROR);
            
            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = s1;
            
            Response r1 = createResetResponse(map1);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createResetResponse(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = s1;
            
            Response r3 = createResetResponse(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
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
            FeatureDependencies d1;
            d1.add_dependencies("A");
            d1.add_dependencies("B");
            
            Response r1 = createListFeatureResponse({{"test", d1}});
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createListFeatureResponse({{"test", d1}});
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            FeatureDependencies d2;
            d1.add_dependencies("C");
            d1.add_dependencies("B");
            
            Response r3 = createListFeatureResponse({{"test", d2}});
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
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
            Feature f1;
            f1.set_version("1.0");
            f1.set_data(DATA);
            
            FeatureStatus s1; 
            s1.set_status(Status::SUCCESS);
            
            FeatureAndStatus fs1;
            *(fs1.mutable_feature()) = f1;
            *(fs1.mutable_status()) = s1;
            
            Response r1 = createSaveResponse({{"test",fs1}});

            Feature f2;
            f2.set_version("3.5");
            f2.set_data("data-2");

            FeatureStatus s2; 
            s2.set_status(Status::FAILED);
            s2.set_error("bad version");
              
            FeatureAndStatus fs2;          
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;
            
            Response r2 = createSaveResponse({{"test-2",fs2}});
            
            Response r3 = createSaveResponse({{"test",fs1},{"test-2",fs2}});
            
            Response r = r1 + r2;
            std::cout << r << std::endl;
            
            if(r != r3) throw std::runtime_error("Bad aggregation");

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
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            Response r1 = createRestoreResponse({{"test", s1}});
 
            FeatureStatus s2;
            s2.set_status(Status::FAILED);
            s2.set_error("error msg");
            
            Response r2 = createRestoreResponse({{"test-2", s2}});
            
            Response r = createRestoreResponse({{"test", s1},{"test-2", s2}});

            Response r3 = r1 + r2;
            std::cout << r3 << std::endl;
            
            if(r != r3) throw std::runtime_error("Bad aggregation");

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
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            Response r1 = createResetResponse({{"test", s1}});
 
            FeatureStatus s2;
            s2.set_status(Status::FAILED);
            s2.set_error("error msg");
            
            Response r2 = createResetResponse({{"test-2", s2}});
            
            Response r = createResetResponse({{"test", s1},{"test-2", s2}});

            Response r3 = r1 + r2;
            std::cout << r3 << std::endl;
            
            if(r != r3) throw std::runtime_error("Bad aggregation");

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
            FeatureDependencies d1;
            d1.add_dependencies("A");
            d1.add_dependencies("B");
          
            Response r1 = createListFeatureResponse({{"test", d1}});

            FeatureDependencies d2;
            d2.add_dependencies("A");
            d2.add_dependencies("B");

            Response r2 = createListFeatureResponse({{"test2", d2}});
            
            Response r3 = createListFeatureResponse({{"test", d1},{"test2", d2}});

            Response r = r1 + r2;
            std::cout << r << std::endl;
            
            if(r != r3) throw std::runtime_error("Bad aggregation");
            
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
    testNumber = "4.1";
    testName = "Deserialize save query from UI";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {  
            std::string saveQueryJson = "{\"version\": \"1.0\",\"passphrase\": \"\",\"featuresList\": [{\"name\":\"etn-mass-management\"}]}";
            Query query = saveQueryFromUiJson (saveQueryJson);
            
            if(query.parameters_case() != Query::ParametersCase::kSave)
            {
                const cxxtools::SerializationInfo si = deserializeJson(saveQueryJson);
                std::cout << serializeJson(si, true) << std::endl;
                std::cout << query << std::endl;

                throw std::runtime_error("Invalid query type");
            }
            
            if(query.save().features(0) != "etn-mass-management")
            {
                const cxxtools::SerializationInfo si = deserializeJson(saveQueryJson);
                std::cout << serializeJson(si, true) << std::endl;
                std::cout << query << std::endl;
                throw std::runtime_error("bad deserialisation from UI");
            }


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
    testNumber = "4.2";
    testName = "Deserialize restore query from UI";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {  
            std::string restoreQueryJson = "{\"version\": \"1.0\",\"passphrase\": \"my pass phrase\",\"data\": [{\"object\": {\"version\": \"1.0\",\"data\": {\"timeout\": \"40\"}}},{\"no-object\": {\"version\": \"1.0\",\"data\": \"data in text\"}}]}";
            Query queryRestore = restoreQueryFromUiJson (restoreQueryJson);
            
            if(queryRestore.parameters_case() != Query::ParametersCase::kRestore)
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("Invalid query type");
            }
            
            if(queryRestore.restore().map_features_data().at("object").data() != "{\"timeout\":\"40\"}")
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("bad data object serialisation");
            }
            
            if(queryRestore.restore().map_features_data().at("no-object").data() != "data in text")
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("bad data for none object serialisation");
            }
            
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
    testNumber = "4.3";
    testName = "Serialize save response for UI";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());

        try
        {  
            Feature f1;
            f1.set_version("1.0");
            f1.set_data("{\"timeout\":\"40\"}");
            
            FeatureStatus s1; 
            s1.set_status(Status::SUCCESS);
            
            FeatureAndStatus fs1;
            *(fs1.mutable_feature()) = f1;
            *(fs1.mutable_status()) = s1;

            Feature f2;
            f2.set_version("1.0");
            f2.set_data("data in text");

            FeatureStatus s2; 
            s2.set_status(Status::SUCCESS);
              
            FeatureAndStatus fs2;          
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;

            
            Response r = createSaveResponse({{"object",fs1},{"no-object",fs2}});

            std::string strV1 = "{\"data\":[{\"no-object\":{\"version\":\"1.0\",\"data in text\"}},{\"object\":{\"version\":\"1.0\",\"data\":{\"timeout\":\"40\"}}}]}";
            std::string strV2 = "{\"data\":[{\"object\":{\"version\":\"1.0\",\"data\":{\"timeout\":\"40\"}}},{\"no-object\":{\"version\":\"1.0\",\"data in text\"}}]}";
            std::string responseInStr = responseToUiJson(r);
            
            if(responseInStr != strV1 && responseInStr != strV2)
            {
                std::cout << responseToUiJson(r) << std::endl;
                throw std::runtime_error("invalid response");
            }
            
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
    
    google::protobuf::ShutdownProtobufLibrary();
}

#endif //FTY_COMMON_DTO_BUILD_DRAFT_API
