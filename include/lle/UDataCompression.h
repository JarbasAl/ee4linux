// C:\EEX\Low-Level Engine\Utilities\UDataCompression.h (file name inferred): wraps the PKWARE Data Compression
// Library (implode/explode). Layout proven by its unit (0x10056369..0x1005662f) (operator new(0xc) in
// FSMemoryFile::Compress 0xa05e).
#ifndef LLE_UDATACOMPRESSION_H
#define LLE_UDATACOMPRESSION_H

class LLE_API UDataCompression {
public:
    UDataCompression();                                                         // 0x56369
    virtual ~UDataCompression();                                                // 0x56393
    // implicit copy ctor (0x4020, emitted with File System.cpp's inlines) and operator= (0x12548)
    long Compress(const char* inData, unsigned long inLength, char* outData, unsigned long inCapacity,
                  unsigned long& outLength, unsigned long inLevel);             // 0x563b0 (provisional names)
    long Decompress(const char* inData, unsigned long inLength, char* outData, unsigned long inCapacity,
                    unsigned long& outLength);                                  // 0x564b9
    long GetDecompressedSize(const char* inData, unsigned long inLength, unsigned long& outSize);   // 0x56544
    bool GetIsCompressed(const char* inData, unsigned long inLength);           // 0x5657f

private:
    // [0] +0x04 implode work buffer, new char[0x8dd8] (PKWARE CMP_BUFFER_SIZE); [1] +0x08 explode work buffer,
    // new char[0x3134] (EXP_BUFFER_SIZE). An array, not two members: two scalar members change File System.cpp's
    // implicit-copy-ctor emission and $E numbering (72 -> 64 exact).
    char* mWorkBuffers[2];
};

#endif
