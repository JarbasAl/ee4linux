// C:\EEX\Empire Earth\Units\EEAGIncreasingVelocityUnitSorter.cpp: NOT A UNIT. The unit
// map split 0x1ebc86..0x1f1f19 at 0x1ed9d4 from the sorter class's vtable 0x446d50, but the whole range 0x1ed9d4..0x1f1f19
// is EEActiveGroup.cpp's second half (EEActiveGroup members up to 0x1efd40, then the std template COMDATs closing that
// unit); the sorter itself (its operator() 0x1ebc47 opens EEActiveGroup.cpp) is defined there. All markers and bodies
// moved to src/Empire Earth/Units/EEActiveGroup.cpp.
