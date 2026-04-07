#ifndef BOOST_PYTHON_OTHER_HPP
# define BOOST_PYTHON_OTHER_HPP

# include <boost/python/detail/prefix.hpp>
// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

# include <boost/config.hpp>

namespace boost { namespace python {

template<class T> struct other
{ 
    using type = T;
};

namespace detail
{
  template<typename T>
  class is_other
  {
   public:
      BOOST_STATIC_CONSTANT(bool, value = false); 
  };

  template<typename T>
  class is_other<other<T> >
  {
   public:
      BOOST_STATIC_CONSTANT(bool, value = true);
  };

  template<typename T>
  class unwrap_other
  {
   public:
      using type = T;
  };

  template<typename T>
  class unwrap_other<other<T> >
  {
   public:
      using type = T;
  };
}

}} // namespace boost::python

#endif
