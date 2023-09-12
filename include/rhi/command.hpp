#ifndef RHI_COMMAND_HPP
#define RHI_COMMAND_HPP

namespace rhi
{
    template <typename Api>
    class CmdBuffer
    {
      private:
        Api::CmdType cmd;

      public:
    };

    template <typename Api>
    class CmdPool
    {
      private:
        Api::CmdPoolType cmd_pool;

      public:
    };
}; // namespace rhi

#endif // RHI_COMMAND_HPP
