set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgqtviewer ${MODE} ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(vsgqtviewer vsgQt)

if (vsgXchange_FOUND)
    target_compile_definitions(vsgqtviewer PRIVATE vsgXchange_FOUND)
    target_link_libraries(vsgqtviewer vsgXchange::vsgXchange)
endif()


install(TARGETS vsgqtviewer DESTINATION bin)

