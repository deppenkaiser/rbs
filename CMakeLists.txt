cmake_minimum_required(VERSION 3.10)

project(rbs LANGUAGES C)

include_guard(GLOBAL)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)

add_compile_definitions(_POSIX_C_SOURCE=200809L)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(rbs STATIC)
add_subdirectory(../sm ${CMAKE_BINARY_DIR}/sm_from_rbs)
add_subdirectory(../logging ${CMAKE_BINARY_DIR}/logging_from_rbs)
add_subdirectory(../api ${CMAKE_BINARY_DIR}/api_from_rbs)

target_sources(rbs PRIVATE
	rbs.c
	rbs_sm.c
)

# Include-Root auf den rbs-Repo-Pfad, damit "<rbs/rbs.h>" und
# "<rbs/rbs_sm.h>" aus allen Konsumenten (z. B. rbs_demo) auflösen.
# Die transitiven Includes (<sm/sm.h>, <logging/logging.h>) liefern die
# PUBLIC-Links auf sm/logging.
target_include_directories(rbs PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rbs
	PUBLIC
		m
		sm
		logging
)

# --- Tests (Regression gegen Off-by-one-Indizierung) ---
enable_testing()

add_executable(rbs_test)
target_sources(rbs_test PRIVATE test/test_rbs.c)
target_link_libraries(rbs_test PRIVATE rbs logging)
add_test(NAME rbs_test COMMAND rbs_test)

add_executable(rbs_effects_test)
target_sources(rbs_effects_test PRIVATE test/test_effects.c)
target_link_libraries(rbs_effects_test PRIVATE rbs logging)
add_test(NAME rbs_effects_test COMMAND rbs_effects_test)

add_executable(rbs_sm_test)
target_sources(rbs_sm_test PRIVATE test/test_rbs_sm.c)
target_link_libraries(rbs_sm_test PRIVATE rbs logging)
add_test(NAME rbs_sm_test COMMAND rbs_sm_test)

add_executable(rbs_else_test)
target_sources(rbs_else_test PRIVATE test/test_else.c)
target_link_libraries(rbs_else_test PRIVATE rbs logging)
add_test(NAME rbs_else_test COMMAND rbs_else_test)
