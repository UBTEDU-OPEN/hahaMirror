find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS})

find_package(InferenceEngine)

include_directories(
                include
                include/faceRec
                include/faceHaha
                include/common
                ${InferenceEngine_INCLUDE_DIRS}
                ${OpenCV_INCLUDE_DIRS})

#link_directories(
#        /opt/intel/openvino/deployment_tools/inference_engine/external/tbb/lib
#        /opt/intel/openvino/deployment_tools/ngraph/lib)

set(UI_FORMS
    mainwindow.ui
)

set(SRC_LISTS
    briefhttpclient.cpp
    facedetect.cpp 
    faceidentify.cpp 
    hahacore.cpp 
    hahaui.cpp
    http_client.cc 
    image.cpp 
    imageoverlay.cpp 
    main.cpp 
    mainwindow.cpp 
    playcamera.cpp 
    producer_record_impl.cc 
    resolution.cpp
    websocket_server.cc


    ${UI_FORMS}
    ${UI_RESOURCES}
)

add_executable(${PROJECT_NAME} ${SRC_LISTS})

target_link_libraries(${PROJECT_NAME}
                      ${OpenCV_LIBS}
                      ${InferenceEngine_LIBRARIES} dl
                      ${CMAKE_SOURCE_DIR}/main_haha/lib/libFaceRec.so
                      ${CMAKE_SOURCE_DIR}/main_haha/lib/libFaceHaha.so
                      pthread curl)

target_link_libraries(${PROJECT_NAME} common)
target_link_libraries(${PROJECT_NAME}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
)

