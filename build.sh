#!/bin/sh

LVGL_INCLUDE_PATH="./lvgl"
LVGL_LIBRARY_PATH="./lvgl/build/lib"

gcc \
  main.c ch36x_interface.c utils.c callbacks.c roboto_mono.c lib_for_user_space/ch36x_lib.c\
  -o ch36x_interface \
  $(pkg-config --cflags --libs sdl2-compat) \
  -I${LVGL_INCLUDE_PATH} \
  -L${LVGL_LIBRARY_PATH} \
  -llvgl -lm \
  -g \
  -D DEBUG

echo "Сборка завершена."
