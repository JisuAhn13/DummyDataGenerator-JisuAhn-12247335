cmake_minimum_required(VERSION 3.16)
project(DummyDataGenerator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# DataPersistence 빌드에서 이미 다운로드된 nlohmann/json 재사용
set(NLOHMANN_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/../DataPersistence/build/_deps/nlohmann_json-src")
if(EXISTS "${NLOHMANN_CACHE}")
    FetchContent_Declare(nlohmann_json SOURCE_DIR "${NLOHMANN_CACHE}")
else()
    FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    TRUE
    )
endif()
FetchContent_MakeAvailable(nlohmann_json)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(DummyDataGenerator main.cpp)
target_link_libraries(DummyDataGenerator PRIVATE nlohmann_json::nlohmann_json)
