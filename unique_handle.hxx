///
/// Talk is cheap. Show me the code.    - Linus Torvalds
///
/// Author: WiNGC (wingc@wingc.net)
/// All rights reserved
///

#pragma once

#ifndef _UNIQUE_HANDLE_HXX
#define _UNIQUE_HANDLE_HXX
///
/// This is a template to accomplish auto resource release.
/// When we use it we have to use a type and the type specific invalid value and resource release function.
/// Most of this auto management concept and code are from Kenny Kerr's
/// <C++ and the Windows API> http://msdn.microsoft.com/en-us/magazine/hh288076.aspx
/// which wraps the type's default invlaid value and the how to close it. But this impl takes the invalid
/// value and the release function as template parameter directly. It does not need to delcare a traits
/// class.
///
/// E.g. On Windows platform, we want to wrap file handle HANDLE and its closer CloseHandle() we just need
///
/// typedef unique_handle<HANDLE, decltype(::CloseHandle), INVALID_HANDLE_VALUE, ::CloseHandle> unique_file;
///
/// and then
///
/// unique_file file;
/// file = ::CreateFile2(...);
///
/// Note: 
///     - Windows platform specific code I always we UpperCamelCased naming convention.
///     - The unique_handle tempate itself is platform independent I place this under Util
//         and follow lower_underscore_cased naming.
//      - The platform specific code like the unique_file definition is placed under Platform/Win32.

template<typename _T,
    typename _D,
    _T _invalid,
    _D _deleter>
class unique_handle
{
    // Copy constructor not allowed.
    unique_handle(unique_handle const&);
    // Assignment not allowed.
    unique_handle& operator=(unique_handle const&);
    void close() throw()
    {
        if (is_valid())
        {
            _deleter(m_value);
            m_value = _invalid;
        }
    }
    _T m_value;
    _D m_deleter;
public:
    explicit unique_handle(_T value = _invalid, _D deleter = _deleter) throw() : m_value(value), m_deleter(_deleter) {};
    ~unique_handle() throw() { close(); };
    bool is_valid() throw() { return (m_value != _invalid); };
    bool reset(_T value = _invalid) throw()
    {
        close();
        m_value = value;
        return is_valid();
    }
    unique_handle& operator=(_T value)
    {
        reset(value);
        return *this;
    }
    _T get() const throw() { return m_value; }
    _T release() throw()
    {
        _T value = m_value;
        m_value = _invalid;
        return value;
    }
    _T* address_of() throw() { return &m_value; }
};

#endif  // _UNIQUE_HANDLE_HXX