file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_executable(vk_test ${SOURCES})

if (BUILD_FOR_X11)
  target_link_libraries(vk_test PUBLIC glm tinyobjloader glfw vulkan shaderc imgui stb dl pthread X11 Xxf86vm Xrandr Xi)
else()
  target_link_libraries(vk_test PUBLIC glm tinyobjloader glfw vulkan shaderc imgui stb)
endif()


#target_link_libraries(vk_test PRIVATE glm::glm-header-only tinyobjloader::tinyobjloader)
  
