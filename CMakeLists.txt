cmake_minimum_required(VERSION 3.24)

project(PspBlueLightFilter)

set(CMAKE_C_STANDARD 11)

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


if (NOT PLATFORM_PSP)
    message(FATAL_ERROR "Can only build for PSP")
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
