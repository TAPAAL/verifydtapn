

add_library(Query AST.cpp NormalizationVisitor.cpp TranslationVisitor.cpp)
add_dependencies(Query unfoldtacpn-ext)
