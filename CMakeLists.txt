cmake_minimum_required(VERSION 3.16)
project(MonopolyDeal)
set(PROJECT_NAME MonopolyDeal)
set(GAME_PROJECT_NAME MonopolyGame)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_PROJECT_NAME})

set(GAME_PUBLIC_INCLUDES
    include/Game.h
    include/JsonConstants.h
    include/IControllerIO.h
)

set(GAME_PRIVATE_INCLUDES
    src/Card.h
    src/CardSet.h
    src/Player.h
    src/CSVRow.h
    src/controllers/AIController.h
    src/controllers/PlayerController.h
)

set(GAME_PRIVATE_SOURCE
    src/CardSet.cpp
    src/Game.cpp
    src/Player.cpp
    src/controllers/AIController.cpp
)

set(GAME_ALL_SOURCES
    ${GAME_PUBLIC_INCLUDES}
    ${GAME_PRIVATE_INCLUDES}
    ${GAME_PRIVATE_SOURCE}
)

add_library(${PROJECT_NAME} STATIC
    ${GAME_ALL_SOURCES}
)

target_precompile_headers(${PROJECT_NAME} PUBLIC include/Monopoly_pch.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

add_executable(${GAME_PROJECT_NAME}
    src/MonopolyDeal.cpp
)
target_include_directories(${GAME_PROJECT_NAME} PUBLIC include)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

add_subdirectory(json)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json)
target_link_libraries(${GAME_PROJECT_NAME} PUBLIC nlohmann_json)
target_link_libraries(${GAME_PROJECT_NAME} PUBLIC ${PROJECT_NAME})

add_custom_command(
        TARGET ${GAME_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/cardList.csv
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(
        TARGET ${GAME_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/cardList.csv
                ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Tests
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_PROJECT_NAME MonopolyTest)

add_executable(${TEST_PROJECT_NAME}
    tests/Test.cpp
)
target_link_libraries(${TEST_PROJECT_NAME}
  ${PROJECT_NAME}
  GTest::gtest_main
)

target_include_directories(${TEST_PROJECT_NAME} PUBLIC include)

include(GoogleTest)
gtest_discover_tests(${TEST_PROJECT_NAME})

add_custom_command(
        TARGET ${TEST_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/saves/
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/tests/saves/)

add_custom_command(
        TARGET ${TEST_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/saves/
                ${CMAKE_CURRENT_BINARY_DIR}/tests/saves/)
