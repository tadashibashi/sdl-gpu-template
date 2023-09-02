#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>
#include <SDL_gpu.h>

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

int main()
{
    // Initialize graphics and window
    auto window = GPU_Init(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window)
    {
        spdlog::error("Failed to initialize SDL_gpu {}", GPU_PopErrorCode().details);
        return 1;
    }
    atexit(GPU_Quit);

    // Create entities
    entt::registry reg;

    for (int i = 0; i < 100; ++i)
    {
        auto e = reg.create();
        reg.emplace<Position>(e,
            arc4random() % SCREEN_WIDTH,
            arc4random() % SCREEN_HEIGHT);
        reg.emplace<SDL_Color>(e,
           (uint8_t)(arc4random() % 256),
           (uint8_t)(arc4random() % 256),
           (uint8_t)(arc4random() % 256),
           255);
        reg.emplace<Velocity>(e,
                              (float)arc4random() / UINT32_MAX * 4 - 2,
                              (float)arc4random() / UINT32_MAX * 4 - 2);
        reg.emplace<Size>(e, 10, 10);
        reg.emplace<Scale>(e, 1, 1);
    }


    bool isRunning = true;
    while (isRunning)
    {
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
        for (auto [e, pos, vel, size] : reg.view<Position, Velocity, Size>().each())
        {
            pos.x += vel.x;
            pos.y += vel.y;

            while (pos.x <  -size.x)
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
        for (auto [e, pos, size, color, scale] : reg.view<Position, Size, SDL_Color, Scale>().each())
        {
            GPU_Rectangle2(window, GPU_Rect{pos.x, pos.y, (float)size.x, (float)size.y}, color);
        }

        GPU_Flip(window);
    }

    GPU_FreeTarget(window);
    GPU_Quit();

    return 0;
}
