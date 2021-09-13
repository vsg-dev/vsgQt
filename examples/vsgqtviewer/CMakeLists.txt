set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgqtviewer ${MODE} ${SOURCES} ${HEADERS} ${FORMS})
target_link_libraries(vsgqtviewer
    PRIVATE
        vsgQt
)
if(vsgXchange_FOUND)
    target_link_libraries(vsgqtviewer PRIVATE vsgXchange::vsgXchange)
endif()


install(TARGETS vsgqtviewer DESTINATION bin)

