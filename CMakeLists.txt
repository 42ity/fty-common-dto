cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

##############################################################################################################

project(fty_common_dto VERSION 1.0.0)

########################################################################################################################

find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR})

########################################################################################################################

find_package(Protobuf REQUIRED)

#Create the target
etn_target(shared ${PROJECT_NAME}
    SOURCES
        src/fty_commands_dto.cc
        src/fty_srr_dto.cc
        src/pb_helper.cc
    PUBLIC_INCLUDE_DIR
        include
    PUBLIC
        fty_commands_dto.h
        fty_common_dto_empty.h
        fty_common_dto.h
        fty_srr_dto.h
        fty_userdata_dto.h
        message_bus_config_pb.h
        pb_helper.h
        srr_pb.h
    FLAGS
      "-Wno-useless-cast"
    USES_PRIVATE
        cxxtools
        protobuf
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

########################################################################################################################

#Generate the protofiles for cpp
protobuf_generate_cpp(PROTO_SRCS PROTO_PUBLIC_HDRS
    proto/srr.proto
    proto/message_bus_config.proto
)
target_sources(${PROJECT_NAME} PRIVATE ${PROTO_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#install the Generated headers
install(FILES ${PROTO_PUBLIC_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
