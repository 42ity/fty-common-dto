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

#include <string>
#include <list>

#include <fty_common_messagebus.h>

namespace dto 
{
    namespace srr 
    {

        // ========================= SRR ============================
        //
        // ==========================================================

        struct SrrQueryDto {
            std::string action;
            std::string data;

            SrrQueryDto() = default;

            SrrQueryDto(const std::string action) : action(action) {
            }

            SrrQueryDto(const std::string action, const std::string data) : action(action), data(data) {
            }
        };

        void operator<<(messagebus::UserData &data, const SrrQueryDto &object);
        void operator>>(messagebus::UserData &inputData, SrrQueryDto &object);

        struct SrrFeaturesListDto {
            std::list<std::string> featuresList;

            SrrFeaturesListDto() = default;

            SrrFeaturesListDto(std::list<std::string> featuresList) : featuresList(featuresList) {
            }
        };

        void operator<<(messagebus::UserData &data, const SrrFeaturesListDto &object);
        void operator>>(messagebus::UserData &inputData, SrrFeaturesListDto &object);

        struct SrrResponseDto {
            std::string name;
            std::string status;
            std::string error;

            SrrResponseDto() = default;

            SrrResponseDto(const std::string name) : name(name) {
            }

            SrrResponseDto(const std::string name, const std::string status) : name(name), status(status) {
            }

            SrrResponseDto(const std::string name, const std::string status, const std::string error) : name(name), status(status), error(error) {
            }
        };

        void operator<<(messagebus::UserData &data, const SrrResponseDto &object);
        void operator>>(messagebus::UserData &inputData, SrrResponseDto &object);

        struct SrrResponseDtoList {
            std::string status;
            std::list<SrrResponseDto> responseList;

            SrrResponseDtoList() = default;

            SrrResponseDtoList(const std::string status, const std::list<SrrResponseDto> responseList) : status(status), responseList(responseList) {
            }
        };

        void operator<<(messagebus::UserData &data, const SrrResponseDtoList &object);
        void operator>>(messagebus::UserData &inputData, SrrResponseDtoList &object);

        // ========================= Configuration ==================
        //
        // ==========================================================

        struct ConfigQueryDto {
            std::string action;
            std::string featureName;
            std::string data;

            ConfigQueryDto() = default;

            ConfigQueryDto(const std::string action) : action(action) {
            }

            ConfigQueryDto(const std::string action, const std::string featureName) : action(action), featureName(featureName) {
            }

            ConfigQueryDto(const std::string action, const std::string featureName, const std::string data) :
            action(action),
            featureName(featureName),
            data(data) {
            }
        };

        void operator<<(messagebus::UserData &data, const ConfigQueryDto &object);
        void operator>>(messagebus::UserData &inputData, ConfigQueryDto &object);

        struct ConfigResponseDto {
            std::string status;
            std::string data;
            std::string error;

            ConfigResponseDto() = default;

            ConfigResponseDto(const std::string status) : status(status) {
            }

            ConfigResponseDto(const std::string status, const std::string data) : status(status), data(data) {
            }

            ConfigResponseDto(const std::string status, const std::string data, const std::string error) : status(status), data(data), error(error) {
            }
        };

        void operator<<(messagebus::UserData &data, const ConfigResponseDto &object);
        void operator>>(messagebus::UserData &inputData, ConfigResponseDto &object);
    }
}

#endif
