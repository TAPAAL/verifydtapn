

add_library(DataStructures CoveredMarkingVisitor.cpp PWList.cpp TimeDartPWList.cpp WorkflowPWList.cpp NonStrictMarkingBase.cpp TimeDartLivenessPWList.cpp WaitingList.cpp)

add_dependencies(DataStructures ptrie-ext)