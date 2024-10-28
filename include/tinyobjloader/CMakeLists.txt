add_library(tinyobjloader INTERFACE)
#add_library(tinyobjloader::tinyobjloader ALIAS tinyobjloader)

target_sources(tinyobjloader INTERFACE ${CMAKE_CURRENT_LIST_DIR}/tiny_obj_loader.h)


