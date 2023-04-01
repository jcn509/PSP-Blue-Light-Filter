
add_library(
    TestCommon OBJECT
    src/test_main.c
    src/set_each_word_in_buffer.c
)

target_include_directories(TestCommon PRIVATE ${TOP_LEVEL_INCLUDES_DIR})
  
function(BuildTest ARG_TARGET ARG_FILL_BUFFER_SRC ARG_PIXEL_FORMAT ARG_FILTER)
    add_executable(${ARG_TARGET} $<TARGET_OBJECTS:TestCommon> $<TARGET_OBJECTS:${ARG_FILL_BUFFER_SRC}> $<TARGET_OBJECTS:BlueLightFilterLib> src/test_specifics.c)

    target_include_directories(${ARG_TARGET} PRIVATE ${TOP_LEVEL_INCLUDES_DIR})

    target_link_libraries(
        ${ARG_TARGET}  
        PRIVATE ${PSP_LIBRARIES} pspdisplay
    )

    build_prx(${ARG_TARGET})
    target_compile_definitions(${ARG_TARGET} PRIVATE PIXEL_FORMAT=${ARG_PIXEL_FORMAT})
    if(ARG_FILTER)
        target_compile_definitions(${ARG_TARGET} PRIVATE FILTER_ENABLED=1)
    endif()
endfunction()

function(BuildFilteredAndUnFilteredTestPrx ARG_FILL_BUFFER_SRC ARG_PIXEL_FORMAT)
    set(PRX_NAME_PREFIX "${ARG_PIXEL_FORMAT}_${ARG_FILL_BUFFER_SRC}")
    BuildTest("${PRX_NAME_PREFIX}_Filtered" ${ARG_FILL_BUFFER_SRC} ${ARG_PIXEL_FORMAT} ON)
    BuildTest("${PRX_NAME_PREFIX}_UnFiltered" ${ARG_FILL_BUFFER_SRC} ${ARG_PIXEL_FORMAT} OFF)
endfunction()

function(BuildFilteredAndUnFilteredTestPrxForAllFormats ARG_FILL_BUFFER_NAME ARG_FILL_BUFFER_SRC)
    add_library(
        ${ARG_FILL_BUFFER_NAME} OBJECT
        ${ARG_FILL_BUFFER_SRC}
    )

    BuildFilteredAndUnFilteredTestPrx(${ARG_FILL_BUFFER_NAME} "PSP_DISPLAY_PIXEL_FORMAT_565")
    BuildFilteredAndUnFilteredTestPrx(${ARG_FILL_BUFFER_NAME} "PSP_DISPLAY_PIXEL_FORMAT_5551")
    BuildFilteredAndUnFilteredTestPrx(${ARG_FILL_BUFFER_NAME} "PSP_DISPLAY_PIXEL_FORMAT_4444")
    BuildFilteredAndUnFilteredTestPrx(${ARG_FILL_BUFFER_NAME} "PSP_DISPLAY_PIXEL_FORMAT_8888")
endfunction()

# We need unfiltered versions so that we can check that e.g. the code to generate a blue screen actually works
BuildFilteredAndUnFilteredTestPrxForAllFormats(FillBufferWhite src/test_fill_buffer_white.c)
BuildFilteredAndUnFilteredTestPrxForAllFormats(FillBufferBlack src/test_fill_buffer_black.c)
BuildFilteredAndUnFilteredTestPrxForAllFormats(FillBufferBlue src/test_fill_buffer_blue.c)
BuildFilteredAndUnFilteredTestPrxForAllFormats(FillBufferYellow src/test_fill_buffer_yellow.c)
BuildFilteredAndUnFilteredTestPrxForAllFormats(FillBufferAlternatingBlackWhite src/test_fill_buffer_alternating_black_white.c)