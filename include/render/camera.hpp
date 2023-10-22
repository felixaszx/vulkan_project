#ifndef RENDER_CAMERA_HPP
#define RENDER_CAMERA_HPP

#include <memory>
#include "ext/buffer.hpp"
#include "glms.hpp"

namespace proj
{
    namespace render
    {
        struct Camera : public vk::DescriptorBufferInfo
        {
            inline static const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

            float yaw_ = 0.0f;
            float pitch_ = 0.0f;
            float fov_ = 45.0f;
            float near_ = 0.1f;
            float far_ = 1000.0f;

            vk::Extent2D extent_;
            std::unique_ptr<Buffer> uniform_buffer_ = nullptr;

            struct
            {
                glm::mat4 view_{1.0f};
                glm::mat4 proj_{1.0f};
            } matrices_;
            glm::vec3 position_{};

            Camera(vma::Allocator allocator, vk::Extent2D extent);

            glm::vec3 get_front();
            void update_matrices();
        };
    }; // namespace render
};     // namespace proj

#endif // RENDER_CAMERA_HPP
