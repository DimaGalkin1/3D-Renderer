#include "rendering.hpp"

ThreeDL::Renderer::Renderer(SDL_Renderer* renderer, SDL_Window* window, Camera& camera, int width, int height)
    : renderer_(renderer),
      window_(window),
      camera_(camera),
      width_(width),
      height_(height),
      zbuffer_(width * height)
{}

void ThreeDL::Renderer::add(Object* object) {
    render_queue_.push_back(object);
}

void ThreeDL::Renderer::main_loop() {
    process_keys();
    render();
}

////// DEBUG //////

void ThreeDL::Renderer::track_keys(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        keys_[event.key.keysym.sym] = true;
    } else if (event.type == SDL_KEYUP) {
        keys_[event.key.keysym.sym] = false;
    }
}

void ThreeDL::Renderer::process_keys() {
    for (const auto& [key, pressed] : keys_) {
        if (!pressed) continue;

        switch (key) {
            case SDLK_w: camera_.move_forward(-0.1); break;
            case SDLK_s: camera_.move_forward(0.1); break;
            case SDLK_a: camera_.move_right(0.1); break;
            case SDLK_d: camera_.move_right(-0.1); break;
            case SDLK_LEFT: camera_.pan(0.5); break;
            case SDLK_RIGHT: camera_.pan(-0.5); break;
            case SDLK_UP: camera_.tilt(0.5); break;
            case SDLK_DOWN: camera_.tilt(-0.5); break;
            // case SDLK_q: camera_.roll(-0.5); break;
            // case SDLK_e: camera_.roll(0.5); break;
        }
    }

    camera_.calculate_dirs();
}

////// END DEBUG //////

void ThreeDL::Renderer::putpixel(int x, int y, const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer_, x, y);
}

void ThreeDL::Renderer::clear(const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer_);
}

void ThreeDL::Renderer::draw_line(const Line& line, const SDL_Color& color) {
    int dx = abs(line.b.x - line.a.x);
    int sx = line.a.x < line.b.x ? +1 : -1;
    int dy = -abs(line.b.y - line.a.y);
    int sy = line.a.y < line.b.y ? +1 : -1;
    int e = dx + dy;
    
	int x = line.a.x;
	int y = line.a.y;
	
    while (1) {
        if (x >= 0 && x < width_ && y >= 0 && y < height_)
            putpixel(x, y, color);

        if (x == line.b.x && y == line.b.y) break;
        int e2 = 2 * e;
        if (e2 >= dy) {
            if (x == static_cast<int>(line.b.x)) break;
            e += dy;
            x += sx;
        }
        if (e2 <= dx) {
            if (y == static_cast<int>(line.b.y)) break;
            e += dx;
            y += sy;
        }
    }
}

void ThreeDL::Renderer::render_object(const Object& object) {
    for (const auto& triangle : object.mesh_.triangles_) {
        GSPTriangle copy = triangle;
        //copy.translate(object.position_);
        render_triangle(copy, object.mesh_.texture_);
    }
}

void ThreeDL::Renderer::render_triangle(const GSPTriangle& triangle, SDL_Surface* texture) {
    GSPTriangle copy = triangle;
    copy.translate(camera_.position_);
    copy.rotate(camera_.rotation_);

    std::vector<ThreeDL::GSPTriangle> clipped_triangles = clip_triangle(copy);

    for (const auto& clipped : clipped_triangles) {
        SSPTriangle projected = project(clipped);
        rasterise_triangle(projected, texture);
    }
}

void ThreeDL::Renderer::rasterise_triangle(const SSPTriangle& triangle_g, SDL_Surface* texture) {
    SSPTriangle triangle = triangle_g;
    
    std::sort(std::begin(triangle.vertices_), std::end(triangle.vertices_), [](const Vec2& a, const Vec2& b) {
        return a.y > b.y;
    });

    std::sort(std::begin(triangle.uvs_), std::end(triangle.uvs_), [](const Vec2& a, const Vec2& b) {
        return a.y > b.y;
    });

    int y_max = triangle.vertices_[0].y;
    int y_min = triangle.vertices_[2].y;
    int y_mid = triangle.vertices_[1].y;

    Vec2 horiz_line_dir = {1, 0};
    Vec2 line_one = triangle.vertices_[2] - triangle.vertices_[0];
    Vec2 line_two = triangle.vertices_[1] - triangle.vertices_[0];
    Vec2 on_line = triangle.vertices_[0];

    Line line = {triangle.vertices_[0], triangle.vertices_[2]};
    Line line2 = {triangle.vertices_[1], triangle.vertices_[0]};

    for (int i = y_max; i > y_min; --i) {
        if (static_cast<int>(y_mid) == i) {
            line_two = triangle.vertices_[2] - triangle.vertices_[1];
            on_line = triangle.vertices_[2];
            line2 = {triangle.vertices_[2], triangle.vertices_[1]};
        }

        Vec2 intersect_one = ThreeDL::vec2_intersection({0, static_cast<double>(i)}, horiz_line_dir, on_line, line_one);
        intersect_one.depth_info_ = ThreeDL::calculate_z_index(line, intersect_one.x, intersect_one.y);
        Vec2 intersect_two = ThreeDL::vec2_intersection({0, static_cast<double>(i)}, horiz_line_dir, on_line, line_two);
        intersect_two.depth_info_ = ThreeDL::calculate_z_index(line2, intersect_two.x, intersect_two.y);

        int y = intersect_one.y;
        int x_max;
        int x_min;

        if (intersect_one.x > intersect_two.x) {
            x_max = intersect_one.x;
            x_min = intersect_two.x;
        } else {
            x_max = intersect_two.x;
            x_min = intersect_one.x;
        }

        Line ln = {intersect_one, intersect_two};

        for (int j = x_min; j < x_max; ++j) {
            double z = ThreeDL::calculate_z_index(ln, j, y);
            if (j >= 0 && j < width_ && y >= 0 && y < height_ && z > (zbuffer_)[y * width_ + j]) {
                zbuffer_.at(y * width_ + j) = z;
                putpixel(j, y, {255, 255, 255, 255});
            }
        }
    }
}

std::vector<ThreeDL::GSPTriangle> ThreeDL::Renderer::clip_to_plane(const GSPTriangle& triangle, const Plane& plane, bool side) {
    std::vector<GSPTriangle> clipped;

    ThreeDL::Vec3 ba = triangle.vertices_[1] - triangle.vertices_[0];
    ThreeDL::Vec3 cb = triangle.vertices_[2] - triangle.vertices_[1];
    ThreeDL::Vec3 ac = triangle.vertices_[0] - triangle.vertices_[2];

    ThreeDL::Intersect intersect_ba = ba.intersects(plane, triangle.vertices_[0]);
    ThreeDL::Intersect intersect_cb = cb.intersects(plane, triangle.vertices_[1]);
    ThreeDL::Intersect intersect_ac = ac.intersects(plane, triangle.vertices_[2]);

    if (!intersect_ba.intersects_ && !intersect_cb.intersects_ && !intersect_ac.intersects_) {
        bool onside = plane.normal_.dot(triangle.vertices_[0] - plane.position_) > 0;
        bool inside = (side) ? onside : !onside;

        if (inside) {
            clipped.push_back(triangle);
        }

        return clipped;
    }

    if (intersect_ba.intersects_ && intersect_cb.intersects_ && intersect_ac.intersects_) {
        bool onside = plane.normal_.dot(triangle.vertices_[0] - plane.position_) > 0;
        bool inside = (side) ? onside : !onside;

        if (inside) {
            clipped.push_back(triangle);
        }

        return clipped;
    }

    if (intersect_ba.intersects_ && intersect_cb.intersects_) {
        bool onside = plane.normal_.dot(triangle.vertices_[1] - plane.position_) > 0;
        bool inside = (side) ? onside : !onside;

        if (inside) {
            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[1], intersect_cb.point_, intersect_ba.point_},
                triangle.uvs_
            });
        } else {
            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[0], triangle.vertices_[2], intersect_cb.point_},
                triangle.uvs_
            });

            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[0], intersect_ba.point_, intersect_cb.point_},
                triangle.uvs_
            });
        }

        return clipped;
    }

    if (intersect_cb.intersects_ && intersect_ac.intersects_) {
        bool onside = plane.normal_.dot(triangle.vertices_[2] - plane.position_) > 0;
        bool inside = (side) ? onside : !onside;

        if (inside) {
            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[2], intersect_cb.point_, intersect_ac.point_},
                triangle.uvs_
            });
        } else {
            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[0], triangle.vertices_[1], intersect_cb.point_},
                triangle.uvs_
            });

            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[0], intersect_ac.point_, intersect_cb.point_},
                triangle.uvs_
            });
        }

        return clipped;
    }

    if (intersect_ac.intersects_ && intersect_ba.intersects_) {
        bool onside = plane.normal_.dot(triangle.vertices_[0] - plane.position_) > 0;
        bool inside = (side) ? onside : !onside;

        if (inside) {
            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[0], intersect_ba.point_, intersect_ac.point_},
                triangle.uvs_
            });
        } else {
            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[1], triangle.vertices_[2], intersect_ac.point_},
                triangle.uvs_
            });

            clipped.push_back({
                std::vector<Vec3> {triangle.vertices_[1], intersect_ba.point_, intersect_ac.point_},
                triangle.uvs_
            });
        }

        return clipped;
    }

    return clipped;
}

std::vector<ThreeDL::GSPTriangle> ThreeDL::Renderer::clip_to_plane(const std::vector<GSPTriangle>& triangles, const Plane& plane, bool side) {
    std::vector<GSPTriangle> clipped;

    for (const auto& triangle : triangles) {
        std::vector<GSPTriangle> clipped_triangles = clip_to_plane(triangle, plane, side);
        clipped.insert(clipped.end(), clipped_triangles.begin(), clipped_triangles.end());
    }

    return clipped;
}

std::vector<ThreeDL::GSPTriangle> ThreeDL::Renderer::clip_triangle(const GSPTriangle& triangle) {
    Plane near_plane = {
        {0, 0, -0.01},
        {0, 0, -1},
        {1, 0, 0}
    };

    Vec3 side = {0, 0, 1};
    side.rotate(0, -hf_fov_, 0);
    Vec3 norm = side;
    norm.rotate(0, -90, 0);

    Plane left_plane = {
        {0, 0, 0},
        norm,
        side
    };

    side = {0, 0, 1};
    side.rotate(0, hf_fov_, 0);
    norm = side;
    norm.rotate(0, 90, 0);

    Plane right_plane = {
        {0, 0, 0},
        norm,
        side
    };

    std::vector<ThreeDL::GSPTriangle> near_clipped = clip_to_plane(triangle, near_plane, true);
    std::vector<ThreeDL::GSPTriangle> left_clipped = clip_to_plane(near_clipped, left_plane, true);
    std::vector<ThreeDL::GSPTriangle> right_clipped = clip_to_plane(left_clipped, right_plane, true);

    return right_clipped;
}

ThreeDL::SSPTriangle ThreeDL::Renderer::project(const GSPTriangle& triangle) {
    std::vector<Vec2> vertices;
    
    for (const auto& vertex : triangle.vertices_) {
        Vec3 o = {0, 0, 0};
        Vec3 d = vertex;

        double dtp = (static_cast<double>(width_) / 2) / 0.73205080757;

        double t = (dtp - o.z) / d.z;

        double x = (o.x + t * d.x) + (static_cast<double>(width_) / 2);
        double y = (o.y + t * d.y )+ (static_cast<double>(height_) / 2);

        vertices.push_back(ThreeDL::Vec2{x, y});
    }

    return {vertices, triangle.uvs_};
}

void ThreeDL::Renderer::render() {
    clear({0, 0, 0, 255});

    for (const auto& object : render_queue_) {
        render_object(*object);
    }

    SDL_RenderPresent(renderer_);

    for (int i = 0; i < width_ * height_; i++) {
        zbuffer_[i] = -INFINITY;
    }
}

ThreeDL::Renderer::~Renderer() {
    for (const auto& object : render_queue_) {
        if (object == nullptr) continue;
        if (object->mesh_.texture_ == nullptr) continue;
        SDL_FreeSurface(object->mesh_.texture_);
    }   
}