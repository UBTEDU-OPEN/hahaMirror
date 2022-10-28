include_directories(/usr/include/freetype2/)

set(JUMP_BASE_SRCS
    checkcode.cc
    ConvertUTF.c
    fileopt.cc
    logging.cc
    mongoose.c
    png.cc
    processinfo.cc
    # putpng.cc
    puttext.cc
    run_cmd.cc
    string.cc
    time.cc
)

add_library(common SHARED ${JUMP_BASE_SRCS})
target_link_libraries(common freetype spdlog muduo_base ${OpenCV_LIBS} Boost::filesystem Boost::system pthread)
