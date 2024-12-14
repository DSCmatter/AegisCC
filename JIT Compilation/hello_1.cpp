// hello_1.cpp 
#include <iostream>
#include <string>
#include <unistd.h>

int main(){
    // Get the user name 
    std::string name;
    std::cout << "What is your name?\n";
    std::getline(std::cin, name);
    std::string hello_name = "Hello, " + name + "!\n";

    //Greet the user 
    // std::cout << hello_name; -> Normally 
    // In OS 'write' function:
    write(STDIN_FILENO, (const void *) hello_name.c_str(), hello_name.size());
}