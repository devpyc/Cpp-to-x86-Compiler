#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;

class CodeGenerator {
    ofstream out;

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

    // 2. Variable Declare
    gen.assign(4,10); // save from rbp-4
    gen.assign(8,20); // save from rbp-8

    // 3. Compile End
    gen.end();

    // 4. g++ build
    cout<<"Assembly file generated: "<<asmFile<<endl;
    string compileCmd="g++ "+asmFile+" -o main";

    int result=system(compileCmd.c_str());

    if (result==0) {
        cout<<"Build Success! Run './main'"<<endl;
    } else {
        cout<<"Build Failed."<<endl;
    }

    return 0;
}