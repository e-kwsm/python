// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/detail/result.hpp>
#include <boost/type.hpp>
#include <functional>

using boost::python::detail::result;
using boost::type;

void expect_int(type<int>*) {}
void expect_string(type<char*>*) {}

struct X {};

int main()
{
    // Test the usage which works for functions, member functions, and data members
    expect_int(
        result((int(*)())BOOST_NULLPTR)
        );

    expect_int(
        result((int(*)(char))BOOST_NULLPTR)
        );

    expect_int(
        result((int(X::*)())BOOST_NULLPTR)
        );

    expect_int(
        result((int(X::*)(char))BOOST_NULLPTR)
        );

    expect_int(
        result((int(X::*))BOOST_NULLPTR)
        );

    expect_string(
        result((char*(*)())BOOST_NULLPTR)
        );
    
    expect_string(
        result((char*(*)(char))BOOST_NULLPTR)
        );
    
    expect_string(
        result((char*(X::*)())BOOST_NULLPTR)
        );
    
    expect_string(
        result((char*(X::*)(char))BOOST_NULLPTR)
        );
    
    expect_string(
        result((char*(X::*))BOOST_NULLPTR)
        );
    
    // Show that we can use the general version that works for
    // AdaptableFunctions
    expect_int(
        result((int(*)())BOOST_NULLPTR, 0)
        );

    expect_int(
        result((int(*)(char))BOOST_NULLPTR, 0)
        );

    expect_int(
        result((int(X::*)())BOOST_NULLPTR, 0)
        );

    expect_int(
        result((int(X::*)(char))BOOST_NULLPTR, 0)
        );

    expect_int(
        result((int(X::*))BOOST_NULLPTR, 0)
        );
    
    expect_int(
        result(std::plus<int>(),0)
        );

    expect_string(
        result((char*(*)())BOOST_NULLPTR, 0)
        );
    
    expect_string(
        result((char*(*)(char))BOOST_NULLPTR, 0)
        );
    
    expect_string(
        result((char*(X::*)())BOOST_NULLPTR, 0)
        );
    
    expect_string(
        result((char*(X::*)(char))BOOST_NULLPTR, 0)
        );
    
    expect_string(
        result((char*(X::*))BOOST_NULLPTR, 0)
        );
    
    expect_string(
        result(std::plus<char*>(),0)
        );

    return 0;
}
