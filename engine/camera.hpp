#pragma once

#include "utils.hpp"

namespace ThreeDL {
    class Camera {
        public:
            Camera(const Vec3& position, const Vec3& rotation);
            Camera() = delete;

            Vec3 position_;
            Vec3 rotation_;
            Vec3 forward_;
            Vec3 right_;

            void calculate_dirs();

            void move_forward(double delta);
            void move_right(double delta);

            void tilt(double theta);
            void pan(double theta);
            void roll(double theta);

            void set_max_rot(const Vec3& max_rot) {
                max_rot_ = max_rot;
            }

            ~Camera() = default;
        private:
            Vec3 max_rot_ = {360, 360, 360};
    };
}