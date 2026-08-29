cmake_minimum_required(VERSION 3.10)
project(rbs LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)

add_compile_definitions(_POSIX_C_SOURCE=200809L)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- sm State-Machine (rbs_sm steuert die FSM) ---
add_subdirectory(../../libraries/sm ${CMAKE_BINARY_DIR}/sm)

add_executable(rbs)

target_sources(rbs PRIVATE
	src/main.c
	src/rbs.c
	src/rbs_sm.c
)

target_include_directories(rbs PRIVATE
	../../libraries
)

target_link_libraries(rbs
	PUBLIC
		m
		sm
)

# --- Tests (Regression gegen Off-by-one-Indizierung) ---
enable_testing()

add_executable(rbs_test)

target_sources(rbs_test PRIVATE
	test/test_rbs.c
	src/rbs.c
)

target_include_directories(rbs_test PRIVATE
	src
	../../libraries
)

add_test(NAME rbs_test COMMAND rbs_test)

# --- Test für Effekt-Staging-Semantik (gemeinsamer Trigger feuert alle Effekte) ---
add_executable(rbs_effects_test)

target_sources(rbs_effects_test PRIVATE
	test/test_effects.c
	src/rbs.c
)

target_include_directories(rbs_effects_test PRIVATE
	src
	../../libraries
)

add_test(NAME rbs_effects_test COMMAND rbs_effects_test)

# --- Test für die rbs_sm-State-Machine (Marker-Routing + Konsumieren) ---
add_executable(rbs_sm_test)

target_sources(rbs_sm_test PRIVATE
	test/test_rbs_sm.c
	src/rbs.c
	src/rbs_sm.c
)

# rbs_sm.c ruft sm_run(); sm zieht threading/string transitiv mit.
target_include_directories(rbs_sm_test PRIVATE
	src
	../../libraries
)

target_link_libraries(rbs_sm_test
	PUBLIC
		m
		sm
)

add_test(NAME rbs_sm_test COMMAND rbs_sm_test)


