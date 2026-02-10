#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;

class CodeGenerator {
    ofstream out;
    map<string,int>variableOffsets; // 변수 이름 -> 스택 위치
    int currentOffset=0; // 사용된 스택

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
        out<<"  sub rsp, 32\n"; // 지역 변수를 위한 공간 확보
    }

    // 1. 변수 선언: 문자열을 주면 자동으로 오프셋 할당하기
    void declare(string name, int value) {
        currentOffset+=4; // int 크기만큼 이동
        variableOffsets[name]=currentOffset;
        out<<"  mov DWORD PTR [rbp-"<<currentOffset<<"], "<<value<<"    #"<<name<<" = "<<value<<"\n";
    }

    // 2. 변수 덧셈 연산 (dest = lhs + rhs)
    void add(string dest, string lhs, string rhs) {
        int lhsOffset=variableOffsets[lhs];
        int rhsOffset=variableOffsets[rhs];

        currentOffset+=4;
        variableOffsets[dest]=currentOffset;

        // 메모리끼리의 연산이 불가능하므로 eax 레지스터 이용
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

    // 2. Variable Name Declare
    // 수동으로 오프셋을 제어하던 기존 방식에서 변수 이름으로 제어하도록 변경
    gen.declare("a",10);
    gen.declare("b",20);

    // a와 b를 더해서 c에 저장하는 명령어 생성
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