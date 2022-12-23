cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME MonopolyDeal)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MonopolyDeal)

set(GAME_PUBLIC_INCLUDES
    include/Game.h
)

set(GAME_PRIVATE_INCLUDES
    src/Card.h
    src/CardSet.h
    src/Player.h
)

set(GAME_PRIVATE_SOURCE
    src/Card.cpp
    src/CardSet.cpp
    src/Game.cpp
    src/Player.cpp
    src/MonopolyDeal.cpp
)

set(GAME_ALL_SOURCES
    ${GAME_PUBLIC_INCLUDES}
    ${GAME_PRIVATE_INCLUDES}
    ${GAME_PRIVATE_SOURCE}
)

add_library(${PROJECT_NAME} STATIC
    ${GAME_ALL_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

