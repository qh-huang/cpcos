#include "uart.hpp"
#include <thread>

using namespace std;
using namespace cpcos;

// prompt usage message
void usage(const char *progname) {
    cout << "Usage: " << progname << " <serial port> <baud rate>" << endl;
    cout << "Example: " << progname << " /dev/ttyUSB0 115200" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return 1;
    }
    uint32_t baudrate = 115200;
    if (argc == 3) {
        // check if argv[2] is a integer
        char *endptr;
        baudrate = strtol(argv[2], &endptr, 10);
        if (*endptr != '\0') {
            usage(argv[0]);
            return 1;
        }
    }
    shared_ptr<Uart> uart = nullptr;
    try {
        uart = make_shared<Uart>(argv[1], baudrate, 2000);
    } catch (const UartException &e) {
        cerr << "UartException: " << e.what() << endl;
        return 1;
    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }
    if (!uart) {
        cerr << "failed to create uart" << endl;
        return 1;
    }
    cout << "start reading from uart ..." << endl;
    while (true) {
        std::string str = uart->ReadStr();
        if (str.size() > 0) {
            std::cout << str << std::endl;
        }
        this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}
