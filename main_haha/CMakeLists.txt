cmake_minimum_required(VERSION 2.8)
project(main)

add_compile_options(-std=c++11)

add_definitions(-march=native)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #c++11

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#message("algorithm opencv dir: " ${OpenCV_INCLUDE_DIRS})

find_package(InferenceEngine)

message("InferenceEngine_LIBRARIES: " ${InferenceEngine_LIBRARIES})
message("InferenceEngine_LIBRARIES: " ${InferenceEngine_LIBRARIES})

include_directories(
		include
		include/faceRec
		include/faceHaha
		include/common
		${InferenceEngine_INCLUDE_DIRS}
		${OpenCV_INCLUDE_DIRS})

link_directories(
        /opt/intel/openvino/deployment_tools/inference_engine/external/tbb/lib
        /opt/intel/openvino/deployment_tools/ngraph/lib)

aux_source_directory(./src SRC_LIST1)
#ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST1})
add_executable(${PROJECT_NAME} ${SRC_LIST1})

target_link_libraries(${PROJECT_NAME}
                      ${OpenCV_LIBS}
                      ${InferenceEngine_LIBRARIES} dl
                      ${CMAKE_CURRENT_SOURCE_DIR}/lib/libFaceRec.so
                      ${CMAKE_CURRENT_SOURCE_DIR}/lib/libFaceHaha.so)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install)
