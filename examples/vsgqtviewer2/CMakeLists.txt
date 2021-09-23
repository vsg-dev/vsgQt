set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgqtviewer2 ${MODE} ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(vsgqtviewer2 vsgQt)

if (vsgXchange_FOUND)
    target_compile_definitions(vsgqtviewer2 PRIVATE vsgXchange_FOUND)
    target_link_libraries(vsgqtviewer2 vsgXchange::vsgXchange)
endif()


install(TARGETS vsgqtviewer2 DESTINATION bin)

