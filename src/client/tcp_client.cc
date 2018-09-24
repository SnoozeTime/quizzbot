//
// Created by benoit on 18/09/06.
//

#include "tcp_client.h"

namespace quizzbot {

    using boost::asio::ip::tcp;

    tcp_client::tcp_client(boost::asio::io_service &io_service,
            const std::string &server, const std::string &port,
            event_queue<command> *queue)
    : resolver_(io_service),
    socket_(io_service),
    queue_(queue) {
        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        tcp::resolver::query query(server, port);
        resolver_.async_resolve(query,
                                boost::bind(&tcp_client::handle_resolve, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::iterator));
    }

    void tcp_client::handle_resolve(const boost::system::error_code &err,
                        tcp::resolver::iterator endpoint_iterator) {
        if (!err) {
            // Attempt a connection to the first endpoint in the list. Each endpoint
            // will be tried until we successfully establish a connection.
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect(endpoint,
                                  [endpoint_iterator = ++endpoint_iterator, this] (const boost::system::error_code &err) {
                handle_connect(err, endpoint_iterator);
            });
        } else {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void tcp_client::handle_connect(const boost::system::error_code &err,
                        boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
        if (!err) {
            // The connection was successful. Now we can listen to incoming messages.
            begin_read();

        } else if (endpoint_iterator != tcp::resolver::iterator()) {
            // The connection failed. Try the next endpoint in the list.
            socket_.close();
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect(endpoint,
                                  [endpoint_iterator = ++endpoint_iterator, this] (const boost::system::error_code &err) {
                                      handle_connect(err, endpoint_iterator);
                                  });
        } else {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void tcp_client::send(const command &cmd) {
        socket_.get_io_service().post([cmd, this] {
            boost::asio::async_write(socket_, boost::asio::buffer(protocol_.pack(cmd)),
                    [] (const boost::system::error_code& err, size_t /* bytes_send */) {
                if (err) {
                    std::cerr << err.message() << std::endl;
                }
            });
        });
    }

    void tcp_client::begin_read() {
        socket_.async_read_some(boost::asio::buffer(input_buf_),
                [this] (const boost::system::error_code& error, size_t bytes_received) {

            if (!error) {
                aggr_packet_.insert(aggr_packet_.end(), input_buf_.begin(), input_buf_.begin()+bytes_received);
                handle_read();
            } else {
                std::cerr << error.message() << std::endl;
            }
        });
    }

    void tcp_client::handle_read() {
        auto maybe_msg = protocol_.parse(aggr_packet_);
        if (maybe_msg) {
            queue_->push(maybe_msg.value());
        }

        begin_read();
    }
}