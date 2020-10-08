/*  =========================================================================
    fty_srr_dto - Fty srr dto

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
#include <list>

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

        cxxtools::SerializationInfo deserializeJson(const std::string & json)
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

        std::string serializeJson(const cxxtools::SerializationInfo & si, bool beautify)
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

        Query createRestoreQuery(const std::map<FeatureName, Feature> & restoreData, const std::string & passpharse, const std::string & version, const std::string & checksum)
        {
            Query query;

            RestoreQuery & restoreQuery = *(query.mutable_restore());
            *(restoreQuery.mutable_map_features_data()) = {restoreData.begin(), restoreData.end()};
            restoreQuery.set_passpharse(passpharse);
            restoreQuery.set_version(version);
            restoreQuery.set_checksum(checksum);

            return query;
        }
        
        Query createRestoreListQuery(const std::list<std::map<FeatureName, Feature>>& restoreData, const std::string & passpharse, const std::string & version)
        {
            Query query;
            RestoreQuery& restoreQuery = *(query.mutable_restore());
            restoreQuery.set_passpharse(passpharse);
            restoreQuery.set_version(version);
            return query;
        }
        
        Query createResetQuery(const std::set<FeatureName> & features, const std::string & version)
        {
            Query query;

            ResetQuery & resetQuery = *(query.mutable_reset());
            *(resetQuery.mutable_features()) = {features.begin(), features.end()};
            resetQuery.set_version(version);
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

            std::string type = data.front();
            data.pop_front();

            if(type == SAVE_TYPE)
                query = saveQueryFromUiJson(payload);
            else if(type == RESTORE_TYPE)
                query = restoreQueryFromUiJson(payload);
            else if(type == RESET_TYPE)
                query = resetQueryFromUiJson(payload);
            else if(type == LIST_TYPE)
                query = createListFeatureQuery();
            else  
                throw std::runtime_error("Wrong query type");

            /*if(!query.ParseFromString(payload))
            {
                throw std::runtime_error("Impossible to deserialize");
            }*/
        }

        void operator<< (UserData & data, const Query & query)
        {
            cxxtools::SerializationInfo si;
            std::string type;

            switch (query.parameters_case())
            {
            case Query::ParametersCase::kSave :
                si <<= query.save();
                type = SAVE_TYPE;
                break;

            case Query::ParametersCase::kRestore :
                si <<= query.restore();
                type = RESTORE_TYPE;
                break;
            
            case Query::ParametersCase::kReset :
                si <<= query.reset();
                type = RESET_TYPE;
                break;

            case Query::ParametersCase::kListFeature :
                type = LIST_TYPE;
                break;
            
            default:
                break;
            }
            /*if(!query.SerializeToString(&payload))
            {
                throw std::runtime_error("Impossible to serialize");
            }*/
            data.push_back(serializeJson(si));
            data.push_back(type);
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
            si.getMember(SRR_VERSION) >>= *(query.mutable_version());
            si.getMember(CHECKSUM) >>= *(query.mutable_checksum());

            google::protobuf::Map<std::string, Feature>& mapFeaturesData = *(query.mutable_map_features_data());

            cxxtools::SerializationInfo featuresSi = si.getMember(DATA);
            cxxtools::SerializationInfo::Iterator it;
            for (it = featuresSi.begin(); it != featuresSi.end(); ++it)
            {
                const cxxtools::SerializationInfo siTemp = (cxxtools::SerializationInfo)*it;
                for (const auto &featureSi : siTemp)
                {
                    std::string featureName = featureSi.name();
                    
                    Feature f;

                    featureSi >>= f;

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

        void operator<<= (cxxtools::SerializationInfo& si, const SaveQuery & query)
        {
            si.addMember(PASS_PHRASE) <<= query.passpharse();

            cxxtools::SerializationInfo & featuresSi = si.addMember(FEATURE_LIST);

            for( const auto & name : query.features() )
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");
                featureSi.addMember(FEATURE_NAME) <<= name;
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const RestoreQuery & query)
        {
            si.addMember(PASS_PHRASE) <<= query.passpharse();
            si.addMember(SRR_VERSION) <<= query.version();
            si.addMember(CHECKSUM) <<= query.checksum();
            cxxtools::SerializationInfo & featuresSi = si.addMember(DATA);
            
            for( const auto & item : query.map_features_data())
            {
                cxxtools::SerializationInfo & entrySi = featuresSi.addMember("");
                cxxtools::SerializationInfo & featureSi = entrySi.addMember(item.first);
                
                featureSi <<= item.second;
                
            }
            
            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const ResetQuery & query)        
        {
            
            cxxtools::SerializationInfo & featuresSi = si.addMember(FEATURE_LIST);

            for( const auto & name : query.features() )
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");
                featureSi.addMember(FEATURE_NAME) <<= name;
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);    
        }

        void operator>>= (const cxxtools::SerializationInfo& si, Feature & feature)
        {
            si.getMember(SRR_VERSION) >>= *(feature.mutable_version());
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

            feature.set_data(data);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const Feature & feature)
        {
            si.addMember(SRR_VERSION) <<= feature.version();

            cxxtools::SerializationInfo & data = si.addMember(DATA);
            
            try
            {
                //try to unserialize the data if they are on Json format
                cxxtools::SerializationInfo dataSi = deserializeJson(feature.data());

                if(dataSi.category() == cxxtools::SerializationInfo::Category::Void || dataSi.category() == cxxtools::SerializationInfo::Category::Value || dataSi.isNull())
                {
                    data <<= feature.data();
                }
                else
                {
                    dataSi.setName(DATA);
                    data = dataSi;
                    data.setName(DATA);
                    data.setCategory(cxxtools::SerializationInfo::Category::Object);
                }
                
            }
            catch(const std::exception& /* e */)
            {
                //put the data as a string if they are not in Json
                data <<= feature.data();
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

        static Status stringToStatus(const std::string & statusStr);
        
        Response createSaveResponse(const std::map<FeatureName, FeatureAndStatus> & mapFeaturesData, const std::string & version)
        {
            FeatureStatus featureStatus;
            featureStatus.set_status(Status::SUCCESS);
            return createSaveResponse(mapFeaturesData, version, "", featureStatus);
        }
        
        Response createSaveResponse(const std::string & version, const FeatureStatus & featureStatus)
        {
            return createSaveResponse({}, version, "", featureStatus);
        }
        
        Response createSaveResponse(const std::map<FeatureName, FeatureAndStatus> & mapFeaturesData, const std::string & version, const std::string & checksum, const FeatureStatus & featureStatus)
        {
            Response response;

            SaveResponse & saveResponse = *(response.mutable_save());
            *(saveResponse.mutable_map_features_data()) = {mapFeaturesData.begin(), mapFeaturesData.end()};
            saveResponse.set_version(version);
            saveResponse.set_checksum(checksum);
            *(saveResponse.mutable_status()) = featureStatus;
            return response;
        }

        Response createRestoreResponse(const std::map<FeatureName, FeatureStatus> & mapStatus)
        {
            FeatureStatus featureStatus;
            featureStatus.set_status(Status::SUCCESS);
            return createRestoreResponse(featureStatus, mapStatus);
        }
        
        Response createRestoreResponse(const FeatureStatus & featureStatus, const std::map<FeatureName, FeatureStatus> & mapStatus)
        {
            Response response;
            RestoreResponse & restoreResponse = *(response.mutable_restore());
            *(restoreResponse.mutable_map_features_status()) = {mapStatus.begin(), mapStatus.end()};
            *(restoreResponse.mutable_status()) = featureStatus;
            return response;
        }
        
        Response createRestoreResponse(const FeatureStatus & featureStatus)
        {
            return createRestoreResponse(featureStatus, {});
        }

        Response createResetResponse(const std::map<FeatureName, FeatureStatus> & mapStatus)
        {
            Response response;

            ResetResponse & resetResponse = *(response.mutable_reset());
            *(resetResponse.mutable_map_features_status()) = {mapStatus.begin(), mapStatus.end()};

            return response;
        }

        Response createListFeatureResponse(const std::map<FeatureName, FeatureDependencies> & mapFeaturesDependencies, const std::string & version, const std::string & passphrassDefinition)
        {
            return createListFeatureResponse(mapFeaturesDependencies, version, passphrassDefinition, "");
        }
                
        Response createListFeatureResponse(const std::map<FeatureName, FeatureDependencies> & mapFeaturesDependencies, const std::string & version, const std::string & passphrassDefinition, const std::string & passphrassDescription)
        {
            Response response;

            ListFeatureResponse & listResponse = *(response.mutable_list_feature());
            *(listResponse.mutable_map_features_dependencies()) = {mapFeaturesDependencies.begin(), mapFeaturesDependencies.end()};
            listResponse.set_version(version);
            listResponse.set_passphrass_definition(passphrassDefinition);
            listResponse.set_passphrass_description(passphrassDescription);
            return response;
        }

        //userdata serializer / deserializer
        void operator>> (UserData & data, Response & response)
        {
            std::string payload = data.front();
            data.pop_front();

            std::string type = data.front();
            data.pop_front();
            
            cxxtools::SerializationInfo si = deserializeJson(payload);

            if(type == SAVE_TYPE)
                si >>= *(response.mutable_save());
            else if(type == RESTORE_TYPE)
                si >>= *(response.mutable_restore());
            else if(type == RESET_TYPE)
                si >>= *(response.mutable_reset());
            else if(type == LIST_TYPE)
                si >>= *(response.mutable_list_feature());
            else  
                throw std::runtime_error("Wrong query type");

            /*if(!response.ParseFromString(payload))
            {
                throw std::runtime_error("Impossible to deserialize");
            }*/
        }

        void operator<< (UserData & data, const Response & response)
        {
            cxxtools::SerializationInfo si;
            std::string type;

            switch (response.parameters_case())
            {
            case Response::ParametersCase::kSave :
                si <<= response.save();
                type = SAVE_TYPE;
                break;

            case Response::ParametersCase::kRestore :
                si <<= response.restore();
                type = RESTORE_TYPE;
                break;
            
            case Response::ParametersCase::kReset :
                si <<= response.reset();
                type = RESET_TYPE;
                break;

            case Response::ParametersCase::kListFeature :
                si <<= response.list_feature();
                type = LIST_TYPE;
                break;
            
            default:
                break;
            }
            /*if(!response.SerializeToString(&payload))
            {
                throw std::runtime_error("Impossible to serialize");
            }*/
            data.push_back(serializeJson(si));
            data.push_back(type);
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
            si.addMember(SRR_VERSION) <<= response.version();
            si.addMember(CHECKSUM) <<= response.checksum();
            si.addMember(STATUS) <<= statusToString(response.status().status());
            si.addMember(ERROR) <<= response.status().error();
            
            cxxtools::SerializationInfo & featuresSi = si.addMember(DATA);
            
            for( const auto & item : response.map_features_data())
            {
                cxxtools::SerializationInfo & entrySi = featuresSi.addMember("");
                cxxtools::SerializationInfo & featureSi = entrySi.addMember(item.first);
                const auto & featureAndStatus = item.second;
                
                featureSi.addMember(SRR_VERSION) <<= featureAndStatus.feature().version();
                featureSi.addMember(STATUS) <<= statusToString(featureAndStatus.status().status());
                featureSi.addMember(ERROR) <<= featureAndStatus.status().error();

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
                        data.setCategory(cxxtools::SerializationInfo::Category::Object);
                    }
                    
                }
                catch(const std::exception& /* e */)
                {
                    //put the data as a string if they are not in Json
                    data <<= featureAndStatus.feature().data();
                }

                data.setName(DATA);
                
            }
            
            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator<<= (cxxtools::SerializationInfo& si, const RestoreResponse & response)
        {
            
            if (response.map_features_status().empty())
            {
                si.addMember(STATUS) <<= statusToString(response.status().status());
            }
            else
            {
                si.addMember(STATUS) <<= statusToString(getGlobalStatus(response));
            }
            si.addMember(ERROR) <<= response.status().error();
            
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
            si.addMember(SRR_VERSION) <<= response.version();
            si.addMember(PASS_PHRASE_DEFINITION) <<= response.passphrass_definition();
            si.addMember(PASS_PHRASE_DESCRIPTION) <<= response.passphrass_description();
            
            cxxtools::SerializationInfo & featuresSi = si.addMember(FEATURE_LIST);
            for( const auto & item : response.map_features_dependencies())
            {
                cxxtools::SerializationInfo & featureSi = featuresSi.addMember("");

                featureSi.addMember(FEATURE_NAME) <<= item.first;
                const FeatureDependencies & dep = item.second;
                featureSi.addMember(FEATURE_DESCRIPTION) <<= dep.description();

                std::set<FeatureName> dependencies(dep.dependencies().begin(), dep.dependencies().end());

                featureSi.addMember(DEPENDENCIES) <<= dependencies;
            }

            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
        }

        void operator>>= (const cxxtools::SerializationInfo& si, SaveResponse & response)
        {
            google::protobuf::Map<std::string, FeatureAndStatus> & mapFeaturesData = *(response.mutable_map_features_data());
            
            if (si.findMember(SRR_VERSION) != NULL) {
                si.getMember(SRR_VERSION) >>= *(response.mutable_version());
            }
            
            if (si.findMember(CHECKSUM) != NULL) {
                si.getMember(CHECKSUM) >>= *(response.mutable_checksum());
            }
            
            if (si.findMember(STATUS) != NULL) {
                std::string statusStr;    
                si.getMember(STATUS) >>= statusStr;
                stringToStatus(statusStr);
                response.mutable_status()->set_status(stringToStatus(statusStr));
            }
            
            if (si.findMember(ERROR) != NULL) {
                std::string error;
                si.getMember(ERROR) >>= error;
                response.mutable_status()->set_error(error);
            }
            
            cxxtools::SerializationInfo featuresSi = si.getMember(DATA);
            
            cxxtools::SerializationInfo::Iterator it;
            for (it = featuresSi.begin(); it != featuresSi.end(); ++it)
            {
                const cxxtools::SerializationInfo siTemp = (cxxtools::SerializationInfo)*it;
                for (const auto &si : siTemp)
                {
                    std::string featureName = si.name();
                    
                    FeatureAndStatus fs;
                    Feature & f = *(fs.mutable_feature());

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
                    
                    FeatureStatus & s = *(fs.mutable_status());
                    
                    std::string statusStr, error;
                    
                    si.getMember(STATUS) >>= statusStr;
                    si.getMember(ERROR) >>= error;
                    
                    s.set_status(stringToStatus(statusStr));
                    s.set_error(error);

                    mapFeaturesData[featureName] = std::move(fs);
                }
            }
        }

        void operator>>= (const cxxtools::SerializationInfo& si, RestoreResponse & response)
        {
            google::protobuf::Map<std::string, FeatureStatus>& mapFeaturesData = *(response.mutable_map_features_status());
            
            if (si.findMember(STATUS) != NULL) {
                std::string statusStr;    
                si.getMember(STATUS) >>= statusStr;
                stringToStatus(statusStr);
                response.mutable_status()->set_status(stringToStatus(statusStr));
            }
            
            if (si.findMember(ERROR) != NULL) {
                std::string error;
                si.getMember(ERROR) >>= error;
                response.mutable_status()->set_error(error);
            }
            
            for (const auto & featureSi : si.getMember(STATUS_LIST) )
            {
                std::string name, statusStr, error;

                featureSi.getMember(FEATURE_NAME) >>= name;
                featureSi.getMember(STATUS) >>= statusStr;
                if (featureSi.findMember(ERROR) != NULL){
                    featureSi.getMember(ERROR) >>= error;
                }

                FeatureStatus f;
                f.set_status(stringToStatus(statusStr));
                f.set_error(error);

                mapFeaturesData[name] = std::move(f);
            }
            
        }

        void operator>>= (const cxxtools::SerializationInfo& si, ResetResponse & response)
        {
            google::protobuf::Map<std::string, FeatureStatus>& mapFeaturesData = *(response.mutable_map_features_status());
            
            for (const auto & featureSi : si.getMember(STATUS_LIST) )
            {
                std::string name, statusStr, error;

                featureSi.getMember(FEATURE_NAME) >>= name;
                featureSi.getMember(STATUS) >>= statusStr;
                featureSi.getMember(ERROR) >>= error;

                FeatureStatus f;
                f.set_status(stringToStatus(statusStr));
                f.set_error(error);

                mapFeaturesData[name] = std::move(f);
            }
        }

        void operator>>= (const cxxtools::SerializationInfo& si, ListFeatureResponse & response)
        {
            google::protobuf::Map<std::string, FeatureDependencies>& mapDependencies = *(response.mutable_map_features_dependencies());
            
            if (si.findMember(SRR_VERSION) != NULL) {
                si.getMember(SRR_VERSION) >>= *(response.mutable_version());
            }
            
            if (si.findMember(PASS_PHRASE_DEFINITION) != NULL) {
                si.getMember(PASS_PHRASE_DEFINITION) >>= *(response.mutable_passphrass_definition());
            }
            
            if (si.findMember(PASS_PHRASE_DESCRIPTION) != NULL) {
                si.getMember(PASS_PHRASE_DESCRIPTION) >>= *(response.mutable_passphrass_description());
            }
            
            if (si.findMember(FEATURE_LIST) != NULL) {
                for (const auto & featureSi : si.getMember(FEATURE_LIST) )
                {   
                    std::string name, description;
                    std::set<FeatureName> dependencies;

                    featureSi.getMember(FEATURE_NAME) >>= name;
                    featureSi.getMember(FEATURE_DESCRIPTION) >>= description;
                    featureSi.getMember(DEPENDENCIES) >>= dependencies;

                    FeatureDependencies deps;
                    deps.set_description(description);
                    for(const auto & dep : dependencies)
                    {
                        deps.add_dependencies(dep);
                    }

                    mapDependencies[name] = std::move(deps);
                }
            }
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
        
        static Status stringToStatus(const std::string & statusStr)
        {
            for(const auto & it : statusInString )
            {
                if(it.second == statusStr)
                    return it.first;
            }
            return Status::UNKNOWN;
        }
    } // srr
} // dto


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

    std::string defaultVersion = "1.0";
    std::string passPhraseDef = "8";
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
            f1.set_data("data 1");
            
            Query query1 = createRestoreQuery({{"test", f1}}, "myPassphrase", defaultVersion, "myChecksum");
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createRestoreQuery({{"test", f1}},"myPassphrase", defaultVersion, "myChecksum");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Feature f2;
            f2.set_version("1.0");
            f2.set_data("data 2");
            Query query3 = createRestoreQuery({{"test-1", f2}},"hsGH<hkherjg", defaultVersion, "myChecksum");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");
            
            Feature f3;
            f3.set_version("1.0");
            f3.set_data("{\"timeout\":\"40\"}");
            
            Query query5 = createRestoreQuery({{"test", f3}},"myPassphrase", defaultVersion, "myChecksum");
            std::cout << query5 << std::endl;
            
            UserData userdata1;
            userdata1 << query5;

            Query query6;
            userdata1 >> query6;
            
            if(query5 != query6) throw std::runtime_error("Bad serialization to userdata");


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
            Query query1 = createResetQuery({"test"}, defaultVersion);
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createResetQuery({"test"}, defaultVersion);
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createResetQuery({"testgg"}, defaultVersion);
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
            Query query3 = createResetQuery({"testgg"}, defaultVersion);
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
            
            Response r1 = createSaveResponse(map1, defaultVersion);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createSaveResponse(map1, defaultVersion);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureAndStatus> map2;
            map2["test2"] = fs1;
            
            Response r3 = createSaveResponse(map2, defaultVersion);
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
            s1.set_error("aaaa");
            
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
            
            Response r1 = createListFeatureResponse({{"test", d1}}, defaultVersion, passPhraseDef);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createListFeatureResponse({{"test", d1}}, defaultVersion, passPhraseDef);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            FeatureDependencies d2;
            d1.add_dependencies("C");
            d1.add_dependencies("B");
            
            Response r3 = createListFeatureResponse({{"test", d2}}, defaultVersion, passPhraseDef);
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
            
            Response r1 = createSaveResponse({{"test",fs1}}, defaultVersion);

            Feature f2;
            f2.set_version("3.5");
            f2.set_data("data-2");

            FeatureStatus s2; 
            s2.set_status(Status::FAILED);
            s2.set_error("bad version");
              
            FeatureAndStatus fs2;          
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;
            
            Response r2 = createSaveResponse({{"test-2",fs2}}, defaultVersion);
            
            Response r3 = createSaveResponse({{"test",fs1},{"test-2",fs2}}, defaultVersion);
            
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
          
            Response r1 = createListFeatureResponse({{"test", d1}}, defaultVersion, passPhraseDef);

            FeatureDependencies d2;
            d2.add_dependencies("A");
            d2.add_dependencies("B");

            Response r2 = createListFeatureResponse({{"test2", d2}}, defaultVersion, passPhraseDef);
            
            Response r3 = createListFeatureResponse({{"test", d1},{"test2", d2}}, defaultVersion, passPhraseDef);

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
            std::string restoreQueryJson =R"TEST({
                    "passphrase":"a","version":"1.0","checksum":"qDYTMxDem+",
                    "data":[{"automations":{"version":"1.0","data":{"automationList":[{"id":"etn_automation_id_1576571918088",
                    "name":"Test","createdBy":"admin","createdOn":"2019-12-17T08:38:38.088+0000","comments":"","active":false,
                    "timeout":36000,"notification":{"notifyOnFailure":false,"emails":[]}}]}}}]})TEST";
            Query queryRestore = restoreQueryFromUiJson (restoreQueryJson);
            std::cout << queryRestore << std::endl;
            
            dto::UserData reqData;
            reqData << queryRestore;
            std::cout << reqData.front() << std::endl;

            if(queryRestore.parameters_case() != Query::ParametersCase::kRestore)
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("Invalid query type");
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
    
    //Next test
    testNumber = "4.3";
    testName = "Deserialize restore query from automation";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {  
            std::string restoreQueryJson = R"TEST(
                   {"version": "1.0","passphrase": "my pass phrase","checksum":"qDYTMxDem+",
                    "data": [{"automation-settings": {"version": "1.0","status": "success","error": "",
                    "data": {"server": {"timeout": "10000","background": "0","workdir": ".","verbose": "0"}}}}, 
                    {"automations": {"version": "1.0","status": "success","error": "",
                    "data": {"automationList": [{"id": "etn_automation_id_1576571918088","name": 
                    "Test","createdBy": "admin","createdOn": "2019-12-17T08:38:38.088+0000","comments": "","active": false,"timeout": 36000,
                    "notification": {"notifyOnFailure": false,"emails": []},"schedule": "2019-12-17T08:37:00.000Z",
                    "triggerType": "CAT_SCHEDULE","triggers": {"ipmInfraEvent": []},"tasks": [{"index": 0,"name": "Wait 10 seconds",
                    "group": "WAIT","subgroup": "DELAY","properties": [{"key": "duration","value": ["10"]}],"timeout": 3600}]}]}}}]})TEST";

            Query queryRestore = restoreQueryFromUiJson (restoreQueryJson);
            std::cout << queryRestore << std::endl;
            
            if(queryRestore.parameters_case() != Query::ParametersCase::kRestore)
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("Invalid query type");
            }
            
            if(queryRestore.restore().map_features_data().at("automation-settings").data() != "{\"server\":{\"timeout\":\"10000\",\"background\":\"0\",\"workdir\":\".\",\"verbose\":\"0\"}}")
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("bad data object serialisation");
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
    testNumber = "4.4";
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

            
            Response r = createSaveResponse({{"object",fs1},{"no-object",fs2}}, defaultVersion);

            std::string strV1 = "{\"version\":\"1.0\",\"checksum\":\"\",\"status\":\"success\",\"error\":\"\",\"data\":[{\"no-object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":\"data in text\"}},{\"object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":{\"timeout\":\"40\"}}}]}";
            std::string strV2 = "{\"version\":\"1.0\",\"checksum\":\"\",\"status\":\"success\",\"error\":\"\",\"data\":[{\"object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":{\"timeout\":\"40\"}}},{\"no-object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":\"data in text\"}}]}";
            
            std::string responseInStr = responseToUiJson(r);
            std::cout << "responseToUiJson " << responseInStr << std::endl;
            
            if(responseInStr != strV1 && responseInStr != strV2)
            {
                std::cout << responseToUiJson(r) << std::endl;
                throw std::runtime_error("invalid response");
            }
            
            UserData userdata1;
            userdata1 << r;

            Response r1;
            userdata1 >> r1;
            std::string responseInStrAfterDeserilize = responseToUiJson(r1);
            std::cout << "responseInStrAfterDeserilize " << responseInStrAfterDeserilize << std::endl;
            
            if(r != r1) throw std::runtime_error("Bad serialization to userdata");
            
            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    
    //Next test
    testNumber = "4.5";
    testName = "Serialize FeatureStatus from EMC4J";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {  
            
            std::map<FeatureName, FeatureAndStatus> mapResponse;
            
            Feature f1;
            f1.set_version("1.0");
            
            FeatureStatus s1; 
            s1.set_status(Status::SUCCESS);
            
            FeatureAndStatus fs1;
            *(fs1.mutable_feature()) = f1;
            *(fs1.mutable_status()) = s1;

            Feature f2;
            f2.set_version("1.0");

            FeatureStatus s2; 
            s2.set_status(Status::SUCCESS);
              
            FeatureAndStatus fs2;          
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;
            
            mapResponse["automation"] = fs1;
            mapResponse["virtual-asset"] = fs2;

            Response r3 = createSaveResponse(mapResponse, defaultVersion);
            
            std::cout << r3 << std::endl;
            
            dto::UserData resp;
            resp.push_back("{\"statusList\":[{\"name\":\"automation\",\"status\":\"success\",\"error\":\"\"}]}");
            resp.push_back("restore");
            
            Response partialResp;
            resp >> partialResp;

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

