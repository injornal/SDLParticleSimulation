#include <SDL.h>
#include <SDL_image.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "SDL_keycode.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_surface.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct vec2 {
    float x;
    float y;
    vec2(float x, float y) : x(x), y(y) {};

    vec2 lnormal() { return vec2(-y, x); }
    vec2 rnormal() { return vec2(y, -x); }

    vec2 rotate(float angle) {
        return vec2(this->x * cos(angle) + this->y * -sin(angle),
                    this->x * sin(angle) + this->y * cos(angle));
    }

    const float operator*(const vec2 &rhv) {
        return this->x * rhv.x + this->y * rhv.y;
    }
};

class Circle {
    SDL_Rect rect;
    int radius;
    vec2 velocity;
    float gravity;
    SDL_Texture *texture;

   public:
    Circle(int x, int y, int r, SDL_Renderer *renderer)
        : velocity(-5 + rand() % 10, -5 + rand() % 10) {
        this->rect.x = x - r;
        this->rect.y = y - r;
        this->rect.w = 2 * r;
        this->rect.h = 2 * r;
        this->radius = r;
        this->gravity = 0.1;
        this->texture = IMG_LoadTexture(renderer, "textures/circle.png");
    }

    void update(Circle *circles, unsigned int size, unsigned int offset) {
        this->rect.x += this->velocity.x;
        this->rect.y += this->velocity.y;

        for (int i = offset; i < size; i++) {
            Circle *circle = &circles[i];
            vec2 diff = vec2(circle->rect.x - this->rect.x,
                             circle->rect.y - this->rect.y);
            float dist = std::sqrt(pow(diff.x, 2) + pow(diff.y, 2));
            if (dist < circle->radius + this->radius) {
                if (dist == 0) {
                    this->rect.x += this->radius + circle->radius;
                    return;
                }

                assert(dist != 0);
                diff.x /= dist;
                diff.y /= dist;

                circle->rect.x += diff.x * (2 * circle->radius - dist) / 2;
                circle->rect.y += diff.y * (2 * circle->radius - dist) / 2;

                this->rect.x -= diff.x * (2 * this->radius - dist) / 2;
                this->rect.y -= diff.y * (2 * this->radius - dist) / 2;

                float angle = acos(diff.x);
                if (diff.y < 0) angle = -angle;

                vec2 transformedThisVelocity = this->velocity.rotate(angle);
                vec2 transformedCircleVelocity = circle->velocity.rotate(angle);
                std::swap(transformedThisVelocity.x,
                          transformedCircleVelocity.x);
                this->velocity = transformedThisVelocity.rotate(-angle);
                circle->velocity = transformedCircleVelocity.rotate(-angle);
            }
        }

        if (this->rect.x < 0) {
            this->rect.x = 0;
            this->velocity.x = -this->velocity.x;
        } else if (this->rect.x + this->rect.w > SCREEN_WIDTH) {
            this->rect.x = SCREEN_WIDTH - this->rect.w;
            this->velocity.x = -this->velocity.x;
        }
        if (this->rect.y < 0) {
            this->rect.y = 0;
            this->velocity.y = -this->velocity.y;
        } else if (this->rect.y + this->rect.h > SCREEN_HEIGHT) {
            this->rect.y = SCREEN_HEIGHT - this->rect.h;
            this->velocity.y = -this->velocity.y;
        }

        this->velocity.y += this->gravity;
    }

    void render(SDL_Renderer *renderer) {
        SDL_RenderCopy(renderer, this->texture, NULL, &this->rect);
    }
};

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("Shooter 01", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                          SCREEN_HEIGHT, 0);

    if (!window) {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH,
               SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    std::vector<Circle> circles;
    int n = 5;
    for (int i = 0; i < n; i++) {
        circles.push_back(Circle(10 + i * (SCREEN_WIDTH - 20) / n,
                                 10 + rand() % 200, 50, renderer));
    }
    // circles.push_back(Circle(100, 100, 50, renderer));
    // circles.push_back(Circle(300, 100, 50, renderer));
    while (true) {
        SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
        SDL_RenderClear(renderer);

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(0);
                    break;

                default:
                    break;
            }
        }

        for (int i = 0; i < circles.size(); i++) {
            circles[i].update(circles.data(), circles.size(), i + 1);
        }
        for (int i = 0; i < circles.size(); i++) {
            circles[i].render(renderer);
        }
        SDL_RenderPresent(renderer);
    }
}
