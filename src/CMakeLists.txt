
add_subdirectory(Core)
add_subdirectory(DiscreteVerification)
add_executable(verifydtapn-${ARCH_TYPE} main.cpp)
add_dependencies(verifydtapn-${ARCH_TYPE} unfoldtacpn-ext)
target_link_libraries(verifydtapn-${ARCH_TYPE} Core DiscreteVerification unfoldtacpn)
