cmake_minimum_required(VERSION 2.6)
project(projstm32)

ENABLE_LANGUAGE(ASM)

set( CMAKE_SYSTEM_NAME linux )
set( TOOLCHAIN_PREFIX arm-none-eabi- )
set( CMAKE_C_COMPILER   arm-none-eabi-gcc )
set( CMAKE_OBJCOPY   arm-none-eabi-objcopy )
set( CMAKE_ASM_COMPILER arm-none-eabi-gcc )
set( _CMAKE_TOOLCHAIN_PREFIX arm-none-eabi- )

SET(CMAKE_C_FLAGS "-mthumb -Os -fno-builtin -mcpu=cortex-m0 -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m0 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -g -mthumb -mcpu=cortex-m0 -mabi=aapcs -nostartfiles -T../arm-gcc-link.ld" CACHE INTERNAL "executable linker flags")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst  ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

add_definitions(-DSTM32F091)
add_definitions(-ggdb -gdwarf-2)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

include_directories( . ./cmsis_boot ./cmsis_boot/startup ./cmsis_core ./cmsis_lib/include ./cmsis_lib/source ./stm32_lib/inc ./src )

SET(INSTALL_TARGET_ID "" CACHE STRING "If set, install only on this targert ID")

IF( ${INSTALL_TARGET_ID} MATCHES "(.+)" )
	SET( STFLASH_OPT "--serial ${INSTALL_TARGET_ID}" )
ELSE()
	SET( STFLASH_OPT "" )
ENDIF()

add_definitions(-DSNET_V2)

SET( PROJECT_BIN ${CMAKE_PROJECT_NAME}  )
macro (add_sources)
  FILE(GLOB _src 
    "${PROJECT_SOURCE_DIR}/${ARGN}/*.h"
    "${PROJECT_SOURCE_DIR}/${ARGN}/*.c"
    "${PROJECT_SOURCE_DIR}/${ARGN}/*.s"
    )
  list (APPEND SRCS "${_src}")
endmacro()

add_sources( . )
add_sources( cmsis_boot )
add_sources( cmsis_boot/startup )
add_sources( cmsis_core )
add_sources( cmsis_lib/include )
add_sources( cmsis_lib/source )
add_sources( src )

add_executable(${PROJECT_BIN} ${SRCS} )
add_dependencies( ${PROJECT_BIN} version )

ADD_CUSTOM_TARGET(${PROJECT_BIN}.hex ALL DEPENDS ${PROJECT_BIN} COMMAND arm-none-eabi-objcopy --strip-unneeded -Oihex ${PROJECT_BINARY_DIR}/${PROJECT_BIN} ${PROJECT_BINARY_DIR}/${PROJECT_BIN}.hex)

#install(CODE "execute_process(COMMAND /usr/local/bin/st-flash ${STFLASH_OPT} --format ihex write ${PROJECT_BIN}.hex )")

SET( OPENOCD_ARG "-s /usr/share/openocd/scripts -f interface/stlink.cfg -f target/stm32f0x.cfg" )
SET( OPENOCD_CMD "${OPENOCD_CMD}adapter speed 2000" )

install(CODE "execute_process(COMMAND openocd ${OPENOCD_ARG} -d1 -c \"${OPENOCD_CMD}\" -c \"program ${PROJECT_BINARY_DIR}/${PROJECT_BIN}.hex reset exit\")")


ADD_CUSTOM_TARGET( gdbserver openocd
		-s "/usr/share/openocd/scripts"
		-f "interface/stlink.cfg" 
		-f "target/stm32f0x.cfg"
		-c "${OPENOCD_CMD}"
)
