#include <exception>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <SDL_gpu.h>
#include <functional>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Position {
    float x, y;
};

struct Scale {
    float x, y;
};

struct Size {
    int x, y;
};

struct Velocity {
    float x, y;
};

bool isRunning = false;

struct Context {
    Context(GPU_Target *window, entt::registry &reg) : window(window), reg(reg) { }

    GPU_Target *window;
    entt::registry &reg;
};

void em_loop(void *ctx)
{
    auto &context = *reinterpret_cast<Context *>(ctx);
    auto window = context.window;
    auto &reg = context.reg;

    // Handle events
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    isRunning = false;
                break;
            default:
                break;
        }
    }

    // Update logic
    for (auto [e, pos, vel, size]: reg.view<Position, Velocity, Size>().each())
    {
        pos.x += vel.x;
        pos.y += vel.y;

        while (pos.x < -size.x)
            pos.x += SCREEN_WIDTH + size.x;
        while (pos.y < -size.y)
            pos.y += SCREEN_HEIGHT + size.y * 2;
        while (pos.x > SCREEN_WIDTH + size.x)
            pos.x -= SCREEN_WIDTH + size.x * 2;
        while (pos.y > SCREEN_HEIGHT + size.y)
            pos.y -= SCREEN_HEIGHT + size.y * 2;
    }

    // Render logic
    GPU_ClearColor(window, {10, 10, 10, 255});
    for (auto [e, pos, size, color, scale]: reg.view<Position, Size, SDL_Color, Scale>().each())
    {
        GPU_Rectangle2(window, GPU_Rect{pos.x, pos.y, (float) size.x, (float) size.y}, color);
    }

    GPU_Flip(window);
}


int main()
{
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Initialize graphics and window
    auto window = GPU_Init(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window)
    {
        spdlog::error("Failed to initialize SDL_gpu {}", GPU_PopErrorCode().details);
        return 1;
    }


    try {
        // Create entities
        entt::registry reg;

        for (int i = 0; i < 100; ++i)
        {
            auto e = reg.create();
            reg.emplace<Position>(e,
                random() % SCREEN_WIDTH,
                random() % SCREEN_HEIGHT);
            reg.emplace<SDL_Color>(e,
               (uint8_t)(random() % 256),
               (uint8_t)(random() % 256),
               (uint8_t)(random() % 256),
               255);
            reg.emplace<Velocity>(e,
                                  (float)random() / RAND_MAX * 4 - 2,
                                  (float)random() / RAND_MAX * 4 - 2);
            reg.emplace<Size>(e, 10, 10);
            reg.emplace<Scale>(e, 1, 1);
        }


        isRunning = true;

        Context ctx(window, reg);

#ifdef EMSCRIPTEN
        emscripten_set_main_loop_arg(em_loop, &ctx, -1, 1);
#else
        while (isRunning)
        {
            em_loop(&ctx);
        }

#endif

    }
    catch(const std::exception &e)
    {
        spdlog::error("Error occurred: {}", e.what());

        auto gpuError = GPU_PopErrorCode();
        if (gpuError.details)
            spdlog::error("SDL GPU error: {}: {}", gpuError.function, gpuError.details);

        GPU_FreeTarget(window);
        GPU_Quit();
    }
    catch(...)
    {
        spdlog::error("An error occurred: {}", GPU_PopErrorCode().details);

        auto gpuError = GPU_PopErrorCode();
        if (gpuError.details)
            spdlog::error("SDL GPU error: {}: {}", gpuError.function, gpuError.details);

        GPU_FreeTarget(window);
        GPU_Quit();
    }

    GPU_FreeTarget(window);
    GPU_Quit();

    return 0;
}
