//  (C) Copyright Joel de Guzman 2003.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies. This
//  software is provided "as is" without express or implied warranty, and
//  with no claim as to its suitability for any purpose.

#ifndef INDEXING_SUITE_DETAIL_JDG20036_HPP
# define INDEXING_SUITE_DETAIL_JDG20036_HPP

# include <boost/python/extract.hpp>
# include <boost/scoped_ptr.hpp>
# include <boost/detail/binary_search.hpp>
# include <boost/get_pointer.hpp>
# include <vector>
# include <map>

namespace boost { namespace python { namespace detail {

#if defined(NDEBUG)
#define BOOST_PYTHON_INDEXING_CHECK_INVARIANT
#else
#define BOOST_PYTHON_INDEXING_CHECK_INVARIANT check_invariant()
#endif
    
    template <class Proxy>
    struct compare_proxy_index
    {
        //  This functor compares a proxy and an index.
        //  This is used by proxy_group::first_proxy to
        //  get first proxy with index i.
        
        template <class Index>
        bool operator()(PyObject* prox, Index i) const
        {
            return extract<Proxy&>(prox)().get_index() < i;
        }
    };        
 
    //  The proxy_group class holds a vector of container element
    //  proxies. First, what is a container element proxy? A container 
    //  element proxy acts like a smart pointer holding a reference to 
    //  a container and an index (see container_element, for details). 
    //
    //  The proxies are held in a vector always sorted by its index.
    //  Various functions manage the addition, removal and searching
    //  of proxies from the vector.
    //
    template <class Proxy>
    class proxy_group
    {
    public:
    
        typedef typename std::vector<PyObject*>::const_iterator const_iterator;
        typedef typename std::vector<PyObject*>::iterator iterator;
        typedef typename Proxy::index_type index_type;
        typedef typename Proxy::policies_type policies_type;
        
        iterator
        first_proxy(index_type i)
        {
            // Return the first proxy with index <= i
            return boost::detail::lower_bound(
                proxies.begin(), proxies.end(), 
                i, compare_proxy_index<Proxy>());
        }

        void
        remove(Proxy& proxy)
        {
            // Remove a proxy
            for (iterator iter = first_proxy(proxy.get_index());
                iter != proxies.end(); ++iter)
            {
                if (&extract<Proxy&>(*iter)() == &proxy)
                {
                    proxies.erase(iter);
                    break;
                }
            }
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
        }

        void
        add(PyObject* prox)
        {
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
            // Add a proxy
            proxies.insert(
                first_proxy(extract<Proxy&>(prox)().get_index()), prox);
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
        }

        void
        erase(index_type from, index_type to)
        {
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
            // Erase all proxies with indexes from..to 
            replace(from, to, 0);
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
        }

        void
        replace(
            index_type from, 
            index_type to, 
            typename std::vector<PyObject*>::size_type len)
        {
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
            // Erase all proxies with indexes from..to.
            // Adjust the displaced indexes such that the
            // final effect is that we have inserted *len*
            // number of proxies in the vacated region. This
            // procedure involves adjusting the indexes of 
            // the proxies.
            
            iterator left = first_proxy(from);
            iterator right = proxies.end(); // we'll adjust this later
            
            for (iterator iter = left; iter != right; ++iter)
            {
                if (extract<Proxy&>(*iter)().get_index() > to)
                {
                    right = iter; // adjust right
                    break;
                }
                extract<Proxy&> p(*iter);
                p().detach();
            }
            
            typename std::vector<PyObject*>::size_type 
                offset = left-proxies.begin();
            proxies.erase(left, right);
            right = proxies.begin()+offset;

            while (right != proxies.end())
            {
                extract<Proxy&> p(*right);
                p().set_index(
                    policies_type::adjust_index(
                        extract<Proxy&>(*right)().get_index(), from, to, len));
                ++right;
            }
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
        }
        
        PyObject*
        find(index_type i)
        {
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
            // Find the proxy with *exact* index i.
            // Return 0 (null) if no proxy with the 
            // given index is found.
            iterator iter = first_proxy(i);
            if (iter != proxies.end()
                && extract<Proxy&>(*iter)().get_index() == i)
            {
                BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
                return *iter;
            }
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
            return 0;
        }

        typename std::vector<PyObject*>::size_type 
        size() const
        {
            BOOST_PYTHON_INDEXING_CHECK_INVARIANT;
            // How many proxies are there so far?
            return proxies.size();
        } 

    private:

#if !defined(NDEBUG)
        void
        check_invariant() const
        {
            for (const_iterator i = proxies.begin(); i != proxies.end(); ++i)
            {
                if ((*i)->ob_refcnt <= 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, 
                        "Invariant: Proxy vector in an inconsistent state");
                    throw_error_already_set();
                }
            }
        }
#endif
        
        std::vector<PyObject*> proxies;
    };
            
    // proxy_links holds a map of Container pointers (keys)
    // with proxy_group(s) (data). Various functions manage 
    // the addition, removal and searching of proxies from 
    // the map.
    //
    template <class Proxy, class Container>
    class proxy_links
    {
    public:
    
        typedef std::map<Container*, proxy_group<Proxy> > links_t;
        typedef typename Proxy::index_type index_type;

        void
        remove(Proxy& proxy)
        {
            // Remove a proxy.
            typename links_t::iterator r = links.find(&proxy.get_container());
            if (r != links.end())
            {
                r->second.remove(proxy);
                if (r->second.size() == 0)
                    links.erase(r);
            }
        }
        
        void
        add(PyObject* prox, Container& container)
        {
            // Add a proxy
            links[&container].add(prox);
        }
        
        void
        erase(Container& container, index_type from, index_type to)
        {
            // Erase all proxies with indexes from..to 
            typename links_t::iterator r = links.find(&container);
            if (r != links.end())
            {
                r->second.erase(from, to);
                if (r->second.size() == 0)
                    links.erase(r);
            }
        }

        void
        replace(
            Container& container, 
            index_type from, index_type to, index_type len)
        {
            // Erase all proxies with indexes from..to.
            // Adjust the displaced indexes such that the
            // final effect is that we have inserted *len*
            // number of proxies in the vacated region. This
            // procedure involves adjusting the indexes of 
            // the proxies.

            typename links_t::iterator r = links.find(&container);
            if (r != links.end())
            {
                r->second.replace(from, to, len);
                if (r->second.size() == 0)
                    links.erase(r);
            }
        }
        
        PyObject*
        find(Container& container, index_type i)
        {
            // Find the proxy with *exact* index i.
            // Return 0 (null) if no proxy with the given 
            // index is found.
            typename links_t::iterator r = links.find(&container);
            if (r != links.end())
                return r->second.find(i);
            return 0;
        }

    private:
    
        links_t links;
    };
    
    // container_element is our container proxy class.
    // This class acts like a smart pointer to a container
    // element. The class holds an index and a reference to
    // a container. Dereferencing the smart pointer will
    // retrieve the nth (index) element from the container.
    //
    // A container_element can also be detached from the
    // container. In such a detached state, the container_element
    // holds a copy of the nth (index) element, which it 
    // returns when dereferenced.
    //
    template <class Container, class Index, class Policies>
    class container_element
    {
    public:
    
        typedef Index index_type;
        typedef typename Container::value_type element_type;
        typedef Policies policies_type;
        typedef container_element<Container, Index, Policies> self_t;
        typedef proxy_group<self_t> links_type;
        
        container_element(object container, Index index)
            : ptr()
            , container(container)
            , index(index)
        {
        }
            
        container_element(container_element const& ce)
            : ptr(ce.ptr.get() == 0 ? 0 : new element_type(*ce.ptr.get()))
            , container(ce.container)
            , index(ce.index)
        {
        }

        ~container_element()
        {
            if (!is_detached())
                get_links().remove(*this);
        }
                      
        element_type& operator*() const
        {
            if (is_detached())
                return *ptr.get();
            return Policies::get_item(get_container(), index);
        }
        
        element_type* get() const
        {
            if (is_detached())
                return ptr.get();
            return &Policies::get_item(get_container(), index);
        }
        
        void
        detach()
        {
            if (!is_detached())
            {
                ptr.reset(
                    new element_type(
                        Policies::get_item(get_container(), index)));
                container = object(); // free container. reset it to None
            }
        }
        
        bool
        is_detached() const
        {
            return ptr.get() != 0;
        }

        Container& 
        get_container() const
        {
            return extract<Container&>(container)();
        }
        
        Index 
        get_index() const
        {
            return index;
        }

        void 
        set_index(Index i)
        {
            index = i;
        }
 
        static proxy_links<self_t, Container>&
        get_links()
        {
            // All container_element(s) maintain links to
            // its container in a global map (see proxy_links).
            // This global "links" map is a singleton.
            
            static proxy_links<self_t, Container> links;
            return links; // singleton
        }

    private:
            
        container_element& operator=(container_element const& ce);

        scoped_ptr<element_type> ptr;
        object container;
        Index index;
    };

    template <
          class Container
        , class DerivedPolicies
        , class ContainerElement
        , class Index
    >
    struct no_proxy_helper
    {                
        static void
        register_container_element()
        { 
        }

        static object
        base_get_item_(back_reference<Container&> const& container, PyObject* i)
        { 
            return object(
                DerivedPolicies::get_item(
                    container.get(), DerivedPolicies::
                        convert_index(container.get(), i)));
        }

        static void
        base_replace_indexes(
            Container& container, Index from, 
            Index to, Index n)
        {
        }

        static void
        base_erase_indexes(Container& container, Index from, Index to)
        {
        }
    };            
          
    template <
          class Container
        , class DerivedPolicies
        , class ContainerElement
        , class Index
    >
    struct proxy_helper
    {        
        static void
        register_container_element()
        { 
            register_ptr_to_python<ContainerElement>();
        }

        static object
        base_get_item_(back_reference<Container&> const& container, PyObject* i)
        { 
            // Proxy  
            Index idx = DerivedPolicies::convert_index(container.get(), i);
            
            if (PyObject* shared = 
                ContainerElement::get_links().find(container.get(), idx))
            {
                handle<> h(python::borrowed(shared));
                return object(h);
            }
            else
            {
                object prox(ContainerElement(container.source(), idx));
                ContainerElement::
                    get_links().add(prox.ptr(), container.get());
                return prox;
            }
        }

        static void
        base_replace_indexes(
            Container& container, Index from, 
            Index to, Index n)
        {
            ContainerElement::get_links().replace(container, from, to, n);
        }
        
        static void
        base_erase_indexes(
            Container& container, Index from, Index to)
        {
            ContainerElement::get_links().erase(container, from, to);
        }
    };        
    
    template <
          class Container
        , class DerivedPolicies
        , class ProxyHandler
        , class Element
        , class Index
    >
    struct slice_helper
    {        
        static object 
        base_get_slice(Container& container, PySliceObject* slice)
        { 
            Index from, to;
            base_get_slice_data(container, slice, from, to);
            return DerivedPolicies::get_slice(container, from, to);
        }

        static void
        base_get_slice_data(
            Container& container, PySliceObject* slice, Index& from, Index& to)
        {
            if (Py_None == slice->start)
                from = DerivedPolicies::get_min_index(container);
            else 
                from = DerivedPolicies::convert_index(container, slice->start);

            if (Py_None == slice->stop)
                to = DerivedPolicies::get_max_index(container);
            else 
                to = DerivedPolicies::convert_index(container, slice->stop);
        }        
   
        static void 
        base_set_slice(Container& container, PySliceObject* slice, PyObject* v)
        {
            Index from, to;
            base_get_slice_data(container, slice, from, to);
            
            extract<Element&> elem(v);
            // try if elem is an exact Element
            if (elem.check())
            {
                ProxyHandler::base_replace_indexes(container, from, to, 1);
                DerivedPolicies::set_slice(container, from, to, elem());
            }
            else
            {
                //  try to convert elem to Element
                extract<Element> elem(v);
                if (elem.check())
                {
                    ProxyHandler::base_replace_indexes(container, from, to, 1);
                    DerivedPolicies::set_slice(container, from, to, elem());
                }
                else
                {
                    //  Otherwise, it must be a list or some container
                    handle<> l_(python::borrowed(v));
                    object l(l_);
    
                    std::vector<Element> temp;
                    for (int i = 0; i < l.attr("__len__")(); i++)
                    {
                        object elem(l[i]);
                        extract<Element const&> x(elem);
                        //  try if elem is an exact Element type
                        if (x.check())
                        {
                            temp.push_back(x());
                        }
                        else
                        {
                            //  try to convert elem to Element type
                            extract<Element> x(elem);
                            if (x.check())
                            {
                                temp.push_back(x());
                            }
                            else
                            {
                                PyErr_SetString(PyExc_TypeError, 
                                    "Invalid sequence element");
                                throw_error_already_set();
                            }
                        }
                    }          
                  
                    ProxyHandler::base_replace_indexes(container, from, to, 
                        temp.end()-temp.begin());
                    DerivedPolicies::set_slice(container, from, to, 
                        temp.begin(), temp.end());
                }
            }            
        }
        
        static void 
        base_delete_slice(Container& container, PySliceObject* slice)
        { 
            Index from, to;
            base_get_slice_data(container, slice, from, to);
            ProxyHandler::base_erase_indexes(container, from, to);
            DerivedPolicies::delete_slice(container, from, to);
        }  
    };
    
    template <
          class Container
        , class DerivedPolicies
        , class ProxyHandler
        , class Element
        , class Index
    >
    struct no_slice_helper
    {        
        static void
        slicing_not_suported()
        {
            PyErr_SetString(PyExc_RuntimeError, "Slicing not supported");
            throw_error_already_set();
        }
        
        static object 
        base_get_slice(Container& container, PySliceObject* slice)
        { 
            slicing_not_suported();
            return object();
        }
   
        static void 
        base_set_slice(Container& container, PySliceObject* slice, PyObject* v)
        {
            slicing_not_suported();
        }
        
        static void 
        base_delete_slice(Container& container, PySliceObject* slice)
        { 
            slicing_not_suported();
        }  
    };

  }}  // namespace python::detail

    template <class Container, class Index, class Policies> 
    inline typename Container::value_type* 
    get_pointer(
        python::detail::container_element<Container, Index, Policies> const& p)
    {
        // Get the pointer of a container_element smart pointer
        return p.get();
    }

} // namespace boost

#endif // INDEXING_SUITE_DETAIL_JDG20036_HPP