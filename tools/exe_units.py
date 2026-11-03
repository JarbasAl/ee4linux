"""COFF object reader."""
import pathlib
import struct
import sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))


class CoffEdit:

    def __init__(self, path):
        self.b = bytearray(pathlib.Path(path).read_bytes())
        b = self.b
        self.nsec, _, self.symptr, self.nsym, self.opt, _ = struct.unpack_from("<HIIIHH", b, 2)
        self.secs = []
        for i in range(self.nsec):
            o = 20 + self.opt + 40 * i
            name = bytes(b[o:o + 8]).rstrip(b"\0").decode(errors="replace")
            size, rawptr, relptr, _, nrel, _, chars = struct.unpack_from("<IIIIHHI", b, o + 16)
            self.secs.append({"num": i + 1, "hdr": o, "name": name, "size": size, "raw": rawptr, "relptr": relptr,
                              "nrel": nrel, "chars": chars})
        self.strtab_at = self.symptr + 18 * self.nsym
        self.strtab = bytearray(b[self.strtab_at:])
        self.syms = {}
        i = 0
        while i < self.nsym:
            o = self.symptr + 18 * i
            raw = bytes(b[o:o + 8])
            name = (raw.rstrip(b"\0") if raw[:4] != b"\0\0\0\0" else
                    bytes(self.strtab[struct.unpack_from("<I", raw, 4)[0]:]).split(b"\0", 1)[0]).decode(errors="replace")
            value, sec, typ, cls, naux = struct.unpack_from("<IhHBB", b, o + 8)
            self.syms[i] = {"idx": i, "off": o, "name": name, "value": value, "sec": sec, "type": typ, "cls": cls,
                            "naux": naux}
            i += 1 + naux
        self.new_syms = []

    def relocs(self, sec):
        for k in range(sec["nrel"]):
            o = sec["relptr"] + 10 * k
            va, si, typ = struct.unpack_from("<IIH", self.b, o)
            yield o, va, si, typ

    def code(self, sec):
        return self.b[sec["raw"]:sec["raw"] + sec["size"]]

    def rename_static(self, sym, name):
        assert len(name) <= 8
        o = sym["off"]
        self.b[o:o + 8] = name.encode().ljust(8, b"\0")
        self.b[o + 16] = 3
        sym["name"], sym["cls"] = name, 3

    def add_undefined(self, name, func):
        for idx, n, _ in self.new_syms:
            if n == name:
                return idx
        idx = self.nsym + len(self.new_syms)
        self.new_syms.append((idx, name, func))
        return idx

    def write(self, path):
        strtab = bytearray(self.strtab) if self.strtab else bytearray(4)
        recs = b""
        for idx, name, func in self.new_syms:
            if len(name) <= 8:
                raw = name.encode().ljust(8, b"\0")
            else:
                raw = struct.pack("<II", 0, len(strtab))
                strtab += name.encode() + b"\0"
            recs += raw + struct.pack("<IhHBB", 0, 0, 0x20 if func else 0, 2, 0)
        struct.pack_into("<I", strtab, 0, len(strtab))
        out = bytearray(self.b[:self.strtab_at]) + recs + strtab
        struct.pack_into("<I", out, 12, self.nsym + len(self.new_syms))
        pathlib.Path(path).write_bytes(bytes(out))
