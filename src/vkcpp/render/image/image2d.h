#ifndef VKCPP_RENDER_IMAGE_IMAGE_2D_H
#define VKCPP_RENDER_IMAGE_IMAGE_2D_H

#include "stb/stb_image.h"
#include "image.h"
#include <utility>

namespace vkcpp
{
    class Image2D : public Image
    {
    private:
        const char *filename_{VK_NULL_HANDLE};

        int tex_width_;

        int tex_height_;

        int tex_channels_;

    public:
        explicit Image2D(const Device *device,
                         const CommandPool *command_pool,
                         const char *filename,
                         VkFilter filter = VK_FILTER_LINEAR,
                         VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                         bool anisotropic = true,
                         bool mipmap = true,
                         bool load = true);
        explicit Image2D(const Device *device,
                         const CommandPool *command_pool,
                         const VkExtent3D &extent,
                         VkFilter filter = VK_FILTER_LINEAR,
                         VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                         bool anisotropic = true,
                         bool mipmap = true,
                         bool load = true);
        virtual ~Image2D() = default;

        void init_image_view();

        void init_image_2d();
        /**
         * Must same width and height
         * Maybe problem : read and write at the same time
         */
        void sub_texture_image(const char *filename);
    };

} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_IMAGE_IMAGE_2D_H