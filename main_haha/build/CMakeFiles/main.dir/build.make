# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/ubt/tools/build/cmake-3.22.1-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /home/ubt/tools/build/cmake-3.22.1-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubt/code/algorithm/hushuping/main_haha

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubt/code/algorithm/hushuping/main_haha/build

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/main.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/src/main.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/main.dir/src/main.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubt/code/algorithm/hushuping/main_haha/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/src/main.cpp.o -MF CMakeFiles/main.dir/src/main.cpp.o.d -o CMakeFiles/main.dir/src/main.cpp.o -c /home/ubt/code/algorithm/hushuping/main_haha/src/main.cpp

CMakeFiles/main.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubt/code/algorithm/hushuping/main_haha/src/main.cpp > CMakeFiles/main.dir/src/main.cpp.i

CMakeFiles/main.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubt/code/algorithm/hushuping/main_haha/src/main.cpp -o CMakeFiles/main.dir/src/main.cpp.s

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/src/main.cpp.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

main: CMakeFiles/main.dir/src/main.cpp.o
main: CMakeFiles/main.dir/build.make
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_dnn.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_gapi.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_highgui.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_ml.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_objdetect.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_photo.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_stitching.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_video.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_videoio.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/deployment_tools/inference_engine/lib/intel64/libinference_engine_legacy.so
main: /home/ubt/intel/openvino_2020.3.194/deployment_tools/inference_engine/lib/intel64/libinference_engine.so
main: /home/ubt/intel/openvino_2020.3.194/deployment_tools/inference_engine/lib/intel64/libinference_engine_c_api.so
main: /home/ubt/intel/openvino_2020.3.194/deployment_tools/inference_engine/lib/intel64/libinference_engine_nn_builder.so
main: ../lib/libFaceRec.so
main: ../lib/libFaceHaha.so
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_imgcodecs.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_calib3d.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_features2d.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_flann.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_imgproc.so.4.3.0
main: /home/ubt/intel/openvino_2020.3.194/opencv/lib/libopencv_core.so.4.3.0
main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubt/code/algorithm/hushuping/main_haha/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: main
.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /home/ubt/code/algorithm/hushuping/main_haha/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubt/code/algorithm/hushuping/main_haha /home/ubt/code/algorithm/hushuping/main_haha /home/ubt/code/algorithm/hushuping/main_haha/build /home/ubt/code/algorithm/hushuping/main_haha/build /home/ubt/code/algorithm/hushuping/main_haha/build/CMakeFiles/main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main.dir/depend

