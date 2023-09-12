#ifndef RHI_DEVICE_HPP
#define RHI_DEVICE_HPP

namespace rhi
{
    template <typename Api>
    class Queue
    {
      private:
        Api::QueueType queue;

      public:
        Queue(Api::QueueType new_queue);
    };

    template <typename Api>
    class Device
    {
      private:
        Api api;

      public:
        Device(const Api::DeviceConfigType& config);
        ~Device();

        Queue<Api> get_graphics_queue();
        Queue<Api> get_transfer_queue();
        Queue<Api> get_compute_queue();
        Queue<Api> get_present_queue(); // alias of graphics queue
    };

    template <typename Api>
    inline Device<Api>::Device(const Api::DeviceConfigType& config)
    {
        api = {};
        Api::config_init_device(config);
    }

    template <typename Api>
    inline Device<Api>::~Device()
    {
        Api::destroy_device();
    }

    template <typename Api>
    inline Queue<Api> Device<Api>::get_graphics_queue()
    {
        return Queue(Api::get_graphics_queue());
    }

    template <typename Api>
    inline Queue<Api> Device<Api>::get_transfer_queue()
    {
        return Queue(Api::get_transfer_queue());
    }

    template <typename Api>
    inline Queue<Api> Device<Api>::get_compute_queue()
    {
        return Queue(Api::get_transfer_queue());
    }

    template <typename Api>
    inline Queue<Api> Device<Api>::get_present_queue()
    {
        return Queue(Api::get_graphics_queue());
    }

    template <typename Api>
    inline Queue<Api>::Queue(Api::QueueType new_queue)
        : queue(new_queue)
    {
    }
}; // namespace rhi

#endif // RHI_DEVICE_HPP
