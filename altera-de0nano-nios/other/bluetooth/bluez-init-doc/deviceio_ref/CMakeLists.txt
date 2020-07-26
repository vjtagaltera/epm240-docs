cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

PROJECT (bluezapp_test)

message(${PROJECT_SOURCE_DIR})
message(${PROJECT_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/bluezlib/include)
set(CMAKE_INSTALL_STAGING ${PROJECT_BINARY_DIR}/bin-install-staging)
set(CPU_ARCH aarch64)
set(CMAKE_C_COMPILER ${PROJECT_SOURCE_DIR}/../prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER ${PROJECT_SOURCE_DIR}/../prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++ CACHE PATH "" FORCE)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/bluezlib/lib64/bluez)
##include_directories(${PROJECT_SOURCE_DIR}/../buildroot/output/rockchip_rk1808/host/aarch64-buildroot-linux-gnu/sysroot/usr/include)
link_directories(${PROJECT_SOURCE_DIR}/../buildroot/output/rockchip_rk1808/host/aarch64-buildroot-linux-gnu/sysroot/usr/lib)


add_definitions("-Wall -g")

add_subdirectory("bluezlib")
add_subdirectory("test")


