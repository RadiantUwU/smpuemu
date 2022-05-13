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
        {"hlt",0 }, {"nop",1 },

        {"adr",2 },
        {"lda",3 }, {"sta",4 }, {"ldb",5 }, {"swp",6 },
        {"ldh",7 }, {"ldl",8 }, {"sth",9 }, {"stl",10}, {"ldq",11}, {"stq",12},
        {"cla",13}, {"clb",14}, {"clq",15},

        {"add",20},{"addc",21}, {"sub",22},{"subc",23},
        {"shl",24},{"shlc",25}, {"shr",26},{"shrc",27},
        {"and",28}, {"or",29},  {"xor",30},{"nand",31}, {"nor",32},{"xnor",33},
        {"cksm",34},{"cksmc",35},
        {"incr",36},{"decr",37},

        {"jmp",38},{"jmpc",39},{"jmpz",40},{"jmpq",41},
        {"psh",42},{"pop",43},{"subr",44},{"ret",45}
    };
    a.addrlen = 2;
    a.addDef("stb","swp ldb swp");
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