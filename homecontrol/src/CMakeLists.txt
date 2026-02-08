add_executable(${ProjectName}
    main.c
    variaveis_globais.c
    ssd1306.c
    functions.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4 
    hardware_i2c
    hardware_gpio
    hardware_timer
    )

pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 1)

pico_add_extra_outputs(${ProjectName})