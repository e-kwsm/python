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
        result((int(*)())nullptr)
        );

    expect_int(
        result((int(*)(char))nullptr)
        );

    expect_int(
        result((int(X::*)())nullptr)
        );

    expect_int(
        result((int(X::*)(char))nullptr)
        );

    expect_int(
        result((int(X::*))nullptr)
        );

    expect_string(
        result((char*(*)())nullptr)
        );
    
    expect_string(
        result((char*(*)(char))nullptr)
        );
    
    expect_string(
        result((char*(X::*)())nullptr)
        );
    
    expect_string(
        result((char*(X::*)(char))nullptr)
        );
    
    expect_string(
        result((char*(X::*))nullptr)
        );
    
    // Show that we can use the general version that works for
    // AdaptableFunctions
    expect_int(
        result((int(*)())nullptr, 0)
        );

    expect_int(
        result((int(*)(char))nullptr, 0)
        );

    expect_int(
        result((int(X::*)())nullptr, 0)
        );

    expect_int(
        result((int(X::*)(char))nullptr, 0)
        );

    expect_int(
        result((int(X::*))nullptr, 0)
        );
    
    expect_int(
        result(std::plus<int>(),0)
        );

    expect_string(
        result((char*(*)())nullptr, 0)
        );
    
    expect_string(
        result((char*(*)(char))nullptr, 0)
        );
    
    expect_string(
        result((char*(X::*)())nullptr, 0)
        );
    
    expect_string(
        result((char*(X::*)(char))nullptr, 0)
        );
    
    expect_string(
        result((char*(X::*))nullptr, 0)
        );
    
    expect_string(
        result(std::plus<char*>(),0)
        );

    return 0;
}
