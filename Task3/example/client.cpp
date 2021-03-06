#include <iostream>
#include "tcp.hpp"
#include <cstring>

int main() {
    std::string str, temp;
    while(std::cin >> temp) {
        str += temp + " ";
    }
    Tasks::tcp::Connection connection;
    connection.connect("127.127.127.127", 8088);
    connection.set_timeout(1000);
    size_t size = str.size();
    connection.writeExact(&size, sizeof(size));
    connection.writeExact(str.data(), str.size());
    connection.close();
    return 0;
}