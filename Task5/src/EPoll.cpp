#include "EPoll.hpp"
#include "Exceptions.hpp"

namespace Tasks {

namespace net {

void throw_epollerr(bool cond, const std::string& what) {
    if (cond) {
        throw EPollError(what + " errno: " + std::strerror(errno));
    }
}

EPoll::EPoll() : epfd{::epoll_create(1)} {
    throw_epollerr(!epfd, "Error in creating epoll");
}

void EPoll::mod(int fd, EVENT_FLAG flag) const {
    ::epoll_event event{};
    event.events = static_cast<uint32_t>(flag);
    event.data.fd = fd;
    int res = epoll_ctl(epfd.get_fd(), EPOLL_CTL_MOD, fd, &event);
    throw_epollerr(res == -1, "Error in adding register");
}

void EPoll::add(int fd, EVENT_FLAG flag) const {
    ::epoll_event event{};
    event.events = static_cast<uint32_t>(flag);
    event.data.fd = fd;
    int res = epoll_ctl(epfd.get_fd(), EPOLL_CTL_ADD, fd, &event);
    throw_epollerr(res == -1, "Error in changing register");
}

void EPoll::del(int fd) const {
    int res = epoll_ctl(epfd.get_fd(), EPOLL_CTL_DEL, fd, nullptr);
    throw_epollerr(res == -1, "Error in removing register");
}

std::vector<::epoll_event> EPoll::wait(int millisec, int maxevents) {
    std::vector<::epoll_event> event_vec(maxevents);
    int cnt_events = ::epoll_wait(epfd.get_fd(), event_vec.data(), event_vec.size(), millisec);
    throw_epollerr(cnt_events == -1, "Error in waiting for events");
    event_vec.resize(cnt_events);
    return event_vec;
}

} //namespace net

} //namespace Tasks