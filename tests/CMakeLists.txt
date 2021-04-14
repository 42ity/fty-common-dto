cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

##############################################################################################################

etn_test_target(${PROJECT_NAME}
    SOURCES
        main.cpp
        fty_srr_dto_tests.cpp
    PREPROCESSOR -DCATCH_CONFIG_FAST_COMPILE
    USES
        ${PROJECT_NAME}
        fty_common
        cxxtools
        protobuf
)
