// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/static_assert.hpp>
#include <boost/python/detail/if_else.hpp>
#include <boost/type_traits/same_traits.hpp>

    typedef char c1;
    typedef char c2[2];
    typedef char c3[3];
    typedef char c4[4];

template <unsigned size>
struct choose
{
#if 1
    typedef typename boost::python::detail::if_<
        (sizeof(c1) == size)
    >::template then<
        c1
    >::template elif<
        (sizeof(c2) == size)
    >::template then<
        c2
    >::template elif<
        (sizeof(c3) == size)
    >::template then<
        c3
    >::template elif<
        (sizeof(c4) == size)
    >::template then<
        c4
    >::template else_<void*>::type type;
#else
    typedef typename boost::python::detail::if_<
        (sizeof(c1) == size)
        , c1
    >::template elif<
        (sizeof(c2) == size)
        , c2
    >::template elif<
        (sizeof(c3) == size)
        , c3
    >::template elif<
        (sizeof(c4) == size)
        , c4
    >::template else_<void*>::type type;
#endif 
};

int main()
{
    BOOST_STATIC_ASSERT((boost::is_same<choose<1>::type,c1>::value));
    BOOST_STATIC_ASSERT((boost::is_same<choose<2>::type,c2>::value));
    BOOST_STATIC_ASSERT((boost::is_same<choose<3>::type,c3>::value));
    BOOST_STATIC_ASSERT((boost::is_same<choose<4>::type,c4>::value));
    BOOST_STATIC_ASSERT((boost::is_same<choose<5>::type,void*>::value));
    return 0;
}