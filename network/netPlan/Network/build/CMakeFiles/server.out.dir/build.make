# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/username/Desktop/Linux_Programming/network/netPlan/Network

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/username/Desktop/Linux_Programming/network/netPlan/Network/build

# Include any dependencies generated for this target.
include CMakeFiles/server.out.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/server.out.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/server.out.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server.out.dir/flags.make

CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o: CMakeFiles/server.out.dir/flags.make
CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o: /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.grpc.pb.cc
CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o: CMakeFiles/server.out.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/username/Desktop/Linux_Programming/network/netPlan/Network/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o -MF CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o.d -o CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o -c /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.grpc.pb.cc

CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.grpc.pb.cc > CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.i

CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.grpc.pb.cc -o CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.s

CMakeFiles/server.out.dir/NetworkTest.pb.cc.o: CMakeFiles/server.out.dir/flags.make
CMakeFiles/server.out.dir/NetworkTest.pb.cc.o: /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.pb.cc
CMakeFiles/server.out.dir/NetworkTest.pb.cc.o: CMakeFiles/server.out.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/username/Desktop/Linux_Programming/network/netPlan/Network/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/server.out.dir/NetworkTest.pb.cc.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server.out.dir/NetworkTest.pb.cc.o -MF CMakeFiles/server.out.dir/NetworkTest.pb.cc.o.d -o CMakeFiles/server.out.dir/NetworkTest.pb.cc.o -c /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.pb.cc

CMakeFiles/server.out.dir/NetworkTest.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.out.dir/NetworkTest.pb.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.pb.cc > CMakeFiles/server.out.dir/NetworkTest.pb.cc.i

CMakeFiles/server.out.dir/NetworkTest.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.out.dir/NetworkTest.pb.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/username/Desktop/Linux_Programming/network/netPlan/Network/NetworkTest.pb.cc -o CMakeFiles/server.out.dir/NetworkTest.pb.cc.s

CMakeFiles/server.out.dir/test_server.cpp.o: CMakeFiles/server.out.dir/flags.make
CMakeFiles/server.out.dir/test_server.cpp.o: /home/username/Desktop/Linux_Programming/network/netPlan/Network/test_server.cpp
CMakeFiles/server.out.dir/test_server.cpp.o: CMakeFiles/server.out.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/username/Desktop/Linux_Programming/network/netPlan/Network/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/server.out.dir/test_server.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server.out.dir/test_server.cpp.o -MF CMakeFiles/server.out.dir/test_server.cpp.o.d -o CMakeFiles/server.out.dir/test_server.cpp.o -c /home/username/Desktop/Linux_Programming/network/netPlan/Network/test_server.cpp

CMakeFiles/server.out.dir/test_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.out.dir/test_server.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/username/Desktop/Linux_Programming/network/netPlan/Network/test_server.cpp > CMakeFiles/server.out.dir/test_server.cpp.i

CMakeFiles/server.out.dir/test_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.out.dir/test_server.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/username/Desktop/Linux_Programming/network/netPlan/Network/test_server.cpp -o CMakeFiles/server.out.dir/test_server.cpp.s

# Object files for target server.out
server_out_OBJECTS = \
"CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o" \
"CMakeFiles/server.out.dir/NetworkTest.pb.cc.o" \
"CMakeFiles/server.out.dir/test_server.cpp.o"

# External object files for target server.out
server_out_EXTERNAL_OBJECTS =

server.out: CMakeFiles/server.out.dir/NetworkTest.grpc.pb.cc.o
server.out: CMakeFiles/server.out.dir/NetworkTest.pb.cc.o
server.out: CMakeFiles/server.out.dir/test_server.cpp.o
server.out: CMakeFiles/server.out.dir/build.make
server.out: /usr/lib/libprotobuf.so
server.out: /usr/lib/libgrpc++.so.1.53.0
server.out: /usr/lib/libgrpc.so.30.1.0
server.out: /usr/lib/libcares.so.2.6.1
server.out: /usr/lib/libaddress_sorting.so.30.1.0
server.out: /usr/lib/libupb.so.30.1.0
server.out: /usr/lib/libabsl_raw_hash_set.so.2301.0.0
server.out: /usr/lib/libabsl_hashtablez_sampler.so.2301.0.0
server.out: /usr/lib/libabsl_hash.so.2301.0.0
server.out: /usr/lib/libabsl_city.so.2301.0.0
server.out: /usr/lib/libabsl_low_level_hash.so.2301.0.0
server.out: /usr/lib/libabsl_statusor.so.2301.0.0
server.out: /usr/lib/libgpr.so.30.1.0
server.out: /usr/lib/libabsl_bad_variant_access.so.2301.0.0
server.out: /usr/lib/libabsl_status.so.2301.0.0
server.out: /usr/lib/libabsl_strerror.so.2301.0.0
server.out: /usr/lib/libabsl_random_distributions.so.2301.0.0
server.out: /usr/lib/libabsl_random_seed_sequences.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_pool_urbg.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_randen.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_randen_hwaes.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_randen_hwaes_impl.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_randen_slow.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_platform.so.2301.0.0
server.out: /usr/lib/libabsl_random_internal_seed_material.so.2301.0.0
server.out: /usr/lib/libabsl_random_seed_gen_exception.so.2301.0.0
server.out: /usr/lib/libabsl_cord.so.2301.0.0
server.out: /usr/lib/libabsl_bad_optional_access.so.2301.0.0
server.out: /usr/lib/libabsl_cordz_info.so.2301.0.0
server.out: /usr/lib/libabsl_cord_internal.so.2301.0.0
server.out: /usr/lib/libabsl_cordz_functions.so.2301.0.0
server.out: /usr/lib/libabsl_exponential_biased.so.2301.0.0
server.out: /usr/lib/libabsl_cordz_handle.so.2301.0.0
server.out: /usr/lib/libabsl_crc_cord_state.so.2301.0.0
server.out: /usr/lib/libabsl_crc32c.so.2301.0.0
server.out: /usr/lib/libabsl_crc_internal.so.2301.0.0
server.out: /usr/lib/libabsl_crc_cpu_detect.so.2301.0.0
server.out: /usr/lib/libabsl_str_format_internal.so.2301.0.0
server.out: /usr/lib/libabsl_synchronization.so.2301.0.0
server.out: /usr/lib/libabsl_stacktrace.so.2301.0.0
server.out: /usr/lib/libabsl_symbolize.so.2301.0.0
server.out: /usr/lib/libabsl_debugging_internal.so.2301.0.0
server.out: /usr/lib/libabsl_demangle_internal.so.2301.0.0
server.out: /usr/lib/libabsl_graphcycles_internal.so.2301.0.0
server.out: /usr/lib/libabsl_malloc_internal.so.2301.0.0
server.out: /usr/lib/libabsl_time.so.2301.0.0
server.out: /usr/lib/libabsl_strings.so.2301.0.0
server.out: /usr/lib/libabsl_throw_delegate.so.2301.0.0
server.out: /usr/lib/libabsl_int128.so.2301.0.0
server.out: /usr/lib/libabsl_strings_internal.so.2301.0.0
server.out: /usr/lib/libabsl_base.so.2301.0.0
server.out: /usr/lib/libabsl_spinlock_wait.so.2301.0.0
server.out: /usr/lib/libabsl_raw_logging_internal.so.2301.0.0
server.out: /usr/lib/libabsl_log_severity.so.2301.0.0
server.out: /usr/lib/libabsl_civil_time.so.2301.0.0
server.out: /usr/lib/libabsl_time_zone.so.2301.0.0
server.out: /usr/lib/libssl.so
server.out: /usr/lib/libcrypto.so
server.out: /usr/lib/libz.so
server.out: /usr/lib/libsystemd.so
server.out: /usr/lib/libprotobuf.so
server.out: CMakeFiles/server.out.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/username/Desktop/Linux_Programming/network/netPlan/Network/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable server.out"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.out.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server.out.dir/build: server.out
.PHONY : CMakeFiles/server.out.dir/build

CMakeFiles/server.out.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server.out.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server.out.dir/clean

CMakeFiles/server.out.dir/depend:
	cd /home/username/Desktop/Linux_Programming/network/netPlan/Network/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/username/Desktop/Linux_Programming/network/netPlan/Network /home/username/Desktop/Linux_Programming/network/netPlan/Network /home/username/Desktop/Linux_Programming/network/netPlan/Network/build /home/username/Desktop/Linux_Programming/network/netPlan/Network/build /home/username/Desktop/Linux_Programming/network/netPlan/Network/build/CMakeFiles/server.out.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server.out.dir/depend

