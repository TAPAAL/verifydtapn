

add_subdirectory(Query)
add_library(PlaceVisitor PlaceVisitor.cpp)
target_link_libraries(PlaceVisitor Query)

add_subdirectory(TAPN)

add_library(Core ArgsParser.cpp VerificationOptions.cpp)

add_dependencies(Core argparse-ext)

target_link_libraries(Core Query TAPN)
