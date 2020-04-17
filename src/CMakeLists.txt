set(EXECUTABLE_SRC_LIST "main.c")
set(SHARED_SRC_LIST "animation.c" "audio.c" "common.c" "download.c" "player.c" "scenes.c")

include_directories(SYSTEM ${WEBP_INCLUDE_DIRS} ${WEBPDEMUX_INCLUDE_DIRS})

include(libsuperderpy-src)
if (NOT EMSCRIPTEN)
  target_link_libraries(libodglos ${WEBP_LIBRARIES})
endif()
target_link_libraries(libodglos ${WEBPDEMUX_LIBRARIES})
