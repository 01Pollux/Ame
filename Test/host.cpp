#include <boost/dll.hpp>
#include <boost/signals2.hpp>
#include <iostream>

struct ISomeInterface
{
    virtual boost::signals2::connection listen(boost::signals2::signal<void()>::slot_type t) = 0;
};

struct SomeInterface : ISomeInterface
{
    boost::signals2::signal<void()> signal;

    virtual boost::signals2::connection listen(boost::signals2::signal<void()>::slot_type t) override
    {
        return signal.connect([] { std::cout << "Hello, World!" << std::endl; });
    }
};

int main()
{
    SomeInterface* iface = new SomeInterface;

    {
        boost::dll::shared_library dll("Shared", boost::dll::load_mode::append_decorations);

        auto init     = dll.get<void (*)(SomeInterface*)>("init");
        auto shutdown = dll.get<void (*)()>("shutdown");

        init(iface);
        iface->signal();
        shutdown();
    }

    iface->signal.connect([] { std::cout << "Hello, Host!" << std::endl; });
    iface->signal();

    return 0;
}