set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgqtnative ${MODE} ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(vsgqtnative vsgQt)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

if (vsgXchange_FOUND)
    target_compile_definitions(vsgqtnative PRIVATE vsgXchange_FOUND)
    target_link_libraries(vsgqtnative vsgXchange::vsgXchange)
endif()


install(TARGETS vsgqtnative DESTINATION bin)

