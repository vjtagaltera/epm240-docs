cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

set(SRC_FILES
	simplescan.c
	secondsrc.c
)

set(SVR_FILES
	rfcomm-server.c
	secondsrc.c
)

if(BLUEZ)
add_definitions(-DBLUEZ_USE)
elseif(BSA)
add_definitions(-DBSA_USE)
endif()

add_definitions("-fpermissive")

set(TARGET_BINARY bluezapp_test)
set(TARGET_DEPEND ${SRC_FILES})
#set(TARGET_BINARY bluezapp_serv)
#set(TARGET_DEPEND ${SVR_FILES})

add_executable(${TARGET_BINARY} ${TARGET_DEPEND})

target_include_directories(${TARGET_BINARY} PUBLIC "${bluezapp_test_SOURCE_DIR}/bluezlib/include" )
target_include_directories(${TARGET_BINARY} PUBLIC "${PROJECT_SOURCE_DIR}/../buildroot/output/rockchip_rk1808/host/aarch64-buildroot-linux-gnu/sysroot/usr/include")
target_link_libraries(${TARGET_BINARY} pthread bluetooth )

set_target_properties(${TARGET_BINARY} PROPERTIES LINK_FLAGS "--sysroot ${PROJECT_SOURCE_DIR}/../buildroot/output/rockchip_rk1808/host/aarch64-buildroot-linux-gnu/sysroot")

install(TARGETS ${TARGET_BINARY} DESTINATION bin)


