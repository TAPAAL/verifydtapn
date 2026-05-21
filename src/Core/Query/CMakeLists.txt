

add_library(Query AST.cpp SMCQuery.cpp NormalizationVisitor.cpp TranslationVisitor.cpp)
add_dependencies(Query unfoldtacpn-ext)
