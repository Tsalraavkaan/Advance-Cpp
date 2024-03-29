#include "EPoll.hpp"
#include "Exceptions.hpp"
#include <cstring>

namespace Project {

namespace net {

void throw_epollerr(bool cond, const std::string& what) {
    if (cond) {
        throw Except::EPollError(what + " errno: " + std::strerror(errno));
    }
}

EPoll::EPoll() : epfd{::epoll_create(1)} {
    throw_epollerr(!epfd, "Error in creating epoll");
}

void EPoll::mod(int fd, EVENT_FLAG flag) const {
    ::epoll_event event{};
    event.events = static_cast<uint32_t>(flag) | EPOLLRDHUP;
    event.data.fd = fd;
    int res = epoll_ctl(epfd.getFd(), EPOLL_CTL_MOD, fd, &event);
    throw_epollerr(res == -1, "Error in changing register");
}

void EPoll::add(int fd, EVENT_FLAG flag) const {
    ::epoll_event event{};
    event.events = static_cast<uint32_t>(flag) | EPOLLRDHUP;
    event.data.fd = fd;
    int res = epoll_ctl(epfd.getFd(), EPOLL_CTL_ADD, fd, &event);
    throw_epollerr(res == -1, "Error in adding register");
}

void EPoll::del(int fd) const {
    int res = epoll_ctl(epfd.getFd(), EPOLL_CTL_DEL, fd, nullptr);
    throw_epollerr(res == -1, "Error in removing register");
}

std::vector<::epoll_event> EPoll::wait(int millisec, size_t maxevents) const {
    std::vector<::epoll_event> event_vec(maxevents);
    int cnt_events = ::epoll_wait(epfd.getFd(), event_vec.data(), event_vec.size(), millisec);
    throw_epollerr(cnt_events == -1, "Error in waiting for events");
    event_vec.resize(cnt_events);
    return event_vec;
}

} //namespace net

} //namespace Project