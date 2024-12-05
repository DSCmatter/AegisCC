# x86-64 JIT Compiler

This project demonstrates how to dynamically generate and execute machine code at runtime using executable memory in C++. It includes examples of using machine code embedded in memory, dynamically generated assembly instructions, and calling functions from C++ at runtime.

---

## **Overview**

This program dynamically generates x86-64 assembly instructions to call a global function (`test`) that manipulates a global vector of integers. The machine code is stored in executable memory, and the program executes those instructions by casting the allocated memory to a function pointer.

---

### Prerequisites
- **Linux OS / WSL(used)**
- **g++ Compiler** (Install using: `sudo apt install g++`)

---

## **Steps to Run the Program**

1. Update the system packages:
```
sudo apt update
```
2. Install g++:
```
sudo apt install g++
```
3. Compile and run the programs:
   C++ code:
 ```
 g++ -o hello_1 hello_1.cpp
 ./hello_1
```
Assembly Code:
```
as chunk.s -o chunk.o
objdump -M intel -D chunk.o
```

### Outputs are given in 'read.txt' files in both part1 & part2 

### Contributions
Any contributions are welcome! Feel free to fork the repository, submit issues, and create pull requests for improvements or fixes.

