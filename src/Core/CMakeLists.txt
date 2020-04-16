

add_subdirectory(QueryParser)
add_subdirectory(TAPN)
add_subdirectory(TAPNParser)

add_library(Core ArgsParser.cpp VerificationOptions.cpp)
target_link_libraries(Core QueryParser TAPN TAPNParser)
