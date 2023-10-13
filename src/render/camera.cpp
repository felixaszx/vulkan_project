#include "render/camera.hpp"

namespace proj
{
    namespace render
    {
        Camera::Camera(vma::Allocator allocator, vk::Extent2D extent)
            : extent_(extent)
        {
            using namespace ext;
            BufferCreator uniform_creator(allocator, uniform_buffer, host_seq);
            uniform_buffer_.reset(new Buffer(uniform_creator.create(sizeof(matrices_))));
            uniform_buffer_->map_memory();

            update_matrices();
            this->setBuffer(*uniform_buffer_);
            this->setRange(sizeof(matrices_));
        }

        glm::vec3 Camera::get_front()
        {
            glm::vec3 front(0.0f);
            front.x = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
            front.y = sin(glm::radians(pitch_));
            front.z = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));

            return front;
        }

        void Camera::update_matrices()
        {
            matrices_.view_ = glm::lookAt(position_, position_ + glm::normalize(get_front()), UP);
            matrices_.proj_ = glms::perspective(glm::radians(fov_),                           //
                                                (float)extent_.width / (float)extent_.height, //
                                                near_, far_);
            memcpy(uniform_buffer_->mapping(), 0x0, sizeof(matrices_));
        }
    }; // namespace render
};     // namespace proj
