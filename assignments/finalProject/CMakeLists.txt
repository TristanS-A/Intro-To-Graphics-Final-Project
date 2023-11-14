#Transformations example

file(
 GLOB_RECURSE ASSIGNMENT7_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENT7_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this finalProject's asset folder to bin when it is built
add_custom_target(copyAssetsFP ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENT7_INC} DESTINATION include/finalProject)
add_executable(finalProject ${ASSIGNMENT7_SRC} ${ASSIGNMENT7_INC} ${ASSIGNMENT7_ASSETS})
target_link_libraries(finalProject PUBLIC core IMGUI)
target_include_directories(finalProject PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when finalProject is built
add_dependencies(finalProject copyAssetsFP)