#include "utils.hpp"

ThreeDL::Vec2::Vec2(double x_g, double y_g) 
    : x(x_g),
      y(y_g),
      depth_info_(0)
{}

ThreeDL::Vec2::Vec2(double x_g, double y_g, double depth_info) 
    : x(x_g),
      y(y_g),
      depth_info_(depth_info)
{}

void ThreeDL::Vec2::rotate(double angle, const Vec2& axis) {
    angle = angle * (M_PI / 180); // convert to radians

    double s = sin(angle);
    double c = cos(angle);

    x -= axis.x;
    y -= axis.y;

    double xnew = x * c - y * s;
    double ynew = x * s + y * c;

    x = xnew + axis.x;
    y = ynew + axis.y;
}

double ThreeDL::Vec2::dot(const Vec2& other) {
    return (x * other.x) + (y * other.y);
}

void ThreeDL::Vec2::normalise() {
    double length = sqrt((x * x) + (y * y));
    x /= length;
    y /= length;
}

ThreeDL::Vec2 ThreeDL::Vec2::operator+(const Vec2& other) const {
    return {
        x + other.x,
        y + other.y
    };
}

ThreeDL::Vec2 ThreeDL::Vec2::operator-(const Vec2& other) const {
    return {
        x - other.x,
        y - other.y
    };
}

ThreeDL::Vec2 ThreeDL::Vec2::operator*(double other) const {
    return {
        x * other,
        y * other
    };
}

ThreeDL::Vec2 ThreeDL::Vec2::operator/(double other) const {
    return {
        x / other,
        y / other
    };
}

void ThreeDL::Vec2::operator/=(double other) {
    x /= other;
    y /= other;
}

bool ThreeDL::Vec2::operator==(const Vec2& other) const {
    return (x == other.x) && (y == other.y);
}

bool ThreeDL::Vec2::operator!=(const Vec2& other) const {
    return (x != other.x) || (y != other.y);
}

ThreeDL::Vec3::Vec3(const Vec3& other) 
    : x(other.x),
      y(other.y),
      z(other.z)
{}

ThreeDL::Vec3::Vec3(double x_g, double y_g, double z_g) 
    : x(x_g),
      y(y_g),
      z(z_g)
{}

#include <iostream>

void ThreeDL::Vec3::rotate(double angle_x, double angle_y, double angle_z) {
    angle_x = angle_x * (M_PI / 180); // convert to radians
    angle_y = angle_y * (M_PI / 180); // convert to radians
    angle_z = angle_z * (M_PI / 180); // convert to radians

    double s_x = sin(angle_x);
    double c_x = cos(angle_x);

    double s_y = sin(angle_y);
    double c_y = cos(angle_y);

    double s_z = sin(angle_z);
    double c_z = cos(angle_z);

    double xnew = x * c_y * c_z - y * c_y * s_z + z * s_y;
    double ynew = x * (c_x * s_z + c_z * s_x * s_y) + y * (c_x * c_z - s_x * s_y * s_z) - z * c_y * s_x;
    double znew = x * (s_x * s_z - c_x * c_z * s_y) + y * (c_z * s_x + c_x * s_y * s_z) + z * c_x * c_y;

    x = xnew;
    y = ynew;
    z = znew;
}

double ThreeDL::Vec3::dot(const Vec3& other) const {
    return (x * other.x) + (y * other.y) + (z * other.z);
}

void ThreeDL::Vec3::normalise() {
    double length = sqrt((x * x) + (y * y) + (z * z));
    x /= length;
    y /= length;
    z /= length;
}

double ThreeDL::Vec3::distance(const Vec3& other) const {
    return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2));
}

double ThreeDL::Vec3::mag() const {
    return sqrt((x * x) + (y * y) + (z * z));
}

ThreeDL::Intersect ThreeDL::Vec3::intersects(const Plane& plane, const Vec3& ray_start) {
    double numerator = plane.normal_.dot(plane.position_) - plane.normal_.dot(ray_start);
    double denominator = plane.normal_.dot({x, y, z});

    if (denominator == 0) {
        return {
            false,
            {0, 0, 0},
            0
        };
    }

    double t = numerator / denominator;
    Vec3 pos = ray_start + (Vec3{x, y, z} * t);

    if (
        (pos.z > ray_start.z && pos.z > ray_start.z + z) ||
        (pos.z < ray_start.z && pos.z < ray_start.z + z) ||
        (pos.x > ray_start.x && pos.x > ray_start.x + x) ||
        (pos.x < ray_start.x && pos.x < ray_start.x + x) ||
        (pos.y > ray_start.y && pos.y > ray_start.y + y) ||
        (pos.y < ray_start.y && pos.y < ray_start.y + y)
    ) {
        return {
            false,
            pos,
            t
        };
    }

    return {
        true,
        pos,
        t
    };
}

ThreeDL::Vec3 ThreeDL::Vec3::cross(const Vec3& other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    };
}

ThreeDL::Vec3 ThreeDL::Vec3::operator+(const Vec3& other) const {
    return {
        x + other.x,
        y + other.y,
        z + other.z
    };
}

ThreeDL::Vec3 ThreeDL::Vec3::operator-(const Vec3& other) const {
    return {
        x - other.x,
        y - other.y,
        z - other.z
    };
}

ThreeDL::Vec3 ThreeDL::Vec3::operator*(double other) const {
    return {
        x * other,
        y * other,
        z * other
    };
}

ThreeDL::Vec3 ThreeDL::Vec3::operator/(double other) const {
    return {
        x / other,
        y / other,
        z / other
    };
}

void ThreeDL::Vec3::operator/=(double other) {
    x /= other;
    y /= other;
    z /= other;
}

bool ThreeDL::Vec3::operator==(const Vec3& other) const {
    return (x == other.x) && (y == other.y) && (z == other.z);
}

bool ThreeDL::Vec3::operator!=(const Vec3& other) const {
    return (x != other.x) || (y != other.y) || (z != other.z);
}

ThreeDL::Plane::Plane(const Vec3& position, const Vec3& normal) 
    : position_(position),
      normal_(normal)
{}

ThreeDL::Plane::Plane(const Vec3& position, const Vec3& normal, const Vec3& direction) 
    : position_(position),
      normal_(normal),
      direction_(direction)
{}

ThreeDL::Line::Line(const Vec2& start, const Vec2& end) 
    : a(start),
      b(end)
{}

ThreeDL::Intersect::Intersect(bool intersects, const Vec3& point, double t) 
    : point_(point),
      intersects_(intersects),
      t_(t)
{}

ThreeDL::SSPTriangle::SSPTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3) {
    vertices_[0] = v1;
    vertices_[1] = v2;
    vertices_[2] = v3;
}

ThreeDL::SSPTriangle::SSPTriangle(const std::vector<Vec2>& vertecies)
    : vertices_(vertecies)
{}

ThreeDL::SSPTriangle::SSPTriangle(const std::vector<Vec2>& vertecies, const std::vector<Vec2>& uvs)
    : vertices_(vertecies),
      uvs_(uvs)
{}

ThreeDL::GSPTriangle::GSPTriangle(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
    vertices_[0] = v1;
    vertices_[1] = v2;
    vertices_[2] = v3;
}

ThreeDL::GSPTriangle::GSPTriangle(const std::vector<Vec3>& vertecies)
    : vertices_(vertecies)
{}

ThreeDL::GSPTriangle::GSPTriangle(const std::vector<Vec3>& vertecies, const std::vector<Vec2>& uvs)
    : vertices_(vertecies),
      uvs_(uvs)
{}

void ThreeDL::GSPTriangle::rotate(const Vec3& rotation) {
    for (auto& vertex : vertices_) {
        vertex.rotate(rotation.x, rotation.y, rotation.z);
    }
}

void ThreeDL::GSPTriangle::translate(const Vec3& translation) {
    for (auto& vertex : vertices_) {
        vertex = vertex - translation;
    }
}

std::vector<std::string> ThreeDL::split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (str);
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
};

double ThreeDL::calculate_z_index(const Line& line, double x, double y) {
    if(line.a.depth_info_ == line.b.depth_info_) {
        return line.a.depth_info_;
    }

    double denom = line.b.x - line.a.x;
    double t;

    if (denom == 0) {
        denom = line.b.y - line.a.y;

        if (denom == 0) {
            return line.a.depth_info_;
        }

        t = (y - line.a.y) / denom;
    } else {
        t = (x - line.a.x) / denom;
    }

    return line.a.depth_info_ + t * (line.b.depth_info_ - line.a.depth_info_);
}

ThreeDL::Vec2 ThreeDL::vec2_intersection(const Vec2& a_pos, const Vec2& a_dir, const Vec2& b_pos, const Vec2& b_dir) {
    double t = (b_dir.x * (a_pos.y - b_pos.y) - b_dir.y * (a_pos.x - b_pos.x)) / (a_dir.x * b_dir.y - a_dir.y * b_dir.x);

    return a_pos + a_dir * t;
}