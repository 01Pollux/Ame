#define BOOST_TEST_MODULE PropertyDescriptor
#include <boost/test/included/unit_test.hpp>

#include <Gfx/Shading/PropertyDescriptor.hpp>

BOOST_AUTO_TEST_SUITE(PropertyDescriptor)

namespace Shading = Ame::Gfx::Shading;

BOOST_AUTO_TEST_CASE(ArrayStruct)
{
    /*
struct TestStruct
{
    int x[7];
};

struct Struct1
{
    int x;
    TestStruct n[4];
    float y[19];
    int z;
};

struct Struct2
{
    int x;
    float y[17];
};

struct MyData
{
    Struct1 x1;
    Struct2 x2[2];
    Struct1 y1;
    Struct2 y2;
};

struct Resource
{
    MyData d;
};
    */

    Shading::PropertyDescriptor substruct;
    substruct
        .Int("x", 7);

    Shading::PropertyDescriptor struct1;
    struct1
        .Int("x")
        .Struct("n", substruct, 4)
        .Float("y", 19)
        .Bool("z");

    Shading::PropertyDescriptor struct2;
    struct2
        .Int("x")
        .Float("y", 17);

    Shading::PropertyDescriptor descriptor;
    descriptor
        .Struct("x1", struct1)
        .Struct("x2", struct2, 2)
        .Struct("y1", struct1)
        .Struct("y2", struct2);

    BOOST_CHECK_EQUAL(struct1.GetStructSize(), 760);
    BOOST_CHECK_EQUAL(struct2.GetStructSize(), 276);

    BOOST_CHECK_EQUAL(descriptor.GetOffset("x1.x"), 0);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("x1.n[0]"), 16);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("x1.y[0]"), 464);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("x1.z"), 756);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("x2[0]"), 768);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("y1"), 1344);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("y2"), 2112);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("y2.y[16]"), 2384);
    BOOST_CHECK_EQUAL(descriptor.GetStructSize(), 2388);
}

BOOST_AUTO_TEST_CASE(Struct)
{
    /*
struct TestStruct
{
    int x;
};

struct Struct1
{
    int x;
    TestStruct n;
    float y;
    int z;
};

struct Struct2
{
    int x;
    float y;
};

struct MyData
{
    Struct1 x1;
    Struct2 x2;
    Struct1 y1;
    Struct2 y2;
};

struct Resource
{
    MyData d;
};
    */

    Shading::PropertyDescriptor substruct;
    substruct.Int("x");

    Shading::PropertyDescriptor struct1;
    struct1
        .Int("x")
        .Struct("n", substruct)
        .Float("y")
        .Bool("z");

    Shading::PropertyDescriptor struct2;
    struct2
        .Int("x")
        .Float("y");

    Shading::PropertyDescriptor descriptor;
    descriptor
        .Struct("x1", struct1)
        .Struct("x2", struct2)
        .Struct("y1", struct1)
        .Struct("y2", struct2);

    BOOST_CHECK_EQUAL(struct1.GetStructSize(), 28);
    BOOST_CHECK_EQUAL(struct2.GetStructSize(), 8);

    BOOST_CHECK_EQUAL(descriptor.GetOffset("x1"), 0);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("x2"), 32);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("y1"), 48);
    BOOST_CHECK_EQUAL(descriptor.GetOffset("y2"), 80);
    BOOST_CHECK_EQUAL(descriptor.GetStructSize(), 88);
}

BOOST_AUTO_TEST_SUITE_END()
