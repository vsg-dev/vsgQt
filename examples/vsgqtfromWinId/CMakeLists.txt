set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgqtfromWinId ${MODE} ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(vsgqtfromWinId vsgQt)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

if (vsgXchange_FOUND)
    target_compile_definitions(vsgqtfromWinId PRIVATE vsgXchange_FOUND)
    target_link_libraries(vsgqtfromWinId vsgXchange::vsgXchange)
endif()


install(TARGETS vsgqtfromWinId DESTINATION bin)

