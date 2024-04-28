#pragma once

#define _USE_MATH_DEFINES // for intellisense
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

/*
* @namespace ThreeDL
* @brief A 3D library for software rendering with C++ & SDL2 
*/
namespace ThreeDL {
    /*
    * @class Vec2
    * @brief Represents a 2D vector and gives useful functions for manipulating it
    */
    class Vec2 {
        public:
            Vec2(double x_g, double y_g);
            Vec2(double x_g, double y_g, double depth_info);
            Vec2() = default;

            double x;
            double y;
            double depth_info_;

            void rotate(double angle, const Vec2& axis);
            double dot(const Vec2& other);
            void normalise();

            Vec2 operator+(const Vec2& other) const;
            Vec2 operator-(const Vec2& other) const;
            Vec2 operator*(double other) const;
            Vec2 operator/(double other) const;
            void operator/=(double other);
            bool operator==(const Vec2& other) const;
            bool operator!=(const Vec2& other) const;

            ~Vec2() = default;
    };

    class Intersect;
    class Plane;

    /*
    * @class Vec3
    * @brief Represents a 3D vector and gives useful functions for manipulating it
    */
    class Vec3 {
        public:
            Vec3(const Vec3& other); // no explicit
            Vec3(double x_g, double y_g, double z_g);
            Vec3() = default;

            double x;
            double y;
            double z;

            void rotate(double x_angle, double y_angle, double z_angle);
            void normalise();

            Intersect intersects(const Plane& plane, const Vec3& ray_start);
            
            Vec3 cross(const Vec3& other) const;
            double distance(const Vec3& other) const;
            double dot(const Vec3& other) const;
            double mag() const;

            Vec3 operator+(const Vec3& other) const;
            Vec3 operator-(const Vec3& other) const;
            Vec3 operator*(double other) const;
            Vec3 operator/(double other) const;
            void operator/=(double other);
            bool operator==(const Vec3& other) const;
            bool operator!=(const Vec3& other) const;

            ~Vec3() = default;
    };

    /*
    * @class Plane
    * @brief Describes a plane in 3D space
    */
    class Plane {
        public:
            Plane(const Vec3& position, const Vec3& normal);
            Plane(const Vec3& position, const Vec3& normal, const Vec3& direction);
            Plane() = delete;

            Vec3 position_;
            Vec3 direction_;
            Vec3 normal_;

            ~Plane() = default;
    };

    /*
    * @class Line
    * @brief Represents a two dimensional line
    */
    class Line {
        public:
            Line(const Vec2& start, const Vec2& end);
            Line() = delete;

            Vec2 a;
            Vec2 b;

            ~Line() = default;
    };

    /*
    * @class Intersect
    * @brief Gives information about an intersection
    */
    class Intersect {
        public:
            Intersect(bool intersects, const Vec3& point, double t);
            Intersect() = delete;

            Vec3 point_;
            bool intersects_;
            double t_;

            ~Intersect() = default;
    };

    /*
    * @class SSPTriangle
    * @brief Represents a triangle in screen space, allows for storage of UVs
    */
    class SSPTriangle {
        public:
            explicit SSPTriangle(const std::vector<Vec2>& vertecies);
            SSPTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3);
            SSPTriangle(const std::vector<Vec2>& vertecies, const std::vector<Vec2>& uvs);
            SSPTriangle() = default;

            std::vector<Vec2> vertices_ = {
                {0, 0},
                {0, 0},
                {0, 0}
            };

            std::vector<Vec2> uvs_ = {
                {0, 0},
                {0, 0},
                {0, 0}
            };
        
            ~SSPTriangle() = default;
    };

    /*
    * @class GSPTriangle
    * @brief Represents a triangle in global space, allows for storage of UVs
    */
    class GSPTriangle {
        public:
            explicit GSPTriangle(const std::vector<Vec3>& vertecies);
            GSPTriangle(const Vec3& v1, const Vec3& v2, const Vec3& v3);
            GSPTriangle(const std::vector<Vec3>& vertecies, const std::vector<Vec2>& uvs);
            GSPTriangle() = default;

            std::vector<Vec3> vertices_ = {
                {0, 0, 0},
                {0, 0, 0},
                {0, 0, 0}
            };
            
            std::vector<Vec2> uvs_ = {
                {0, 0},
                {0, 0},
                {0, 0}
            };

            void rotate(const Vec3& rotation);
            void translate(const Vec3& translation);
        
            ~GSPTriangle() = default;
    };

    std::vector<std::string> split(const std::string& str, char delim);
    double calculate_z_index(const Line& line, double x, double y);
    Vec2 vec2_intersection(const Vec2& a_pos, const Vec2& a_dir, const Vec2& b_pos, const Vec2& b_dir);
};