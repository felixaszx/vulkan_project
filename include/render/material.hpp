#ifndef RENDER_MATERIAL_HPP
#define RENDER_MATERIAL_HPP

#include <memory>
#include "ext/image.hpp"
#include "vk/vk.hpp"
#include "glms.hpp"

namespace proj
{
    namespace render
    {
        struct Material
        {
            std::unique_ptr<Image> abledo_ = nullptr;
            std::unique_ptr<Image> specular_ = nullptr;
            std::unique_ptr<Image> ambient_ = nullptr;
            std::unique_ptr<Image> opacity_ = nullptr;
            std::unique_ptr<Image> normal_ = nullptr;
            std::unique_ptr<Image> emissive_ = nullptr;

            void create_general();
            void create_mipmapped();
        };
    }; // namespace render
};     // namespace proj

#endif // RENDER_MATERIAL_HPP
