#include "Connection.hpp"

namespace Tasks {

Connection::Connection(int fd) : sock_fd_{fd} {}

Connection::Connection(const std::string &addr, uint16_t port) : sock_fd_{-1} {
    connect(addr, port);
}

Connection::Connection(Connection &&connection) :
        sock_fd_{std::move(connection.sock_fd_.get_fd())} {
    connection.sock_fd_.set_fd(-1);
}

Connection &Connection::operator=(Connection &&connection) {
    sock_fd_ = std::move(connection.sock_fd_);
    connection.sock_fd_.set_fd(-1);
    return *this;
}

void Connection::set_timeout(long microsec) {
    timeval timeout{.tv_sec = 0, .tv_usec = microsec};
    if (setsockopt(sock_fd_.get_fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0 || 
            setsockopt(sock_fd_.get_fd(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw Tasks::TimeoutError("Error in timeout");
    }

}

void Connection::connect(const std::string &addr, uint16_t port) {
    if (sock_fd_) {
        throw Tasks::DescriptorError("Socket is already opened");
    }
    sock_fd_.open();

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    if (::inet_aton(addr.c_str(), &sock_addr.sin_addr) == 0) {
        throw Tasks::BaseException("Invalid Ip address");
    }
    if (::connect(sock_fd_.get_fd(), reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr)) < 0) {
        throw ConnectionError("Error connecting to " + addr + " " + std::to_string(port));
    }
}

void Connection::close() {
    sock_fd_.close();
}

size_t Connection::write(const void *data, size_t len) {
    if (!sock_fd_) {
        throw Tasks::DescriptorError("Write descriptor is closed");
    }
    ssize_t num_written = ::write(sock_fd_.get_fd(), data, len);
    if (num_written < 0) {
        throw Tasks::WritingError("Error writing into process");
    } else {
        return static_cast<size_t> (num_written);
    }

}

void Connection::writeExact(const void *data, size_t len) {
    size_t num_written = 0;
    while(num_written != len) {
        num_written += write(static_cast<const char*>(data) + num_written, len - num_written);
    }
}

size_t Connection::read(void *data, size_t len) {
    if (!sock_fd_) {
        throw Tasks::DescriptorError("Read descriptor is closed");
    }
    ssize_t num_read = ::read(sock_fd_.get_fd(), data, len);
    if (num_read < 0) {
        throw Tasks::ReadingError("Error reading from process");
    } else {
        return static_cast<size_t> (num_read);
    }

}

void Connection::readExact(void *data, size_t len) {
    size_t num_read = 0;
    while(num_read != len) {
        ssize_t recived = read(static_cast<char*>(data) + num_read, len - num_read);
        if (recived == 0) {
            throw Tasks::ReadingError("EOF is reached before comlete reading");
        }
        num_read += recived;
    }
}

} //namespace Tasks