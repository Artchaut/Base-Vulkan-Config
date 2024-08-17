file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_executable(vk_test ${SOURCES})

target_link_libraries(vk_test PUBLIC glm SDL2 SDL2main vulkan shaderc imgui)
  
