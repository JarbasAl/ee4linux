// EEVectorIO.h (RECONSTRUCTION; file name inferred; every name provisional). Function templates that load and save a
// vector
// through an FSFile: a size, then the elements. The key is taken by value and two derived names ("<key>-Size",
// "<key>-Element") are built and never used (a text-format variant's leftovers). First instantiated for
// vector<long> by EECalamity.cpp (COMDATs 0xadb9f, 0xadc3d). Requires lle/LowLevelEngine.h, <vector>.
#ifndef EE_EEVECTORIO_H
#define EE_EEVECTORIO_H

template <class T> long FSLoadVector(FSFile& inFile, std::vector<T>& outVector, UString inKey)
{
    UString theSizeKey = inKey + "-Size";
    UString theElementKey = inKey + "-Element";
    unsigned long theSize = 0;
    T theElement;
    long theResult = inFile.Read(theSize);
    for (unsigned long i = 0; i < theSize; i++) {
        theResult = inFile.Read(theElement);
        outVector.insert(outVector.end(), theElement);
    }
    return theResult;
}

template <class T> long FSSaveVector(FSFile& outFile, std::vector<T>& inVector, UString inKey)
{
    UString theSizeKey = inKey + "-Size";
    UString theElementKey = inKey + "-Element";
    long theResult = outFile.Write((unsigned long)inVector.size());
    for (typename std::vector<T>::iterator it = inVector.begin(); it != inVector.end() && theResult == kErrorOK; it++)
        theResult = outFile.Write(*it);
    return theResult;
}

#endif
