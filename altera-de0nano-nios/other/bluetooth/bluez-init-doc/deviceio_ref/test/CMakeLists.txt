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

set(COMMON_CUSTOM_INC 
	"${PROJECT_SOURCE_DIR}/bluezlib/include" 
	"${PROJECT_SOURCE_DIR}/../buildroot/output/rockchip_rk1808/host/aarch64-buildroot-linux-gnu/sysroot/usr/include")
set(COMMON_CUSTOM_LFLAG
	"--sysroot ${PROJECT_SOURCE_DIR}/../buildroot/output/rockchip_rk1808/host/aarch64-buildroot-linux-gnu/sysroot")


add_executable(bluezapp_test ${SRC_FILES})
add_executable(bluezapp_serv ${SVR_FILES})

set(TARGET_BINARY bluezapp_test)
target_include_directories(${TARGET_BINARY} PUBLIC ${COMMON_CUSTOM_INC} )
target_link_libraries(${TARGET_BINARY} pthread bluetooth )
set_target_properties(${TARGET_BINARY} PROPERTIES LINK_FLAGS ${COMMON_CUSTOM_LFLAG} )

install(TARGETS ${TARGET_BINARY} DESTINATION bin)

set(TARGET_BINARY bluezapp_serv)
target_include_directories(${TARGET_BINARY} PUBLIC ${COMMON_CUSTOM_INC} )
target_link_libraries(${TARGET_BINARY} pthread bluetooth )
set_target_properties(${TARGET_BINARY} PROPERTIES LINK_FLAGS ${COMMON_CUSTOM_LFLAG} )

install(TARGETS ${TARGET_BINARY} DESTINATION bin)


