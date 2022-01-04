
add_subdirectory(Core)
add_subdirectory(DiscreteVerification)
add_library(verifydtapn verifydtapn.cpp)

add_dependencies(verifydtapn unfoldtacpn-ext boost-ext)
target_link_libraries(verifydtapn Core DiscreteVerification
    ${EXTERNAL_INSTALL_LOCATION}/lib/unfoldtacpn/libunfoldtacpn.a
    )

add_executable(verifydtapn-${ARCH_TYPE} main.cpp)

target_link_libraries(verifydtapn-${ARCH_TYPE} verifydtapn DiscreteVerification)
