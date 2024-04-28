#include "camera.hpp"

ThreeDL::Camera::Camera(const Vec3& position, const Vec3& rotation)
    : position_(position),
    rotation_(rotation)
{}

void ThreeDL::Camera::calculate_dirs() {
    forward_ = {0, 0, 1};
    right_ = {1, 0, 0};

    forward_.rotate(0, -rotation_.y, 0);
    right_.rotate(0, -rotation_.y, 0);
}

void ThreeDL::Camera::move_forward(double delta) {
    position_ = position_ + (forward_ * delta);
}

void ThreeDL::Camera::move_right(double delta) {
    position_ = position_ + (right_ * delta);
}

void ThreeDL::Camera::tilt(double theta) {
    rotation_.x += theta;
    if (rotation_.x > max_rot_.x) rotation_.x = max_rot_.x;
    if (rotation_.x < -max_rot_.x) rotation_.x = -max_rot_.x;
}

void ThreeDL::Camera::pan(double theta) {
    rotation_.y += theta;
    if (rotation_.y > max_rot_.y) rotation_.y = max_rot_.y;
    if (rotation_.y < -max_rot_.y) rotation_.y = -max_rot_.y;
}

void ThreeDL::Camera::roll(double theta) {
    rotation_.z += theta;
    if (rotation_.z > max_rot_.z) rotation_.z = max_rot_.z;
    if (rotation_.z < -max_rot_.z) rotation_.z = -max_rot_.z;
}