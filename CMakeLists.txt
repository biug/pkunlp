cmake_minimum_required(VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(libGrass)

option(STATIC "Statically link to glibc" OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/)
include_directories(${PROJECT_SOURCE_DIR})

set(SOURCE_FILES
    common/parser/fittings/pseudo_tree.cpp
    common/parser/fittings/pseudo_tree.h
    common/parser/implementations/arceager/arceager_depparser.cpp
    common/parser/implementations/arceager/arceager_depparser.h
    common/parser/implementations/arceager/arceager_macros.cpp
    common/parser/implementations/arceager/arceager_macros.h
    common/parser/implementations/arceager/arceager_run.cpp
    common/parser/implementations/arceager/arceager_run.h
    common/parser/implementations/arceager/arceager_state.cpp
    common/parser/implementations/arceager/arceager_state.h
    common/parser/implementations/arceager/arceager_weight.cpp
    common/parser/implementations/arceager/arceager_weight.h
    common/parser/implementations/graph_transition/titov/titov_depparser.h
    common/parser/implementations/graph_transition/titov/titov_macros.cpp
    common/parser/implementations/graph_transition/titov/titov_macros.h
    common/parser/implementations/graph_transition/titov/titov_run.h
    common/parser/implementations/graph_transition/titov/titov_state.cpp
    common/parser/implementations/graph_transition/titov/titov_state.h
    common/parser/implementations/graph_transition/twostack/twostack_depparser.h
    common/parser/implementations/graph_transition/twostack/twostack_macros.cpp
    common/parser/implementations/graph_transition/twostack/twostack_macros.h
    common/parser/implementations/graph_transition/twostack/twostack_run.h
    common/parser/implementations/graph_transition/twostack/twostack_state.cpp
    common/parser/implementations/graph_transition/twostack/twostack_state.h
    common/parser/implementations/graph_transition/graph_transition_depparser.h
    common/parser/implementations/graph_transition/graph_transition_macros.cpp
    common/parser/implementations/graph_transition/graph_transition_macros.h
    common/parser/implementations/graph_transition/graph_transition_run.h
    common/parser/implementations/graph_transition/graph_transition_state.cpp
    common/parser/implementations/graph_transition/graph_transition_state.h
    common/parser/implementations/graph_transition/graph_transition_weight.h
    common/parser/implementations/graph_transition/sim_one_stack_weight.h
    common/parser/implementations/graph_transition/sim_two_stack_weight.h
    common/parser/implementations/POSTagging/postag_depparser.cpp
    common/parser/implementations/POSTagging/postag_depparser.h
    common/parser/implementations/POSTagging/postag_macros.cpp
    common/parser/implementations/POSTagging/postag_macros.h
    common/parser/implementations/POSTagging/postag_run.cpp
    common/parser/implementations/POSTagging/postag_run.h
    common/parser/implementations/POSTagging/postag_weight.cpp
    common/parser/implementations/POSTagging/postag_weight.h
    common/parser/implementations/Segment/seg_depparser.cpp
    common/parser/implementations/Segment/seg_depparser.h
    common/parser/implementations/Segment/seg_macros.cpp
    common/parser/implementations/Segment/seg_macros.h
    common/parser/implementations/Segment/seg_run.cpp
    common/parser/implementations/Segment/seg_run.h
    common/parser/implementations/Segment/seg_weight.cpp
    common/parser/implementations/Segment/seg_weight.h
    common/parser/agenda.h
    common/parser/depparser_base.h
    common/parser/macros_base.cpp
    common/parser/macros_base.h
    common/parser/run_base.h
    common/parser/weight_base.h
    common/token/pos.cpp
    common/token/pos.h
    common/token/tagset.h
    common/token/token.h
    common/token/word.cpp
    common/token/word.h
    include/learning/perceptron/packed_score.h
    include/learning/perceptron/score.h
    include/learning/perceptron/score_array.h
    include/learning/tree/lca.cpp
    include/learning/tree/lca.h
    include/dependency_primitive.cpp
    include/dependency_primitive.h
    include/ngram.h
    LibGrass.cpp
    LibGrass.h
    stdafx.cpp
    stdafx.h
    targetver.h)



if(STATIC STREQUAL "ON")
    if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
    	LIST(APPEND ${SOURCE_FILES} common/utils/memcpy.c)
    	add_library(grass SHARED ${SOURCE_FILES})
        target_link_libraries(grass -static-libgcc -static-libstdc++ -Wl,--wrap=memcpy)
    else()
    	add_library(grass SHARED ${SOURCE_FILES})
        target_link_libraries(grass -static-libgcc -static-libstdc++)
    endif()
else()
    add_library(grass SHARED ${SOURCE_FILES})
endif()

target_compile_definitions(grass PRIVATE LIBGRASS_EXPORTS=1)
