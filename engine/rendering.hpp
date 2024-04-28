#pragma once

#include <SDL2/SDL.h>

#include <algorithm>
//#include <SDL2/SDL_image.h>
#include <unordered_map>

#include "camera.hpp"
#include "objects.hpp"
#include "utils.hpp"

namespace ThreeDL {
    class Renderer {
        public:
            Renderer(SDL_Renderer* renderer, SDL_Window* window, Camera& camera, int width, int height);
            Renderer() = delete;

            // debug
            void track_keys(const SDL_Event& event);
            void process_keys();
            std::unordered_map<SDL_Keycode, bool> keys_;
            // end debug

            void add(Object* object);
            void main_loop();

            ~Renderer();
        private:
            SDL_Renderer* renderer_;
            SDL_Window* window_;

            Camera& camera_;

            const double tan_theta_2_ = 0.73205080757;
            const double hf_fov_ = 36.2060231;

            int width_;
            int height_;

            std::vector<double> zbuffer_;
            std::vector<Object*> render_queue_;

            // utils
            void putpixel(int x, int y, const SDL_Color& color);
            void draw_line(const Line& line, const SDL_Color& color);
            void clear(const SDL_Color& color);

            // rendering functions
            void render_object(const Object& object);
            void render_triangle(const GSPTriangle& triangle, SDL_Surface* texture);
            void rasterise_triangle(const SSPTriangle& triangle, SDL_Surface* texture);
            std::vector<GSPTriangle> clip_to_plane(const GSPTriangle& triangle, const Plane& plane, bool side);
            std::vector<GSPTriangle> clip_to_plane(const std::vector<GSPTriangle>& triangle, const Plane& plane, bool side);
            std::vector<GSPTriangle> clip_triangle(const GSPTriangle& triangle);
            SSPTriangle project(const GSPTriangle& triangle);

            void render();
    };
};