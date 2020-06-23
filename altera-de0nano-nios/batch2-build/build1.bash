#!/bin/bash
# build1.bash   the command dumped from altera fpga monitor c build log


nios2-elf-gcc -g -O0 -ffunction-sections -fverbose-asm -fno-inline -mno-cache-volatile \
    -mhw-mul -mno-hw-div \
    -I "..././16.1/nios2eds/components/altera_nios2/HAL/inc" \
    -I "C:/Users/altera/nios2/apps" \
    -DSYSTEM_BUS_WIDTH=32 -DALT_SINGLE_THREADED -D_JTAG_UART_BASE=8224u \
    -Wl,--defsym -Wl,nasys_stack_top=0x1000 \
    -Wl,--defsym -Wl,nasys_program_mem=0x0 -Wl,--defsym -Wl,nasys_data_mem=0x0 \
    -Wl,--section-start -Wl,.exceptions=0x20 \
    -Wl,--section-start -Wl,.reset=0x0 \
    -Wl,--script="..././16.1/University_Program/Monitor_Program/build/nios_cpp_build.ld" \
    -o "C:/Users/altera/nios2/apps/lightsapp.elf" \
    "C:/Users/altera/nios2/apps/lightsapp.c" \
    "..././16.1/University_Program/Monitor_Program/lib/jtag_uart.c" 


#nios2-elf-objcopy -O srec "C:/Users/altera/nios2/apps/lightsapp.elf" \
#                          "C:/Users/altera/nios2/apps/lightsapp.srec" 


