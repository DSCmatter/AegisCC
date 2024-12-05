#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>

// Print the content of the machine_code vector for debugging purposes
void show_machine_code(const std::vector<uint8_t>& machine_code) {
    int count = 0;
    std::cout << "\nMachine code generated:\n";
    std::cout << std::hex;
    for (auto byte : machine_code) {
        std::cout << (int)byte << " ";
        if (++count % 8 == 0) std::cout << '\n'; // Print 8 bytes per line
    }
    std::cout << std::dec << "\n";
}

int main() {
    // Get the user name
    std::string name;
    std::cout << "What is your name?\n";
    std::getline(std::cin, name);
    std::string hello_name = "Hello, " + name + "!\n";

    // Machine code template
    std::vector<uint8_t> machine_code{
        #ifdef __linux__
        0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,           // "write" syscall (Linux)
        #endif
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,           // stdout file descriptor
        0x48, 0x8d, 0x35, 0x0a, 0x00, 0x00, 0x00,           // Address of the string
        0x48, 0xc7, 0xc2, 0x00, 0x00, 0x00, 0x00,           // String length placeholder
        0x0f, 0x05,                                         // Syscall
        0xc3                                                // Return
    };

    // Append message length and content
    size_t message_size = hello_name.length();
    machine_code[24] = message_size & 0xFF;
    machine_code[25] = (message_size >> 8) & 0xFF;
    machine_code[26] = (message_size >> 16) & 0xFF;
    machine_code[27] = (message_size >> 24) & 0xFF;

    machine_code.insert(machine_code.end(), hello_name.begin(), hello_name.end());

    // Print the generated machine code
    show_machine_code(machine_code);

    // Allocate executable memory
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t required_memory_size = ((machine_code.size() + page_size - 1) / page_size) * page_size;

    uint8_t* mem = (uint8_t*)mmap(nullptr, required_memory_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        std::cerr << "Failed to allocate memory.\n";
        return 1;
    }

    // Copy machine code into allocated memory
    std::copy(machine_code.begin(), machine_code.end(), mem);

    // Execute machine code
    auto func = (void (*)())mem;
    func();

    // Release memory
    munmap(mem, required_memory_size);
    return 0;
}
