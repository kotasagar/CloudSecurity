cmake_minimum_required(VERSION 3.14)
project(HybridCloudSecurity CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Automatically download and build Microsoft SEAL
FetchContent_Declare(
    SEAL
    GIT_REPOSITORY https://github.com/microsoft/SEAL.git
    GIT_TAG        v4.1.1 # Use a stable tag
)
FetchContent_MakeAvailable(SEAL)

add_executable(HybridCloudSecurity
    main.cpp
    fhe.cpp
    enclave.cpp
)

target_link_libraries(HybridCloudSecurity PRIVATE SEAL::seal)
