#include "objects.hpp"

ThreeDL::Mesh::Mesh(std::vector<GSPTriangle> triangles, SDL_Surface* tex)
    : triangles_(triangles),
      texture_(tex)
{}

ThreeDL::Mesh::Mesh(const Mesh& other)
    : triangles_(other.triangles_),
      texture_(other.texture_)
{}

ThreeDL::Object::Object(const Mesh& mesh)
    : mesh_(mesh)
{}

ThreeDL::OBJLoader::OBJLoader(const std::string& model_path, const std::string& texture_path)
    : model_path_(model_path),
      texture_path_(texture_path)
{
    load_model();
    load_texture();
}

ThreeDL::OBJLoader::OBJLoader(const std::string& filename, const SDL_Color& color)
    : model_path_(filename),
      color_(color),
      texture_path_(""),
      textured_(false)
{
    load_model();
}

void ThreeDL::OBJLoader::load_model() {
    std::string line;

    std::ifstream vertex_data (model_path_);

    if (!vertex_data.is_open()) {
        throw std::runtime_error("Could not open OBJ file: " + model_path_);
        return;
    }

    // vertex & texture values for face indexing
    int v_value = 1;
    int t_value = 1;

    std::unordered_map<int, Vec3> vertices;
    std::unordered_map<int, Vec2> texture_coords;

    while (getline(vertex_data, line)) {
        if (line.empty()) continue;
        if (line[0] == '\r') continue;
        if (line[0] == '#') continue;

        if (line[0] == 'v' && line[1] == 't') {
            std::vector<std::string> tokens = split(line.substr(3), ' ');
            texture_coords.insert({
                t_value,
                Vec2({
                    std::stod(tokens[0]),
                    std::stod(tokens[1])
                })
            });

            ++t_value;
        } else if (line[0] == 'v') {
            std::vector<std::string> tokens = split(line.substr(2), ' ');
            vertices.insert(
                std::pair<int, Vec3>(
                    v_value,
                    Vec3({
                        std::stod(tokens[0]),
                        std::stod(tokens[1]),
                        std::stod(tokens[2])
                    })  
                )
            );

            ++v_value;
        } else if (line[0] == 'f') {
            if (textured_) {
                std::vector<std::string> tokens = split(line.substr(2), ' ');
                std::vector<std::string> first = split(tokens[0], '/');
                std::vector<std::string> second = split(tokens[1], '/');
                std::vector<std::string> third = split(tokens[2], '/');

                triangles_.emplace_back(
                    vertices[std::stoi(first[0])],
                    vertices[std::stoi(second[0])],
                    vertices[std::stoi(third[0])]
                );

                triangles_.back().uvs_ = {
                    texture_coords[std::stoi(first[1])],
                    texture_coords[std::stoi(second[1])],
                    texture_coords[std::stoi(third[1])]
                };
            } else {
                std::vector<std::string> tokens = split(line.substr(2), ' ');
                triangles_.emplace_back(
                    vertices[std::stoi(tokens[0])],
                    vertices[std::stoi(tokens[1])],
                    vertices[std::stoi(tokens[2])]
                );
            }
        }
    }
}

void ThreeDL::OBJLoader::load_texture() {
    texture_data_ = IMG_Load(texture_path_.c_str());

    if (texture_data_ == nullptr) {
        throw std::runtime_error("Could not load texture: " + texture_path_);
        return;
    }

    textured_ = true;
}

ThreeDL::Mesh ThreeDL::OBJLoader::export_mesh() {
    exported_ = true;
    return {triangles_, texture_data_};
}

ThreeDL::OBJLoader::~OBJLoader() {
    if (!exported_ && texture_data_ != nullptr) {
        SDL_FreeSurface(texture_data_);
    }
}