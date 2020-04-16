

add_library(TAPNParser TAPNXmlParser.cpp  util.cpp)
add_dependencies(TAPNParser rapidxml-ext)