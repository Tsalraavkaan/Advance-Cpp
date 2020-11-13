#include "Service.hpp"

namespace Tasks {

namespace net {

Service::Service(IServiceListener *listener) : listener_{listener} {}

void Service::setListener(IServiceListener *listener) {
    listener_ = listener;
}

void Service::open(const std::string &addr, uint16_t port) {
    server_.open(addr, port);
    epoll_.add(server_.get_fd(), EVENT_FLAG::RDW);
}

void Service::close() {
    server_.close();
}

void Service::closeConnection(BufConnection *bufcon) {
    while(!connections_.empty()) {
        connections_.back().close();
        connections_.pop_back();
    }
}

void Service::subscribeTo(const tcp::Connection &con, EVENT_FLAG flag) {
    epoll_.mod(con.get_fd(), flag);
}

void Service::unsubscribeFrom(const tcp::Connection &con, EVENT_FLAG flag) {
    epoll_.mod(con.get_fd(), flag);
}

void Service::run() {
    while (true) {
std::cout << server_.get_fd() << std::endl;
        std::vector<::epoll_event> event_vec = epoll_.wait();
        for (::epoll_event &epevent : event_vec) {
            if (epevent.data.fd == server_.get_fd()) {
std::cout << "1" << std::endl;
                connections_.emplace_back(this, server_.accept());
std::cout << "1" << std::endl;
                epoll_.add(connections_.back().get_Desc(), EVENT_FLAG::BASIC);
                listener_->onNewConnection(&(connections_.back()));
            } else {
std::cout << "2" << std::endl;
                int fd = epevent.data.fd;
                std::_List_iterator<BufConnection> iter_client = std::find_if(connections_.begin(),
                    connections_.end(), [fd](BufConnection& client)
                                            { return client.get_Desc().get_fd() == fd; });
                if (epevent.events & EPOLLRDHUP) {
                    listener_->onClose(&*iter_client) ;
                    connections_.erase(iter_client);
                } else if (epevent.events & EPOLLERR) {
                    listener_->onError(&*iter_client);
                } else if (epevent.events & EPOLLIN) {
                    if (iter_client->put_in_read()) {
                        listener_->onReadAvailable(&*iter_client);
                    } else {
                        listener_->onError(&*iter_client);
                    }
                } else if (epevent.events & EPOLLOUT) {
                    if (!iter_client->get_write_buf().empty()) {
                        iter_client->get_from_write();
                    } else {
                        listener_->onWriteDone(&*iter_client);
                    }
                    
                }
            }
        }
    }
}


} //namespace net

} //namespace Tasks