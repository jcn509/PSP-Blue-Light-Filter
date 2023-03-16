
add_library(
    TestCommon OBJECT
    test_main.c
  )
  
function(BuildTest ARG_TARGET ARG_PIXEL_FORMAT ARG_FILTER)
    add_executable(${ARG_TARGET} $<TARGET_OBJECTS:TestCommon> test_specifics.c)

    target_link_libraries(
        ${ARG_TARGET}  
        PRIVATE ${PSP_LIBRARIES} pspdisplay
    )

    build_prx(${ARG_TARGET})
    target_compile_definitions(${ARG_TARGET} PRIVATE PIXEL_FORMAT=${ARG_PIXEL_FORMAT})
    if(ARG_FILTER)
        target_compile_definitions(${ARG_TARGET} PRIVATE FILTER_ENABLED=1)
    endif()
    add_test(NAME "${ARG_TARGET}Test" COMMAND pytest ../test_filter.py WORKING_DIRECTORY ..)
endfunction()

BuildTest(WhiteTest565Filtered "PSP_DISPLAY_PIXEL_FORMAT_565" ON)
BuildTest(WhiteTest565UnFiltered "PSP_DISPLAY_PIXEL_FORMAT_565" OFF)
BuildTest(WhiteTest5551Filtered "PSP_DISPLAY_PIXEL_FORMAT_5551" ON)
BuildTest(WhiteTest5551UnFiltered "PSP_DISPLAY_PIXEL_FORMAT_5551" OFF)
BuildTest(WhiteTest4444Filtered "PSP_DISPLAY_PIXEL_FORMAT_4444" ON)
BuildTest(WhiteTest4444UnFiltered "PSP_DISPLAY_PIXEL_FORMAT_4444" OFF)
BuildTest(WhiteTest8888Filtered "PSP_DISPLAY_PIXEL_FORMAT_8888" ON)
BuildTest(WhiteTest8888UnFiltered "PSP_DISPLAY_PIXEL_FORMAT_8888" OFF)