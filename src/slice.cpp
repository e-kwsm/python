#include "boost/python/slice.hpp"

// Copyright (c) 2004 Jonathan Brandmeyer
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


namespace boost { namespace python { namespace detail {

slice_base::slice_base(PyObject* start, PyObject* stop, PyObject* step)
  : object(reinterpret_cast<detail::new_reference>( PySlice_New(start, stop, step)))
{
}

object
slice_base::start() const
{
    return object( reinterpret_cast<detail::borrowed_reference>(
        (reinterpret_cast<PySliceObject*>(this->ptr()))->start));
}

object
slice_base::stop() const
{
    return object( reinterpret_cast<detail::borrowed_reference>(
        (reinterpret_cast<PySliceObject*>(this->ptr()))->stop));
}

object
slice_base::step() const
{
    return object( reinterpret_cast<detail::borrowed_reference>(
        (reinterpret_cast<PySliceObject*>(this->ptr()))->step));
}

static struct register_slice_pytype_ptr
{
    register_slice_pytype_ptr()
    {
        const_cast<converter::registration &>(
            converter::registry::lookup(boost::python::type_id<boost::python::slice>())
            ).m_class_object = &PySlice_Type;
    }
}register_slice_pytype_ptr_;

} } } // !namespace boost::python::detail
