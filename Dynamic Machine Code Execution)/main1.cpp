#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

struct MemoryPages {
    uint8_t *mem;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t position = 0;

    MemoryPages(size_t pages_requested = 1) {
        mem = (uint8_t*)mmap(nullptr, page_size * pages_requested,
                             PROT_READ | PROT_WRITE | PROT_EXEC, 
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (mem == MAP_FAILED) {
            throw std::runtime_error("Memory allocation failed!");
        }
    }

    ~MemoryPages() { munmap(mem, page_size); }

    void push(const std::vector<uint8_t>& data) {
        if (position + data.size() > page_size) {
            throw std::runtime_error("Insufficient memory!");
        }
        std::memcpy(mem + position, data.data(), data.size());
        position += data.size();
    }

    template <typename T>
    void push(T value) {
        if (position + sizeof(T) > page_size) {
            throw std::runtime_error("Insufficient memory!");
        }
        std::memcpy(mem + position, &value, sizeof(T));
        position += sizeof(T);
    }

    void show_memory() const {
        std::cout << "Memory content (" << position << " bytes):\n";
        for (size_t i = 0; i < position; ++i) {
            printf("0x%02x ", mem[i]);
            if ((i + 1) % 16 == 0) std::cout << "\n";
        }
        std::cout << "\n";
    }
};

namespace AssemblyChunks {
    const std::vector<uint8_t> prologue = {0x55, 0x48, 0x89, 0xe5};  // push rbp; mov rbp, rsp
    const std::vector<uint8_t> epilogue = {0x5d, 0xc3};              // pop rbp; ret
}

std::vector<int> a{1, 2, 3};

void test() {
    std::cout << "Inside test()...\n";
    for (auto& e : a) e -= 5;
}

int main() {
    MemoryPages mp;

    mp.push(AssemblyChunks::prologue);
    mp.push(std::vector<uint8_t>{0x48, 0xb8});  // movabs rax, <address>
    mp.push(reinterpret_cast<size_t>(test));
    mp.push(std::vector<uint8_t>{0xff, 0xd0});  // call rax
    mp.push(AssemblyChunks::epilogue);

    mp.show_memory();

    std::cout << "Before calling test(): ";
    for (int val : a) std::cout << val << " ";
    std::cout << "\n";

    auto func = reinterpret_cast<void (*)()>(mp.mem);
    func();

    std::cout << "After calling test(): ";
    for (int val : a) std::cout << val << " ";
    std::cout << "\n";

    return 0;
}
