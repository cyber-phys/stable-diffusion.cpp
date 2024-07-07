set(TARGET addy)

set(SOURCES
    main.cpp
    kitty_image.cpp
)

add_executable(${TARGET} ${SOURCES})
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE stable-diffusion ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PUBLIC cxx_std_11)
