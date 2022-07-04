#include <iostream>
#include "src/LogBlockQueue.h"
#include <string>

using namespace std;


int main() {
    BlockQueue<string> *ptr = new BlockQueue<string>;

    string s1("AAAA");
    string s2("BBBB");
    //std::cout << "11111111111" << std::endl; 
    ptr->push(s1);
    //std::cout << "22222222222" << std::endl;
    ptr->push(s2);
    //std::cout << ptr->front() << std::endl;

    return 0;
}

