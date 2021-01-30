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
        src/*.cc
    PUBLIC_INCLUDE_DIR
        include
    PUBLIC
        *.h
    USES_PRIVATE
        cxxtools
    USES_PUBLIC
        protobuf
)

#Generate the protofiles for cpp
protobuf_generate_cpp(PROTO_SRCS PROTO_PUBLIC_HDRS proto/srr.proto)
target_sources(${PROJECT_NAME} PRIVATE ${PROTO_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

#install the Generated headers
install(FILES ${PROTO_PUBLIC_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
