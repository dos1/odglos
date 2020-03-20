cmake_minimum_required(VERSION 3.6)

project(odglos C)

SET(LIBSUPERDERPY_GAMENAME "odglos" CACHE INTERNAL "")
SET(LIBSUPERDERPY_GAMENAME_PRETTY "ODGŁOS" CACHE INTERNAL "")

SET(LIBSUPERDERPY_VENDOR "Holy Pangolin")
SET(LIBSUPERDERPY_APPID "com.holypangolin.ODGLOS")
SET(LIBSUPERDERPY_ORIENTATION "LANDSCAPE")
SET(LIBSUPERDERPY_VERSION "1.0")

if (EMSCRIPTEN OR ANDROID)
	set(IMGTOWEBP_QUALITY "80" CACHE INTERNAL "")
	set(IMGTOWEBP_PARAMS "-define webp:image-hint=photo" CACHE INTERNAL "")
endif(EMSCRIPTEN OR ANDROID)

set(EMSCRIPTEN_TOTAL_MEMORY "256" CACHE INTERNAL "")

if (NOT EMSCRIPTEN)
	set(LIBSUPERDERPY_IMGUI ON CACHE INTERNAL "")
endif (NOT EMSCRIPTEN)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/libsuperderpy/cmake")

include(libsuperderpy)

find_package(WebP)
find_package(WebPDemux)

add_subdirectory(libsuperderpy)
add_subdirectory(src)
add_subdirectory(data)
