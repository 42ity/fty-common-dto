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

namespace dto 
{
    namespace srr 
    {

        /**
         * SRR request object
         */
        struct SrrQueryDto {
            std::string action;
            std::string data;

            SrrQueryDto() = default;
            SrrQueryDto(const std::string& action) : action(action) {}
            SrrQueryDto(const std::string& action, const std::string& data) : action(action), data(data) {}
        };

        void operator<<(UserData &data, const SrrQueryDto &object);
        void operator>>(UserData &inputData, SrrQueryDto &object);
        
        /**
         * FeatureDto object treated by SRR
         */
        struct SrrFeatureDto {
            std::string name;
            std::string dependencies;

            SrrFeatureDto() = default;
            SrrFeatureDto(const std::string name) : name(name) {}
            SrrFeatureDto(const std::string name, const std::string dependencies) : name(name), dependencies(dependencies) {}
        };
        
        void operator<<(UserData &data, const SrrFeatureDto &object);
        void operator>>(UserData &inputData, SrrFeatureDto &object);

        /**
         * All features list object treated by SRR
         */
        struct SrrFeaturesListDto {
            std::list<SrrFeatureDto> featuresList;

            SrrFeaturesListDto() = default;
            SrrFeaturesListDto(std::list<SrrFeatureDto> featuresList) : featuresList(featuresList) {}
        };

        void operator<<(UserData &data, const SrrFeaturesListDto &object);
        void operator>>(UserData &inputData, SrrFeaturesListDto &object);
        
        /**
         * List of SrrSaveDto object with a global status
         */
        struct SrrSaveDto {
            std::string status;
            std::string config;

            SrrSaveDto() = default;
            SrrSaveDto(const std::string& status) : status(status) {}
            SrrSaveDto(const std::string& status, const std::string& config) : status(status), config(config) {}
        };

        void operator<<(UserData &data, const SrrSaveDto &object);
        void operator>>(UserData &inputData, SrrSaveDto &object);

        /**
         * SRR restore object
         */
        struct SrrRestoreDto {
            std::string name;
            std::string status;
            std::string error;

            SrrRestoreDto() = default;
            SrrRestoreDto(const std::string& name) : name(name) {}
            SrrRestoreDto(const std::string& name, const std::string& status) : name(name), status(status) {}
            SrrRestoreDto(const std::string& name, const std::string& status, const std::string& error) : name(name), status(status), error(error) {}
        };

        void operator<<(UserData &data, const SrrRestoreDto &object);
        void operator>>(UserData &inputData, SrrRestoreDto &object);

        /**
         * List of SrrRestoreDto object with a global status
         */
        struct SrrRestoreDtoList {
            std::string status;
            std::list<SrrRestoreDto> responseList;

            SrrRestoreDtoList() = default;
            SrrRestoreDtoList(const std::string& status) : status(status) {}
            SrrRestoreDtoList(const std::string& status, const std::list<SrrRestoreDto> responseList) : status(status), responseList(responseList) {}
        };

        void operator<<(UserData &data, const SrrRestoreDtoList &object);
        void operator>>(UserData &inputData, SrrRestoreDtoList &object);
        
    } // srr namespace
    
} // dto namespace

#endif
