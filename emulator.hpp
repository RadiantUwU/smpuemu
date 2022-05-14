#pragma once

#include <memory>

#include "configdef.hpp"

inline unsigned long long positiveModulo(unsigned long long a, unsigned long long b) {
    return (a % b + b) % b;
}

class Memory {
    char* mem = nullptr;
    size_t size;
public:
    Memory() {
        size = Config::memlen * 1024;
        mem = (char*)malloc(size);
    }
    Memory(size_t size_) {
        size = size_;
        mem = (char*)malloc(size);
    }
    Memory(size_t size_, unsigned char* mem) : size(size_) {
        this->mem = (char*)malloc(size);
        memcpy(this->mem, mem, size);
    }
    ~Memory() {
        free(mem);
    }
    unsigned char& operator[] (unsigned long a) {
        return (unsigned char&)mem[a];
    }
    size_t getsize() {
        return size;
    }
    Memory& operator= (const Memory& other) {
        if (this == &other) return *this;
        size = other.size;
        free(mem);
        mem = (char*)malloc(size);
        memcpy(mem, other.mem, size);
        return *this;
    }
};

class StackRegister : protected Memory {
    size_t sp = 0;
public:
    StackRegister() : Memory(Config::stacklen) {}
    StackRegister(size_t size_) : Memory(size_) {}
    using Memory::getsize;
    void push(unsigned char c) {
        (*this)[positiveModulo(sp++,getsize())] = c;
    }
    unsigned char pop() {
        return (*this)[positiveModulo(--sp,getsize())];
    }
    void reset() {
        sp = 0;
    }
    size_t getsp() {
        return sp;
    }
    void setsp(size_t s) {sp = s;}
};
class ALU final {
    unsigned char& geta();
    unsigned char& getb();
    unsigned char& geth();
    unsigned char& getl();
    unsigned char& getq();
    bool c = false;
public:
    void reset() {c = false;}
    bool getc() {return c;}
    bool getz() {return getq() == 0;}
    void swap() {
        unsigned char a = geta();
        unsigned char b = getb();
        geta() = b;
        getb() = a;
    }
    void add() {
        unsigned char a = geta();
        unsigned char b = getb();
        geta() = a + b;
        c = (a + b) > 0xFF;
    }
    void sub() {
        unsigned char a = geta();
        unsigned char b = getb();
        geta() = a - b;
        c = (a - b) < 0;
    }
    void addc() {
        unsigned char a = geta();
        unsigned char b = getb();
        geta() = a + b + c;
        c = (a + b + c) > 0xFF;
    }
    void subc() {
        unsigned char a = geta();
        unsigned char b = getb();
        geta() = a - b - c;
        c = (a - b - c) < 0;
    }
    void loadb() {
        getb() = geta();
    }
    void loadh() {
        geth() = geta();
    }
    void loadl() {
        getl() = geta();
    }
    void loadq() {
        getq() = geta();
    }
    void storeh() {
        geta() = geth();
    }
    void storel() {
        geta() = getl();
    }
    void storeq() {
        geta() = getq();
    }
    void shiftleft() {
        unsigned char a = geta();
        geta() = a << 1;
        c = (a & 0x80) != 0;
    }
    void shiftright() {
        unsigned char a = geta();
        geta() = a >> 1;
        c = (a & 0x01) != 0;
    }
    void shiftrightc() {
        unsigned char a = geta();
        geta() >>= 1;
        if (c) {
            geta() |= 0x80;
        }
        c = (a & 0x01) != 0;
    }
    void shiftleftc() {
        unsigned char a = geta();
        geta() <<= 1;
        if (c) {
            geta() |= 0x01;
        }
        c = (a & 0x80) != 0;
    }
    void andb() {
        geta() &= getb();
        c = false;
    }
    void orb() {
        geta() |= getb();
        c = false;
    }
    void xorb() {
        geta() ^= getb();
        c = false;
    }
    void nandb() {
        geta() = ~(geta() & getb());
        c = false;
    }
    void norb() {
        geta() = ~(geta() | getb());
        c = false;
    }
    void xnorb() {
        geta() = ~(geta() ^ getb());
        c = false;
    }
    void checksum() {
        // parity of a
        unsigned char a = geta();
        unsigned char p = 0;
        for (int i = 0; i < 8; i++) {
            p ^= (a & 0x01);
            a >>= 1;
        }
        c = p != 0;
    }
    void checksumc() {
        // parity of a
        unsigned char a = geta();
        unsigned char p = 0;
        for (int i = 0; i < 8; i++) {
            p ^= (a & 0x01);
            a >>= 1;
        }
        c = (p ^ c) != 0;
    }
    void increment() {
        getq()++;
    }
    void decrement() {
        getq()--;
    }
};
class Emulator {
    friend class ALU;
    unsigned char a,b,h,l,q = 0;
    unsigned short pc = 0;
    StackRegister stack;
    Memory memory;
    ALU alu;
    bool halted = false;
    Emulator() {}
    Emulator(const Emulator&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(const Emulator&) = delete;
    Emulator& operator=(Emulator&&) = delete;
    unsigned short makeadr() {
        return h * 256 + l;
    }
public:
    void reset() {
        stack.reset();
        alu.reset();
        a = b = h = l = q = 0;
        pc = 0;
        halted = false;
    }
    void load(unsigned char* mem, size_t size) {
        memory = Memory(size,mem);
    }
    bool ishalted() {return halted;}
    void step() {
        unsigned char opcode = memory[++pc];
        switch (opcode) {
            case 0x00:
                //halt
                halted = true;
                break;
            case 0x01:
                //nop
                break;
            case 0x02:
                //set address
                h = memory[++pc];
                l = memory[++pc];
                break;
            case 0x03:
                //load a
                a = memory[makeadr()];
                break;
            case 0x04:
                //store a
                memory[makeadr()] = a;
                break;
            case 0x05:
                //load b
                alu.loadb();
                break;
            case 0x06:
                //swap
                alu.swap();
                break;
            case 0x07:
                //load h
                alu.loadh();
                break;
            case 0x08:
                //load l
                alu.loadl();
                break;
            case 0x09:
                //store h
                alu.storeh();
                break;
            case 0x0A:
                //store l
                alu.storel();
                break;
            case 0x0B:
                //load q
                alu.loadq();
                break;
            case 0x0C:
                //store q
                alu.storeq();
                break;
            case 0x0D:
                //load A from memory
                a = memory[++pc];
                break;
            case 0x0E:
                //load B from memory
                b = memory[++pc];
                break;
            case 0x0F:
                //load Q from memory
                q = memory[++pc];
                break;
            case 0x14:
                //add
                alu.add();
                break;
            case 0x15:
                //add carry
                alu.addc();
                break;
            case 0x16:
                //subtract
                alu.sub();
                break;
            case 0x17:
                //subtract carry
                alu.subc();
                break;
            case 0x18:
                //shift left
                alu.shiftleft();
                break;
            case 0x19:
                //shift left carry
                alu.shiftleftc();
                break;
            case 0x1A:
                //shift right
                alu.shiftright();
                break;
            case 0x1B:
                //shift right carry
                alu.shiftrightc();
                break;
            case 0x1C:
                //and
                alu.andb();
                break;
            case 0x1D:
                //or
                alu.orb();
                break;
            case 0x1E:
                //xor
                alu.xorb();
                break;
            case 0x1F:
                //nand
                alu.nandb();
                break;
            case 0x20:
                //nor
                alu.norb();
                break;
            case 0x21:
                //xnor
                alu.xnorb();
                break;
            case 0x22:
                //checksum
                alu.checksum();
                break;
            case 0x23:
                //checksum carry
                alu.checksumc();
                break;
            case 0x24:
                //increment
                alu.increment();
                break;
            case 0x25:
                //decrement
                alu.decrement();
                break;
            case 0x26:
                //jump
                pc = makeadr();
                break;
            case 0x27:
                //jump if carry
                if (alu.getc()) pc = makeadr();
                break;
            case 0x28:
                //jump if zero
                if (alu.getz()) pc = makeadr();
                break;
            case 0x29:
                //jump if not zero
                if (!alu.getz()) pc = makeadr();
                break;
            case 0x2A:
                //push a to stack
                stack.push(a);
                break;
            case 0x2B:
                //pop a from stack
                a = stack.pop();
                break;
            case 0x2C:
                //jump subroutine
                stack.push(pc >> 8);
                stack.push(pc & 0xFF);
                pc = makeadr();
                break;
            case 0x2D:
                //return from subroutine
                pc = stack.pop() + stack.pop() * 256;
                break;

        }
    }
    void run() {
        while (!halted) {
            step();
        }
    }
} emu;
unsigned char& ALU::geta() {
    return emu.a;
}
unsigned char& ALU::getb() {
    return emu.b;
}
unsigned char& ALU::geth() {
    return emu.h;
}
unsigned char& ALU::getl() {
    return emu.l;
}
unsigned char& ALU::getq() {
    return emu.q;
}
