if(VERIFYDTAPN_GetDependencies)
    add_library(Boost::unit_test_framework STATIC IMPORTED)
    set_target_properties(Boost::unit_test_framework PROPERTIES
        IMPORTED_LOCATION ${BOOST_UNIT_TEST_FRAMEWORK_LIBRARY})
else()
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
endif()
include_directories (${TEST_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     ${verifydtapn_SOURCE_DIR}
                     )
add_executable (build_net build_net.cpp)


target_link_libraries(build_net Boost::unit_test_framework verifydtapn DiscreteVerification Core)

add_test(NAME build_net COMMAND build_net)

set_tests_properties(build_net PROPERTIES
    ENVIRONMENT TEST_FILES=${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME fastest_trace_delay
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/fastest_trace/check_delay.sh
        $<TARGET_FILE:verifydtapn-${ARCH_TYPE}>
        ${CMAKE_CURRENT_SOURCE_DIR}/fastest_trace/model.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/fastest_trace/query.xml)
