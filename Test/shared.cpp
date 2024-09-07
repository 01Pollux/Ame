#include <boost/dll.hpp>
#include <boost/signals2.hpp>
#include <iostream>

struct ISomeInterface
{
    virtual boost::signals2::connection listen(boost::signals2::signal<void()>::slot_type t) = 0;
};

boost::signals2::connection con;

void init(ISomeInterface* iface)
{
    con = iface->listen([] { std::cout << "Hello, Shared!" << std::endl; });
}

void shutdown()
{
    con.disconnect();
}

BOOST_DLL_AUTO_ALIAS(init);
BOOST_DLL_AUTO_ALIAS(shutdown);
