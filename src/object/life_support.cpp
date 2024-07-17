// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/object/life_support.hpp>
#include <boost/python/detail/none.hpp>
#include <boost/python/refcount.hpp>

namespace boost { namespace python { namespace objects { 

struct life_support
{
    PyObject_HEAD
    PyObject* patient;
};

extern "C"
{
    static void
    life_support_dealloc(PyObject* self)
    {
        Py_XDECREF(((life_support*)self)->patient);
        self->ob_type->tp_free(self);
    }

    static PyObject *
    life_support_call(PyObject *self, PyObject *arg, PyObject * /*kw*/)
    {
        // Let the patient die now
        Py_XDECREF(((life_support*)self)->patient);
        ((life_support*)self)->patient = BOOST_NULLPTR;
        // Let the weak reference die. This probably kills us.
        Py_XDECREF(PyTuple_GET_ITEM(arg, 0));
        return ::boost::python::detail::none();
    }
}

PyTypeObject life_support_type = {
    PyVarObject_HEAD_INIT(NULL, 0)//(&PyType_Type)
    const_cast<char*>("Boost.Python.life_support"),
    sizeof(life_support),
    0,
    life_support_dealloc,               /* tp_dealloc */
    0,                                  /* tp_print */
    BOOST_NULLPTR,                            /* tp_getattr */
    BOOST_NULLPTR,                            /* tp_setattr */
    BOOST_NULLPTR,                            /* tp_compare */
    BOOST_NULLPTR, //(reprfunc)func_repr,     /* tp_repr */
    BOOST_NULLPTR,                            /* tp_as_number */
    BOOST_NULLPTR,                            /* tp_as_sequence */
    BOOST_NULLPTR,                            /* tp_as_mapping */
    BOOST_NULLPTR,                            /* tp_hash */
    life_support_call,                  /* tp_call */
    BOOST_NULLPTR,                                  /* tp_str */
    BOOST_NULLPTR, // PyObject_GenericGetAttr,      /* tp_getattro */
    BOOST_NULLPTR, // PyObject_GenericSetAttr,      /* tp_setattro */
    BOOST_NULLPTR,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT /* | Py_TPFLAGS_HAVE_GC */,/* tp_flags */
    BOOST_NULLPTR,                                  /* tp_doc */
    BOOST_NULLPTR, // (traverseproc)func_traverse,  /* tp_traverse */
    BOOST_NULLPTR,                                  /* tp_clear */
    BOOST_NULLPTR,                                  /* tp_richcompare */
    0, //offsetof(PyLife_SupportObject, func_weakreflist), /* tp_weaklistoffset */
    BOOST_NULLPTR,                                  /* tp_iter */
    BOOST_NULLPTR,                                  /* tp_iternext */
    BOOST_NULLPTR,                                  /* tp_methods */
    BOOST_NULLPTR, // func_memberlist,              /* tp_members */
    BOOST_NULLPTR, // func_getsetlist,              /* tp_getset */
    BOOST_NULLPTR,                                  /* tp_base */
    BOOST_NULLPTR,                                  /* tp_dict */
    BOOST_NULLPTR,                                  /* tp_descr_get */
    BOOST_NULLPTR,                                  /* tp_descr_set */
    0, //offsetof(PyLife_SupportObject, func_dict),      /* tp_dictoffset */
    BOOST_NULLPTR,                                /* tp_init */
    BOOST_NULLPTR,                                /* tp_alloc */
    BOOST_NULLPTR,                                /* tp_new */
    BOOST_NULLPTR,                                /* tp_free */
    BOOST_NULLPTR,                                /* tp_is_gc */
    BOOST_NULLPTR,                                /* tp_bases */
    BOOST_NULLPTR,                                /* tp_mro */
    BOOST_NULLPTR,                                /* tp_cache */
    BOOST_NULLPTR,                                /* tp_subclasses */
    BOOST_NULLPTR,                                /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
    BOOST_NULLPTR                                 /* tp_del */
#endif
};

PyObject* make_nurse_and_patient(PyObject* nurse, PyObject* patient)
{
    if (nurse == Py_None || nurse == patient)
        return nurse;
    
    if (Py_TYPE(&life_support_type) == BOOST_NULLPTR)
    {
        Py_SET_TYPE(&life_support_type, &PyType_Type);
        PyType_Ready(&life_support_type);
    }
    
    life_support* system = PyObject_New(life_support, &life_support_type);
    if (!system)
        return BOOST_NULLPTR;

    system->patient = BOOST_NULLPTR;
    
    // We're going to leak this reference, but don't worry; the
    // life_support system decrements it when the nurse dies.
    PyObject* weakref = PyWeakref_NewRef(nurse, (PyObject*)system);

    // weakref has either taken ownership, or we have to release it
    // anyway
    Py_DECREF(system);
    if (!weakref)
        return BOOST_NULLPTR;
    
    system->patient = patient;
    Py_XINCREF(patient); // hang on to the patient until death
    return weakref;
}

}}} // namespace boost::python::objects
