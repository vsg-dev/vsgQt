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

install(TARGETS vsgqtviewer DESTINATION bin)

