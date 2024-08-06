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
        ((life_support*)self)->patient = nullptr;
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
    nullptr,                            /* tp_getattr */
    nullptr,                            /* tp_setattr */
    nullptr,                            /* tp_compare */
    nullptr, //(reprfunc)func_repr,     /* tp_repr */
    nullptr,                            /* tp_as_number */
    nullptr,                            /* tp_as_sequence */
    nullptr,                            /* tp_as_mapping */
    nullptr,                            /* tp_hash */
    life_support_call,                  /* tp_call */
    nullptr,                            /* tp_str */
    nullptr, // PyObject_GenericGetAttr,  /* tp_getattro */
    nullptr, // PyObject_GenericSetAttr,  /* tp_setattro */
    nullptr,                            /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT /* | Py_TPFLAGS_HAVE_GC */,/* tp_flags */
    nullptr,                            /* tp_doc */
    nullptr, // (traverseproc)func_traverse,  /* tp_traverse */
    nullptr,                            /* tp_clear */
    nullptr,                            /* tp_richcompare */
    0, //offsetof(PyLife_SupportObject, func_weakreflist), /* tp_weaklistoffset */
    nullptr,                            /* tp_iter */
    nullptr,                            /* tp_iternext */
    nullptr,                            /* tp_methods */
    nullptr, // func_memberlist,        /* tp_members */
    nullptr, //func_getsetlist,         /* tp_getset */
    nullptr,                            /* tp_base */
    nullptr,                            /* tp_dict */
    nullptr,                            /* tp_descr_get */
    nullptr,                            /* tp_descr_set */
    0, //offsetof(PyLife_SupportObject, func_dict),      /* tp_dictoffset */
    nullptr,                            /* tp_init */
    nullptr,                            /* tp_alloc */
    nullptr,                            /* tp_new */
    nullptr,                            /* tp_free */
    nullptr,                            /* tp_is_gc */
    nullptr,                            /* tp_bases */
    nullptr,                            /* tp_mro */
    nullptr,                            /* tp_cache */
    nullptr,                            /* tp_subclasses */
    nullptr,                            /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
    nullptr                             /* tp_del */
#endif
};

PyObject* make_nurse_and_patient(PyObject* nurse, PyObject* patient)
{
    if (nurse == Py_None || nurse == patient)
        return nurse;
    
    if (Py_TYPE(&life_support_type) == nullptr)
    {
        Py_SET_TYPE(&life_support_type, &PyType_Type);
        PyType_Ready(&life_support_type);
    }
    
    life_support* system = PyObject_New(life_support, &life_support_type);
    if (!system)
        return nullptr;

    system->patient = nullptr;
    
    // We're going to leak this reference, but don't worry; the
    // life_support system decrements it when the nurse dies.
    PyObject* weakref = PyWeakref_NewRef(nurse, (PyObject*)system);

    // weakref has either taken ownership, or we have to release it
    // anyway
    Py_DECREF(system);
    if (!weakref)
        return nullptr;
    
    system->patient = patient;
    Py_XINCREF(patient); // hang on to the patient until death
    return weakref;
}

}}} // namespace boost::python::objects
