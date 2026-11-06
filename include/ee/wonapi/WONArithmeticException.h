// Complete actual ArithmeticException18, genuine SDK owner and native RTTI/throw/dtor.
// Only new arithmetic/crypto implementation TUs include this type; generated lifetime0quota.
#ifndef EE_WONAPI_WONARITHMETICEXCEPTION_H
#define EE_WONAPI_WONARITHMETICEXCEPTION_H
#include <exception>
#include <string>
namespace WONAPI {
class ArithmeticException : public std::exception {
public:
    std::string what; //08..18 actual message owner; intentionally hides inherited what()
    ArithmeticException() : exception() {}
    ArithmeticException(const std::string& message) { what=message; }
};
}
#endif
