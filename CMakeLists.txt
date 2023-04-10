cmake_minimum_required(VERSION 3.16)

project(PspBlueLightFilter)

set(CMAKE_C_STANDARD 11)

if (NOT PLATFORM_PSP)
  message(FATAL_ERROR "Can only build for PSP")
endif()

# Enable clang-tidy integration
set(CMAKE_C_CLANG_TIDY clang-tidy-15)
set(CMAKE_CXX_CLANG_TIDY ${CMAKE_C_CLANG_TIDY})

# Enable Cppcheck integration
set(CMAKE_C_CPPCHECK
  cppcheck
  --error-exitcode=1
  --inline-suppr
  --suppress=missingIncludeSystem
  --enable=warning,style,performance,portability,information
)
set(CMAKE_CXX_CPPCHECK ${CMAKE_C_CPPCHECK})

# Enable (most) compiler warnings and have them treated as errors
add_compile_options(-Wall -Wextra -Werror)

# Make sure the PSP library headers are seen as system headers
# so that we don't get static analysis errors from within them
string(REPLACE "-I" "-isystem " CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

# Enable Link-Time Optimization, AKA Inter-Procedure Optimization
if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
  include(CheckIPOSupported)
  check_ipo_supported(RESULT LTOAvailable)

  if(LTOAvailable)
    message("Link-time optimization enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()

# PSPSDK cmake doesn't support just building PRX
function(build_prx ARG_TARGET)
  target_link_options(${ARG_TARGET}
    PUBLIC -specs=${PSPDEV}/psp/sdk/lib/prxspecs
    PUBLIC -Wl,-q,-T${PSPDEV}/psp/sdk/lib/linkfile.prx
    PUBLIC ${PSPDEV}/psp/sdk/lib/prxexports.o
  )

  add_custom_command(
    TARGET ${ARG_TARGET}
    POST_BUILD COMMAND
    "$ENV{PSPDEV}/bin/psp-fixup-imports" "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_TARGET}"
    COMMENT "Calling psp-fixup-imports"
  )
  add_custom_command(
    TARGET ${ARG_TARGET}
    POST_BUILD COMMAND
    "${PSPDEV}/bin/psp-prxgen" "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_TARGET}"
    "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_TARGET}.prx"
    COMMENT "Calling prxgen"
  )
endfunction()

add_library(BlueLightFilterLib OBJECT src/blue_light_filter.c)

# Don't want to do static analysis on the ASM
add_library(
  target_no_static_code_analysis
  OBJECT
  src/imports.S
)
set_target_properties(
  target_no_static_code_analysis
  PROPERTIES
    C_CLANG_TIDY ""
    C_CPPCHECK ""
    C_CPPLINT ""
)
set_property(SOURCE src/imports.S PROPERTY LANGUAGE C)

add_executable(
  BlueLightFilter
  src/exports.exp
  src/main.c
  src/hook_display.c
  $<TARGET_OBJECTS:target_no_static_code_analysis>
  $<TARGET_OBJECTS:BlueLightFilterLib>
)
set(TOP_LEVEL_INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
target_include_directories(BlueLightFilter PRIVATE ${TOP_LEVEL_INCLUDES_DIR})
# -nostdlib along with linking against pspkernel is equivelant to USE_KERNEL_LIBC in an older PSP make file
# (which is now controlled by USE_KERNEL_LIBS in make files used by the current version of the SDK)
target_link_options(BlueLightFilter PRIVATE -nostdlib)

target_link_libraries(
  BlueLightFilter
  PRIVATE ${PSP_LIBRARIES} pspdisplay pspkernel pspmodinfo
)
build_prx(BlueLightFilter)


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests/test_prx_files)
endif()
