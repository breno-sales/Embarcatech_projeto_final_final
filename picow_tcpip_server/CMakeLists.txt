# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)


project(picow_tcpip_server C CXX ASM)

set(WIFI_SSID "Rede_Familia")
set(WIFI_PASSWORD "25021990")

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(picow_tcpip_server_background
        picow_tcp_server.c
        )
target_compile_definitions(picow_tcpip_server_background PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(picow_tcpip_server_background PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(picow_tcpip_server_background
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        FreeRTOS-Kernel-Heap4 
        )

pico_enable_stdio_usb(picow_tcpip_server_background 1)
pico_enable_stdio_uart(picow_tcpip_server_background 0)


pico_add_extra_outputs(picow_tcpip_server_background)

# add_executable(picow_tcpip_server_poll
#         picow_tcp_server.c
#         )
# target_compile_definitions(picow_tcpip_server_poll PRIVATE
#         WIFI_SSID=\"${WIFI_SSID}\"
#         WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
#         )
# target_include_directories(picow_tcpip_server_poll PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
#         )
# target_link_libraries(picow_tcpip_server_poll
#         pico_cyw43_arch_lwip_poll
#         pico_stdlib
#         )



# pico_enable_stdio_usb(picow_tcpip_server_poll 1)
# pico_enable_stdio_uart(picow_tcpip_server_poll 0)

# pico_add_extra_outputs(picow_tcpip_server_poll)



