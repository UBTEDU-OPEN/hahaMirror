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
CMAKE_SOURCE_DIR = /home/ubt/code/Qt/hahaMirror

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubt/code/Qt/hahaMirror/build

# Utility rule file for hahaMirror_autogen.

# Include any custom commands dependencies for this target.
include src/CMakeFiles/hahaMirror_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/hahaMirror_autogen.dir/progress.make

src/CMakeFiles/hahaMirror_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ubt/code/Qt/hahaMirror/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target hahaMirror"
	cd /home/ubt/code/Qt/hahaMirror/build/src && /home/ubt/tools/build/cmake-3.22.1-linux-x86_64/bin/cmake -E cmake_autogen /home/ubt/code/Qt/hahaMirror/build/src/CMakeFiles/hahaMirror_autogen.dir/AutogenInfo.json Release

hahaMirror_autogen: src/CMakeFiles/hahaMirror_autogen
hahaMirror_autogen: src/CMakeFiles/hahaMirror_autogen.dir/build.make
.PHONY : hahaMirror_autogen

# Rule to build all files generated by this target.
src/CMakeFiles/hahaMirror_autogen.dir/build: hahaMirror_autogen
.PHONY : src/CMakeFiles/hahaMirror_autogen.dir/build

src/CMakeFiles/hahaMirror_autogen.dir/clean:
	cd /home/ubt/code/Qt/hahaMirror/build/src && $(CMAKE_COMMAND) -P CMakeFiles/hahaMirror_autogen.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/hahaMirror_autogen.dir/clean

src/CMakeFiles/hahaMirror_autogen.dir/depend:
	cd /home/ubt/code/Qt/hahaMirror/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubt/code/Qt/hahaMirror /home/ubt/code/Qt/hahaMirror/src /home/ubt/code/Qt/hahaMirror/build /home/ubt/code/Qt/hahaMirror/build/src /home/ubt/code/Qt/hahaMirror/build/src/CMakeFiles/hahaMirror_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/hahaMirror_autogen.dir/depend

