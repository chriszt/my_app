project (my_add_lib C)

set (SRC host/my_add_lib_ca.c)

# Add by YL
link_directories(../../out-br/target/usr/lib)

add_executable (${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
			   PRIVATE ta/include
			   PRIVATE include
			   PRIVATE libs/algo/include)  # Add by YL

target_link_libraries (${PROJECT_NAME} PRIVATE teec
                                       PRIVATE algo)  # Add by YL

install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
