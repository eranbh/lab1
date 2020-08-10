#pragma once 

namespace loader
{
    struct proc_info{};
    struct remote_proc_info : proc_info {};
    static constexpr proc_info this_proc{};

    class command_n_control
    {
        public:
            command_n_control();
            ~command_n_control();
            void inject(proc_info = this_proc);
            void inject(remote_proc_info);
    };
    
} // namespace loader
