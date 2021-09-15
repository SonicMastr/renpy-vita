cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "VitaSDK toolchain file")
set(VITASDK_CMAKE_FILE "$ENV{VITASDK}/share/vita.cmake" CACHE PATH "VitaSDK CMake functions file")
include("${VITASDK_CMAKE_FILE}" REQUIRED)

project(Renpy LANGUAGES C)

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")

add_compile_definitions(__psp2__)

add_compile_options(
  -O2 -std=gnu99  # This O2 literally won't do anything for official builds btw
  -w
  -fsingle-precision-constant -fno-common
)

include_directories(
    "include"
    "include/module"
    "include/module/src"
    "include/module/fribidi-src/lib"
    "$ENV{VITASDK}/arm-vita-eabi/include/python2.7"
    "$ENV{VITASDK}/arm-vita-eabi/include/SDL2"
    "$ENV{VITASDK}/arm-vita-eabi/include/freetype2"
)

# Build self

set(ELF "eboot.elf")
set(SELF "eboot.bin")

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE simple_SRC
    source/*.c
)

add_executable("${ELF}"
    ${simple_SRC}
)

target_link_libraries("${ELF}"
  python2.7
  SDL2_mixer
  SDL2_image
  SDL2
  taihen_stub
  libgpu_es4_ext_stub_weak
  libIMGEGL_stub_weak
  libGLESv2_stub_weak
  SceAppMgr_stub
  SceProcessmgr_stub
  SceIme_stub_weak
  vorbisfile
  vorbis
  webp
  ogg
  mpg123
  FLAC
  mikmod
  opus
  avformat
  avfilter
  swscale
  avutil
  avcodec
  swresample
  mp3lame
  freetype
  jpeg
  png
  SceMotion_stub
  SceCommonDialog_stub
  SceHid_stub
  SceGxm_stub
  SceAudio_stub
  ScePower_stub
  SceDisplay_stub
  SceCtrl_stub
  SceTouch_stub
  SceIofilemgr_stub
  SceNet_stub
  SceNetCtl_stub
  SceSsl_stub
  SceVshBridge_stub
  SceSysmodule_stub
  pthread
  ${SDL2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  gcc
  z
  m
)

vita_create_self("${SELF}"
  "${ELF}"
  UNSAFE
  NOASLR
)

add_custom_command(TARGET ${SELF} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/eboot.bin
                ${CMAKE_CURRENT_SOURCE_DIR}/DistributionTool/build/eboot.bin)
