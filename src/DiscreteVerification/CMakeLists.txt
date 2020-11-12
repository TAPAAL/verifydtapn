
add_subdirectory(DataStructures)
add_subdirectory(Util)
add_subdirectory(SearchStrategies)
add_subdirectory(VerificationTypes)
add_subdirectory(Generators)


add_library(DiscreteVerification DeadlockVisitor.cpp
        DiscreteVerification.cpp)

target_link_libraries(DiscreteVerification DataStructures Util SearchStrategies VerificationTypes Generators)
