/* Included first in every unit of the port build (tools/build_port.py), never by the VC6 build.
   It fills the gaps between the VC6 dialect and a modern compiler with its own C++ library. */
#ifndef EE_PORT_COMPAT_H
#define EE_PORT_COMPAT_H

#define NOMINMAX        /* windows.h's min and max macros break the C++ library; the templates below replace them */

#ifdef __cplusplus
#include <typeinfo>     /* VC6 allows typeid without it */
#include <type_traits>

/* windows.h's macros exactly: (((a) < (b)) ? (a) : (b)) and (((a) > (b)) ? (a) : (b)), with the usual arithmetic
   conversions of both arguments. Equal values and NaN give the same operand as the macros, which keeps the simulation
   identical. */
template <class A, class B>
inline typename std::common_type<A, B>::type min(A a, B b) { return a < b ? a : b; }
template <class A, class B>
inline typename std::common_type<A, B>::type max(A a, B b) { return a > b ? a : b; }
#endif

#endif
