#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;

class CodeGenerator {
    ofstream out;
    map<string,int>variableOffsets;
    int currentOffset=0;

public:
    CodeGenerator(string filename) {
        out.open(filename);
    }

    void start() {
        out<<".intel_syntax noprefix\n";
        out<<".global main\n\n";
        out<<"main:\n";
        out<<"  push rbp\n";
        out<<"  mov rbp, rsp\n";
        out<<"  sub rsp, 32\n";
    }

    void declare(string name, int value) {
        currentOffset+=4;
        variableOffsets[name]=currentOffset;
        out<<"  mov DWORD PTR [rbp-"<<currentOffset<<"], "<<value<<"    #"<<name<<" = "<<value<<"\n";
    }

    void add(string dest, string lhs, string rhs) {
        int lhsOffset=variableOffsets[lhs];
        int rhsOffset=variableOffsets[rhs];

        currentOffset+=4;
        variableOffsets[dest]=currentOffset;

        out<<"  mov eax, DWORD PTR [rbp-"<<lhsOffset<<"]\n";
        out<<"  add eax, DWORD PTR [rbp-"<<rhsOffset<<"]\n";
        out<<"  mov DWORD PTR [rbp-"<<currentOffset<<"], eax    # "<<dest<<" = "<<lhs<<" + "<<rhs<<"\n";
    }

    void assign(int offset, int value) {
        out<<"  mov DWORD PTR [rbp-"<<offset<<"], "<<value<<"\n";
    }

    void end() {
        out<<"  mov eax, 0\n";
        out<<"  leave\n";
        out<<"  ret\n";
        out.close();
    }
};

int main() {
    string asmFile="output.s";
    CodeGenerator gen(asmFile);

    // 1. Compile Start
    gen.start();

    // 2-1. Integer Variable Declare
    /*
     *gen.declare(4,10); // save from rbp-4
    gen.declare(8,20); // save from rbp-8
    */

    // 2-2. Variable Name Declare
    gen.declare("a",10);
    gen.declare("b",20);

    gen.add("c","a","b");

    // 3. Compile End
    gen.end();

    // 4. g++ build
    cout<<"Assembly file generated: "<<asmFile<<endl;
    string compileCmd="g++ "+asmFile+" -o my_program";

    int result=system(compileCmd.c_str());

    if (result==0) {
        cout<<"Build Success! Run './my_program'"<<endl;
    } else {
        cout<<"Build Failed."<<endl;
    }
    return 0;
}