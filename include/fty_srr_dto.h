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
        using Feature = std::string;

        class SrrQueryParams;
        using SrrQueryParamsPtr = std::unique_ptr<SrrQueryParams>;

        /**
         *  Actions for query object
         */
        enum class Action { GET_FEATURE_LIST, SAVE, RESET, RESTORE, UNKNOWN};
        std::string actionToString(Action action);
        Action stringToAction(const std::string & actionStr);

        /**
         * Query Object
         * 
         */

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
            static SrrQuery createSave(const std::set<Feature> & features, const std::string & passpharse);
            static SrrQuery createRestore(const std::map<Feature, std::string> & restoreData, const std::string & passpharse);
            static SrrQuery createReset(const std::set<Feature> & features);
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
            virtual bool isEqual(const SrrQueryParamsPtr & params) const = 0;
            virtual Action getAction() const = 0;

            //serialization
            virtual void deserialize(const cxxtools::SerializationInfo& si) = 0;
            virtual void serialize(cxxtools::SerializationInfo& si) const = 0;
        };

        class SrrSaveQuery : public SrrQueryParams
        {
        public:
            std::set<Feature> features;
            std::string passphrase;

            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
            virtual bool isEqual(const SrrQueryParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::SAVE; }
        };

        class SrrRestoreQuery : public SrrQueryParams
        {
        public:
            std::map<Feature, std::string> mapFeaturesData;
            std::string passphrase;

            virtual void deserialize(const cxxtools::SerializationInfo& si) override;
            virtual void serialize(cxxtools::SerializationInfo& si) const override;
            virtual bool isEqual(const SrrQueryParamsPtr & params) const override;
            virtual Action getAction() const override { return Action::RESTORE; }
        };

        class SrrResetQuery : public SrrQueryParams
        {
        public:
            std::set<Feature> features;

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

        /**
         *  Status for SRR response object
         */
        enum class Status { SUCCESS, FAILED, PARTIAL_SUCCESS, UNKNOWN};
        std::string statusToString(Status status);
        Status stringToStatus(const std::string & statusStr);

        class SrrResponseParams;
        using SrrResponseParamsPtr = std::unique_ptr<SrrResponseParams>;

        class SrrResponse
        {
        public:
            Action action = Action::UNKNOWN;
            Status status = Status::UNKNOWN;

            std::string error;

            SrrResponseParamsPtr m_params;

            SrrResponse() = default;

            void fromUserData(UserData & data);
            void toUserData(UserData & data) const;

            //create functions
            static SrrResponse createSave();
            static SrrResponse createRestore();
            static SrrResponse createReset();
            static SrrResponse createGetListFeature();
        };

        void operator>> (UserData & data, SrrResponse & response);
        void operator<< (UserData & data, const SrrResponse & response);

        class SrrResponseParams
        {
        public:
            virtual void fromUserData(UserData & data) = 0;
            virtual void toUserData(UserData & data) const = 0;
        };

        class SrrSaveResponse : public SrrResponseParams
        {
        public:
            std::map<Feature, std::string> mapFeaturesData;

            virtual void fromUserData(UserData & data) override;
            virtual void toUserData(UserData & data) const override;
        };

        class SrrRestoreResponse : public SrrResponseParams
        {
        public:
            std::map<Feature, Status> mapFeaturesStatus;

            virtual void fromUserData(UserData & data) override;
            virtual void toUserData(UserData & data) const override;
        };

        class SrrResetResponse : public SrrResponseParams
        {
        public:
            std::map<Feature, Status> mapFeaturesStatus;

            virtual void fromUserData(UserData & data) override;
            virtual void toUserData(UserData & data) const override;
        };

        class SrrListFeatureResponse : public SrrResponseParams
        {
        public:
            std::vector<Feature> features;
            virtual void fromUserData(UserData & data) override;
            virtual void toUserData(UserData & data) const override;
        };   
    } // srr namespace
    
} // dto namespace

#ifdef FTY_COMMON_DTO_BUILD_DRAFT_API
    // Tests
    void fty_srr_dto_test (bool verbose);

#endif // FTY_COMMON_DTO_BUILD_DRAFT_API

#endif
