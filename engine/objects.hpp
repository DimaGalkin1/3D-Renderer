#pragma once

#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.hpp"

namespace ThreeDL {
    class Mesh {
        public:
            explicit Mesh(const Mesh& other);
            Mesh(std::vector<GSPTriangle> triangles, SDL_Surface* tex);
            Mesh() = delete;

            std::vector<GSPTriangle> triangles_;
            SDL_Surface* texture_;

            ~Mesh() = default;
    };

    class Object {
        public:
            explicit Object(const Mesh& mesh);
            Object() = delete;

            Vec3 position_;
            Vec3 rotation_;

            Mesh mesh_;

            ~Object() = default;
    };

    class OBJLoader {
        public:
            OBJLoader(const std::string& model_path, const std::string& texture_path);
            OBJLoader(const std::string& filename, const SDL_Color& color);
            OBJLoader() = delete;

            void load_model();
            void load_texture();
            
            Mesh export_mesh();

            ~OBJLoader();

            std::vector<GSPTriangle> triangles_;
        private:
            
            SDL_Color color_;
            SDL_Surface* texture_data_;
            
            std::string model_path_;
            std::string texture_path_;

            bool textured_ = false;
            bool exported_ = false;
    };
};