include_directories(/usr/include/freetype2/)

set(COMMON_SRCS
    checkcode.cc
    ConvertUTF.c
    fileopt.cc
    logging.cc
    mongoose.c
    processinfo.cc
    # putpng.cc
    puttext.cc
    run_cmd.cc
    string.cc
    time.cc
)

add_library(common SHARED ${COMMON_SRCS})
target_link_libraries(common freetype spdlog muduo_base ${OpenCV_LIBS} Boost::filesystem Boost::system pthread)
