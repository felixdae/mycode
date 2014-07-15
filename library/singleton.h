#ifndef SINGLETON_H
#define SINGLETON_H 

namespace comm
{
    namespace library
    {
        class DummyLock
        {
            public:
                DummyLock();
                virtual ~DummyLock();
        };

        template <typename ObjType, typename Lock = DummyLock> class Singleton
        {
            private:
                Singleton();
            public:
                virtual ~Singleton();
                static ObjType* GetInstance()
                {
                    //double check lock
                    if (instance_ == nullptr)
                    {
                        DummyLock lock;
                        if (instance_ == nullptr)
                        {
                            instance_ = new ObjType;
                        }
                    }
                    return instance_;
                }
            private:
                static ObjType* instance_ = nullptr;
        }
    }
}
#endif /* SINGLETON_H */
