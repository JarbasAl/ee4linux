// SCRATCH complete original target type, nominal pinned WONCommon/CRC.h.
// Target2a0c70 writes exactlyushort offsets0/2/4; Put2a0c90 accessesushort0.
// Source defaults/Get/Reset independently agree with private product-ID hot code.
#ifndef EE_WONAPI_WONCRC16_H
#define EE_WONAPI_WONCRC16_H
namespace WONAPI {
class CRC16 {
public:
    explicit CRC16(unsigned short inInit = 0, unsigned short inXorOut = 0); //2a0c70
    void Put(const void* inData, unsigned int inLength); //2a0c90
    unsigned short Get() const { return mRegister ^ mXOROutValue; }
    void Reset() { mRegister = mInitValue; }
private:
    unsigned short mRegister; //0
    unsigned short mInitValue; //2
    unsigned short mXOROutValue; //4; actual complete extent6, not roundedstack8
};
}
#endif