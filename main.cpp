#include <iostream>

#include "engine/rendering.hpp"
#include "engine/objects.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <chrono>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

ThreeDL::OBJLoader plane ("plane.obj", SDL_Color {255, 0 , 0});
ThreeDL::Object plane_obj (plane.export_mesh());

ThreeDL::Camera cam ({0, 0, 0}, {0, 0, 0});

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    ThreeDL::Renderer scene (renderer, window, cam, WINDOW_WIDTH, WINDOW_HEIGHT);

    scene.add(&plane_obj);

    while (true) {
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) {
            break;
        }

        scene.main_loop();
        scene.track_keys(event);
    }
}