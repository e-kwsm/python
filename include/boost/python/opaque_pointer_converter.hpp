// Copyright Gottfried Ganßauge 2003..2006.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/*
 * Generic Conversion of opaque C++-pointers to a Python-Wrapper.
 */
# ifndef OPAQUE_POINTER_CONVERTER_HPP_
# define OPAQUE_POINTER_CONVERTER_HPP_

# include <boost/python/detail/prefix.hpp>
# include <boost/python/lvalue_from_pytype.hpp>
# include <boost/python/to_python_converter.hpp>
# include <boost/python/converter/registrations.hpp>
# include <boost/python/detail/dealloc.hpp>
# include <boost/python/detail/type_traits.hpp>
# include <boost/python/detail/none.hpp>
# include <boost/python/type_id.hpp>
# include <boost/python/errors.hpp>

# include <boost/implicit_cast.hpp>

# include <boost/mpl/eval_if.hpp>
# include <boost/mpl/identity.hpp>
# include <boost/mpl/assert.hpp>

// opaque --
//
// registers to- and from- python conversions for a type Pointee.
//
// Note:
// In addition you need to define specializations for type_id
// on the type pointed to by Pointer using
// BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(Pointee)
//
// For an example see libs/python/test/opaque.cpp
//
namespace boost { namespace python {

template <class Pointee>
struct opaque
{
    opaque()
    {
        if (type_object.tp_name == BOOST_NULLPTR)
        {
            type_object.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready (&type_object) < 0)
            {
                throw error_already_set();
            }

            this->register_self();
        }
    }
    
    static opaque instance;
private:
    
    static void* extract(PyObject* op)
    {
        return PyObject_TypeCheck(op, &type_object)
            ? static_cast<python_instance*>(implicit_cast<void*>(op))->x
            : BOOST_NULLPTR
            ;
    }

    static PyObject* wrap(void const* px)
    {
        Pointee* x = *static_cast<Pointee*const*>(px);
        
        if (x == BOOST_NULLPTR)
            return detail::none();

        if ( python_instance *o = PyObject_New(python_instance, &type_object) )
        {
            o->x = x;
            return static_cast<PyObject*>(implicit_cast<void*>(o));
        }
        else
        {
            throw error_already_set();
        }
    }

    void register_self()
    {
        converter::registration const *existing =
            converter::registry::query (type_id<Pointee*>());

        if ((existing == BOOST_NULLPTR) || (existing->m_to_python == BOOST_NULLPTR))
        {
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
            converter::registry::insert(&extract, type_id<Pointee>(), &get_pytype);
            converter::registry::insert(&wrap, type_id<Pointee*>(), &get_pytype);
#else
            converter::registry::insert(&extract, type_id<Pointee>());
            converter::registry::insert(&wrap, type_id<Pointee*>());
#endif
        }
    }

    struct python_instance
    {
        PyObject_HEAD
        Pointee* x;
    };
    
    static PyTypeObject type_object;
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
    static PyTypeObject const *get_pytype(){return  &type_object; }
#endif
};

template <class Pointee>
opaque<Pointee> opaque<Pointee>::instance;

template <class Pointee>
PyTypeObject opaque<Pointee>::type_object =
{
    PyVarObject_HEAD_INIT(BOOST_NULLPTR, 0)
    BOOST_NULLPTR,
    sizeof( BOOST_DEDUCED_TYPENAME opaque<Pointee>::python_instance ),
    0,
    ::boost::python::detail::dealloc,
    0,          /* tp_print */
    BOOST_NULLPTR,          /* tp_getattr */
    BOOST_NULLPTR,          /* tp_setattr */
    BOOST_NULLPTR,          /* tp_compare */
    BOOST_NULLPTR,          /* tp_repr */
    BOOST_NULLPTR,          /* tp_as_number */
    BOOST_NULLPTR,          /* tp_as_sequence */
    BOOST_NULLPTR,          /* tp_as_mapping */
    BOOST_NULLPTR,          /* tp_hash */
    BOOST_NULLPTR,          /* tp_call */
    BOOST_NULLPTR,          /* tp_str */
    BOOST_NULLPTR,          /* tp_getattro */
    BOOST_NULLPTR,          /* tp_setattro */
    BOOST_NULLPTR,          /* tp_as_buffer */
    0,          /* tp_flags */
    BOOST_NULLPTR,          /* tp_doc */
    BOOST_NULLPTR,          /* tp_traverse */
    BOOST_NULLPTR,          /* tp_clear */
    BOOST_NULLPTR,          /* tp_richcompare */
    0,          /* tp_weaklistoffset */
    BOOST_NULLPTR,          /* tp_iter */
    BOOST_NULLPTR,          /* tp_iternext */
    BOOST_NULLPTR,          /* tp_methods */
    BOOST_NULLPTR,          /* tp_members */
    BOOST_NULLPTR,          /* tp_getset */
    BOOST_NULLPTR,          /* tp_base */
    BOOST_NULLPTR,          /* tp_dict */
    BOOST_NULLPTR,          /* tp_descr_get */
    BOOST_NULLPTR,          /* tp_descr_set */
    0,          /* tp_dictoffset */
    BOOST_NULLPTR,          /* tp_init */
    BOOST_NULLPTR,          /* tp_alloc */
    BOOST_NULLPTR,          /* tp_new */
    BOOST_NULLPTR,          /* tp_free */
    BOOST_NULLPTR,          /* tp_is_gc */
    BOOST_NULLPTR,          /* tp_bases */
    BOOST_NULLPTR,          /* tp_mro */
    BOOST_NULLPTR,          /* tp_cache */
    BOOST_NULLPTR,          /* tp_subclasses */
    BOOST_NULLPTR,          /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
    BOOST_NULLPTR           /* tp_del */
#endif
};
}} // namespace boost::python

// If you change the below, don't forget to alter the end of type_id.hpp
#   define BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(Pointee)                     \
    namespace boost { namespace python {                                        \
    template<>                                                                  \
    inline type_info type_id<Pointee>()                                         \
    {                                                                           \
        return type_info (typeid (Pointee *));                                  \
    }                                                                           \
    template<>                                                                  \
    inline type_info type_id<const volatile Pointee&>()                         \
    {                                                                           \
        return type_info (typeid (Pointee *));                                  \
    }                                                                           \
    }}

# endif    // OPAQUE_POINTER_CONVERTER_HPP_
