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

#ifndef FTY_SRR_DTO_H_INCLUDED
#define FTY_SRR_DTO_H_INCLUDED

#include "fty_userdata_dto.h"

#include <memory>
#include <map>
#include <set>
#include <iostream>

#include <cxxtools/serializationinfo.h>

namespace dto 
{
    namespace srr 
    {
        /**
         *  Actions for query object
         */
        enum class Action { GET_FEATURE_LIST, SAVE, RESET, RESTORE, UNKNOWN};
        std::string actionToString(Action action);
        Action stringToAction(const std::string & actionStr);

        /**
         *  Status for SRR response object
         */
        enum class Status { SUCCESS, FAILED, PARTIAL_SUCCESS, UNKNOWN};
        std::string statusToString(Status status);
        Status stringToStatus(const std::string & statusStr);

        /**
         * Feature structures
         * 
         */
        using FeatureName = std::string;
        using FeatureDependencies = std::set<FeatureName>;

        class Feature
        {
        public:
            std::string version;
            std::string data;  
        };
        //For testing mostly
        inline bool operator==(const Feature& lhs, const Feature& rhs){ return ((lhs.data == rhs.data) &&(lhs.version == rhs.version)); }
        inline bool operator!=(const Feature& lhs, const Feature& rhs){ return !(lhs == rhs); }


        class FeatureStatus
        {
        public:
            Status status;
            std::string errorMsg;
        };
        //For testing mostly
        inline bool operator==(const FeatureStatus& lhs, const FeatureStatus& rhs){ return ((lhs.status == rhs.status) &&(lhs.errorMsg == rhs.errorMsg)); }
        inline bool operator!=(const FeatureStatus& lhs, const FeatureStatus& rhs){ return !(lhs == rhs); }
        
        /**
         * Query Object
         * 
         */
        class SrrQueryParams;
        using SrrQueryParamsPtr = std::unique_ptr<SrrQueryParams>;

        class SrrQuery
        {
        private:
            SrrQueryParamsPtr m_params;
        public:
            SrrQuery() = default;

            Action getAction() const;
            SrrQueryParamsPtr & getParams() {return m_params;}
            const SrrQueryParamsPtr & getParams() const {return m_params;}

            void fromUserData(UserData & data);
            void toUserData(UserData & data) const;
            bool isEqual(const SrrQuery & query) const;

            //create functions for query
            static SrrQuery createSave(const std::set<FeatureName> & features, const std::string & passpharse);
            static SrrQuery createRestore(const std::map<FeatureName, Feature> & restoreData, const std::string & passpharse);
            static SrrQuery createReset(const std::set<FeatureName> & features);
            static SrrQuery createGetListFeature();

        };
        void operator>> (UserData & data, SrrQuery & query);
        void operator<< (UserData & data, const SrrQuery & query);
        inline bool operator==(const SrrQuery& lhs, const SrrQuery& rhs){ return lhs.isEqual(rhs); }
        inline bool operator!=(const SrrQuery& lhs, const SrrQuery& rhs){ return !(lhs == rhs); }
        std::ostream& operator<< (std::ostream& os, const SrrQuery& q);

        class SrrQueryParams
        {
        public:
            virtual ~SrrQueryParams(){};
            virtual bool isEqual(const SrrQueryParamsPtr & params) const = 0;
            virtual Action getAction() const = 0;

            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) = 0;
            virtual void serialize(cxxtools::SerializationInfo& si) const = 0;
        };

        class SrrSaveQuery : public SrrQueryParams
        {
        public:
            std::set<FeatureName> features;
            std::string passphrase;

            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
            virtual bool isEqual(const SrrQueryParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::SAVE; }
        };

        class SrrRestoreQuery : public SrrQueryParams
        {
        public:
            std::map<FeatureName, Feature> mapFeaturesData;
            std::string passphrase;

            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
            virtual bool isEqual(const SrrQueryParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::RESTORE; }
        };

        class SrrResetQuery : public SrrQueryParams
        {
        public:
            std::set<FeatureName> features;

            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
            virtual bool isEqual(const SrrQueryParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::RESET; }
        };

        class SrrListFeatureQuery : public SrrQueryParams
        {
        public:
            virtual void deserialize(const cxxtools::SerializationInfo& si) override {}
            virtual void serialize(cxxtools::SerializationInfo& si) const override {}
            virtual bool isEqual(const SrrQueryParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::GET_FEATURE_LIST; }
        };


        /**
         * Response object
         * 
         */
        class SrrResponseParams;
        using SrrResponseParamsPtr = std::unique_ptr<SrrResponseParams>;

        class SrrResponse
        {
        private:
            SrrResponseParamsPtr m_params;
        public:
            SrrResponse() = default;

            Action getAction() const;
            bool isEqual(const SrrResponse & response) const;

            SrrResponseParamsPtr & getParams() {return m_params;}
            const SrrResponseParamsPtr & getParams() const {return m_params;}

            void fromUserData(UserData & data);
            void toUserData(UserData & data) const;

            //create functions
            static SrrResponse createSave(const std::map<FeatureName, std::pair<FeatureStatus,Feature>> & mapFeaturesData);
            static SrrResponse createRestore(const std::map<FeatureName, FeatureStatus> & mapStatus);
            static SrrResponse createReset(const std::map<FeatureName, FeatureStatus> & mapStatus);
            static SrrResponse createGetListFeature(const std::map<FeatureName, FeatureDependencies> & mapFeaturesDependencies);
        };

        void operator>> (UserData & data, SrrResponse & response);
        void operator<< (UserData & data, const SrrResponse & response);
        inline bool operator==(const SrrResponse& lhs, const SrrResponse& rhs){ return lhs.isEqual(rhs); }
        inline bool operator!=(const SrrResponse& lhs, const SrrResponse& rhs){ return !(lhs == rhs); }
        std::ostream& operator<< (std::ostream& os, const SrrResponse& r);

        class SrrResponseParams
        {
        public:
            virtual ~SrrResponseParams(){};

            virtual bool isEqual(const SrrResponseParamsPtr & params) const = 0;
            virtual Action getAction() const = 0;
            virtual Status getGlobalStatus() const = 0;

            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) = 0;
            virtual void serialize(cxxtools::SerializationInfo& si) const = 0;
        };

        class SrrSaveResponse : public SrrResponseParams
        {
        public:
            std::map<FeatureName, std::pair<FeatureStatus,Feature>> mapFeaturesData;

            virtual bool isEqual(const SrrResponseParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::SAVE; }
            virtual Status getGlobalStatus() const override;

            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
            
        };

        class SrrRestoreResponse : public SrrResponseParams
        {
        public:
            std::map<FeatureName, FeatureStatus> mapFeaturesStatus;

            virtual bool isEqual(const SrrResponseParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::RESTORE; }
            virtual Status getGlobalStatus() const override;


            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
        };

        class SrrResetResponse : public SrrResponseParams
        {
        public:
            std::map<FeatureName, FeatureStatus> mapFeaturesStatus;

            virtual bool isEqual(const SrrResponseParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::RESET; }
            virtual Status getGlobalStatus() const override;

            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
        };

        class SrrListFeatureResponse : public SrrResponseParams
        {
        public:
            Status status = Status::SUCCESS;
            std::map<FeatureName, FeatureDependencies> mapFeaturesDependencies;

            virtual bool isEqual(const SrrResponseParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::GET_FEATURE_LIST; }
            virtual Status getGlobalStatus() const override;

            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
        };  
    } // srr namespace
    
} // dto namespace

#ifdef FTY_COMMON_DTO_BUILD_DRAFT_API
    // Tests
    void fty_srr_dto_test (bool verbose);

#endif // FTY_COMMON_DTO_BUILD_DRAFT_API

#endif
