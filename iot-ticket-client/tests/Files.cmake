
set(IOTAPI_TESTS_SOURCES
    tests/IOT_Tester.cpp
    tests/IOT_Base64Tester.cpp
    tests/IOT_RestClientTester.cpp
    tests/main.cpp
)

add_executable(iot-ticket-tests ${IOTAPI_TESTS_SOURCES} )
target_link_libraries(iot-ticket-tests IOT_API cppunit)

install(TARGETS iot-ticket-tests
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
