#include "uart.hpp"
#include <thread>

using namespace std;
using namespace cpcos;

int main(int argc, char* argv[]) {
    shared_ptr<Uart> uart = make_shared<Uart>("/dev/ttymxc3", 115200, 2000);
    while (true) {
        std::string str = uart->ReadStr();
        if (str.size() > 0) {
            std::cout << str << std::endl;
        }
        this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}