// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/long.hpp>

namespace boost { namespace python { namespace detail {

new_reference long_base::call(object const& arg_)
{
    return reinterpret_cast<detail::new_reference>(PyObject_CallFunction(
        reinterpret_cast<PyObject*>(&PyLong_Type), const_cast<char*>("(O)"), 
        arg_.ptr()));
}

new_reference long_base::call(object const& arg_, object const& base)
{
    return reinterpret_cast<detail::new_reference>(PyObject_CallFunction(
        reinterpret_cast<PyObject*>(&PyLong_Type), const_cast<char*>("(OO)"), 
        arg_.ptr(), base.ptr()));
}

long_base::long_base()
    : object(
        reinterpret_cast<detail::new_reference>(
            PyObject_CallFunction(reinterpret_cast<PyObject*>(&PyLong_Type), const_cast<char*>("()")))
        )
{}

long_base::long_base(object_cref arg)
    : object(long_base::call(arg))
{}

long_base::long_base(object_cref arg, object_cref base)
    : object(long_base::call(arg, base))
{}


}}} // namespace boost::python
