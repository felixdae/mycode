#ifndef LIBSCOPEGUARD_H
#define LIBSCOPEGUARD_H 
#include <functional>

namespace comm
{
    namespace library
    {
        class ScopeGuard
        {
            public:
                explicit ScopeGuard(std::function<void()> onExitScope)
                    : onExitScope_(onExitScope), dismissed_(false)
                { }

                ~ScopeGuard()
                {
                    if(!dismissed_)
                    {
                        onExitScope_();
                    }
                }

                void Dismiss()
                {
                    dismissed_ = true;
                }

            private:
                std::function<void()> onExitScope_;
                bool dismissed_;

            private: // noncopyable
                ScopeGuard(ScopeGuard const&);
                ScopeGuard& operator=(ScopeGuard const&);
        };
    }
}

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) comm::library::ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

#endif /* LIBSCOPEGUARD_H */
