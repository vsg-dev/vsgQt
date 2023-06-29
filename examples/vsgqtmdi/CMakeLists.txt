set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgqtmdi ${MODE} ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(vsgqtmdi vsgQt)

if (vsgXchange_FOUND)
    target_compile_definitions(vsgqtmdi PRIVATE vsgXchange_FOUND)
    target_link_libraries(vsgqtmdi vsgXchange::vsgXchange)
endif()


install(TARGETS vsgqtmdi DESTINATION bin)

