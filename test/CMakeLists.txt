find_package (Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${TEST_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     ${verifydtapn_SOURCE_DIR}
                     )
add_definitions (-DBOOST_TEST_DYN_LINK)

add_executable (build_net build_net.cpp)


target_link_libraries(build_net ${Boost_LIBRARIES} verifydtapn DiscreteVerification Core)

add_test(NAME build_net COMMAND build_net)

set_tests_properties(build_net PROPERTIES
    ENVIRONMENT TEST_FILES=${CMAKE_CURRENT_SOURCE_DIR})
