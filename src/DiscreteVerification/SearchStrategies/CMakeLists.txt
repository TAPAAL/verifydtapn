
add_library(SearchStrategies LivenessWeightQueryVisitor.cpp  WeightQueryVisitor.cpp)

add_dependencies(SearchStrategies ptrie-ext)
