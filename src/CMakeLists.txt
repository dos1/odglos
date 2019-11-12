set(EXECUTABLE_SRC_LIST "main.c")
set(SHARED_SRC_LIST "common.c")

include(libsuperderpy-src)
target_link_libraries(libodglos "-lwebpdemux")
