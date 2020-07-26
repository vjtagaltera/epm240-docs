cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(bluezlib)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/bluezlib.h DESTINATION ${CMAKE_INSTALL_STAGING}/usr/include/)

if(CPU_ARCH STREQUAL "arm")
endif()

if(CPU_ARCH STREQUAL "aarch64")
	message("CPU_ARCH is ${CPU_ARCH}")
    if(BLUEZ)
        message("BLUETOOTH_BRANCH is BLUEZ")
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib64/bluez/libDeviceIo.so DESTINATION ${CMAKE_INSTALL_STAGING}/usr/lib/)
    elseif(BSA)
        if(CYPRESS)
            message("BLUETOOTH_BRANCH is BSA:CYPRESS")
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib64/cypress/libDeviceIo.so DESTINATION ${CMAKE_INSTALL_STAGING}/usr/lib/)
        elseif(BROADCOM)
            message("BLUETOOTH_BRANCH is BSA:BROADCOM")
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib64/broadcom/libDeviceIo.so DESTINATION ${CMAKE_INSTALL_STAGING}/usr/lib/)
        endif()
    endif()
endif()

install(FILES ${CMAKE_INSTALL_STAGING}/usr/lib/libDeviceIo.so DESTINATION ${CMAKE_INSTALL_PREFIX}/lib RENAME libDeviceIo.so)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/DeviceIo DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


