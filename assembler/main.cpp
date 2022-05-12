#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "../assembler.hpp"
#include "../colors.hpp"

//#define NDEBUG

Assembler a;
void init_asm() {
    a.consts = {
        {"HLT",0 }, {"NOP",1 },

        {"ADR",2 },
        {"LDA",3 }, {"STA",4 }, {"LDB",5 }, {"SWP",6 },
        {"LDH",7 }, {"LDL",8 }, {"STH",9 }, {"STL",10}, {"LDQ",11}, {"STQ",12},
        {"CLA",13}, {"CLB",14}, {"CLQ",15},

        {"ADD",20},{"ADDC",21}, {"SUB",22},{"SUBC",23},
        {"SHL",24},{"SHLC",25}, {"SHR",26},{"SHRC",27},
        {"AND",28}, {"OR",29},  {"XOR",30},{"NAND",31}, {"NOR",32},{"XNOR",33},
        {"CKSM",34},{"CKSMC",35},
        {"INCR",36},{"DECR",37},

        {"JMP",38},{"JMPC",39},{"JMPZ",40},{"JMPQ",41},
        {"PSH",42},{"POP",43},{"SUBR",44},{"RET",45}
    };
    a.addrlen = 2;
}
int main(int argc, char** argv) {
    init_asm();
    init_color();
    bool makinginf = false;
    bool makingouf = false;
    char* infile = nullptr;
    char* outfile = nullptr;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    makinginf = true;
                    break;
                case 'o':
                    makingouf = true;
                    break;
                case 'v':
                    a.verbose = true;
            }
            continue;
        }
        if (makinginf) {
            infile = argv[i];
            makinginf = false;
        } else if (makingouf) {
            outfile = argv[i];
            makingouf = false;
        }
    }
    #ifndef NDEBUG
        infile = (char*)"input.txt";
        outfile = (char*)"rom.txt";
        a.verbose = true;
    #endif
    if (infile == nullptr) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "No input file given." COLOR_RESET << std::endl;
        return 1;
    } else if (outfile == nullptr) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "No output file given." COLOR_RESET << std::endl;
        return 1;
    }
    std::ifstream inpfile(infile);
    if (!inpfile.is_open()) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "Could not open input file." COLOR_RESET << std::endl;
        return 1;
    }
    std::ofstream outpfile(outfile);
    if (!outpfile.is_open()) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "Could not open output file." COLOR_RESET << std::endl;
        inpfile.close();
        return 1;
    }
    char v;
    std::string af;
    while (true) {
        inpfile.get(v);
        if (inpfile.eof()) break;
        af += v;
    }
    inpfile.close();
    std::vector<unsigned char> out = a.build(af);
    std::stringstream ss;
    ss << std::hex;
    for (unsigned char c : out) {
        ss << (unsigned int)c << " ";
    }
    std::string out_str = ss.str();
    out_str.pop_back();
    outpfile << out_str;
    outpfile.close();
    return 0;
}