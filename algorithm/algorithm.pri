OPENVINO_ROOT_PATH=/home/ubt/intel/openvino/

LIBS += \
    -L$$OPENVINO_ROOT_PATH/opencv/lib/ -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_gapi -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio -lopencv_photo
LIBS += \
    -L$$OPENVINO_ROOT_PATH/deployment_tools/inference_engine/external/tbb/lib/ -ltbb \
    -L$$OPENVINO_ROOT_PATH/inference_engine/lib/intel64/ -linference_engine -linference_engine_legacy -linference_engine_transformations\
    -L$$OPENVINO_ROOT_PATH/deployment_tools/ngraph/lib/ -lngraph \
    -ldl
LIBS += \
    -L$$PWD/lib/ -lFaceHaha -lFaceRec
#    -lopencv_ml
#    -lopencv_objdetect
#    -lopencv_photo
#    -lopencv_stitching

HEADERS += \
    $$PWD/include/common/ai_common.h \
    $$PWD/include/faceHaha/face_haha_api.h \
    $$PWD/include/faceRec/face_recognition_api.h
