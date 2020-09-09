
add_subdirectory(DataStructures)
add_subdirectory(Util)
add_subdirectory(SearchStrategies)
add_subdirectory(VerificationTypes)


add_library(DiscreteVerification DeadlockVisitor.cpp
        DiscreteVerification.cpp
        Generator.cpp
        PlaceVisitor.cpp
        ReducingGenerator.cpp)

target_link_libraries(DiscreteVerification DataStructures Util SearchStrategies VerificationTypes)
