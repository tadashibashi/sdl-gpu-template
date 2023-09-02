add_subdirectory(sdl-gpu)

if (EMSCRIPTEN)
    set(GPU_CompileDefs
            SDL_GPU_DISABLE_OPENGL # use GLES3 by disabling the following...
            SDL_GPU_DISABLE_GLES_2
            SDL_GPU_DISABLE_GLES_1
            MIN_WEBGL_VERSION=2    # use WebGL 2
            MAX_WEBGL_VERSION=2)
    set_target_properties(SDL_gpu PROPERTIES
            COMPILE_DEFINITIONS "${GPU_CompileDefs}")
endif()
