//
// Created by benoit on 18/10/02.
//

#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "common/log.h"
#include "common/message.h"
#include "common/protocol.h"

namespace quizzbot {

    template<typename Msg, typename MsgProtocol> class ChatRoomBase;
    template<typename Msg, typename MsgProtocol> class GameHandlerBase;
    template<typename Msg, typename MsgProtocol> class JoinHandlerBase;

    template<typename Msg, typename MsgProtocol>
    class TcpConnectionBase : public std::enable_shared_from_this<TcpConnectionBase<Msg, MsgProtocol>> {
    public:
        using pointer = std::shared_ptr<TcpConnectionBase>;

        static pointer create_connection(
                boost::asio::io_service &io,
                ChatRoomBase<Msg, MsgProtocol> *room,
                GameHandlerBase<Msg, MsgProtocol>* game,
                JoinHandlerBase<Msg, MsgProtocol>* joiner) {
            return pointer(new TcpConnectionBase(io, room, game, joiner));
        }

        boost::asio::ip::tcp::socket& socket() {
            return socket_;
        }

        void welcome() {
            begin_read();
        }

        void send_command(const Msg& cmd) {


            boost::asio::async_write(socket_, boost::asio::buffer(protocol_.pack(cmd)),
                                     [] (const boost::system::error_code& /* error */, size_t /*bytes_transferred*/) {

                                     });
        }

        const std::string& name() const { return name_; }
        void set_name(std::string name) { name_ = std::move(name); }
    private:

        explicit TcpConnectionBase(
                boost::asio::io_service &io,
                ChatRoomBase<Msg, MsgProtocol> *room,
                GameHandlerBase<Msg, MsgProtocol>* game,
                JoinHandlerBase<Msg, MsgProtocol>* joiner):
                socket_{io},
                room_{room},
                join_handler_{joiner},
                game_handler_{game},
                protocol_{} {
        }

        void begin_read() {

            socket_.async_read_some(boost::asio::buffer(input_buf_), [this] (const boost::system::error_code& error,
                                                                             size_t bytes_received) {
                if (!error) {
                    // hum that is inefficient. Figure out a way to read directly from the input buffer...
                    LOG_DEBUG << "Received " << bytes_received << "bytes\n";
                    acc_packet_.insert(acc_packet_.end(), input_buf_.begin(), input_buf_.begin()+bytes_received);
                    handle_receive();
                } else {
                    std::cerr << error.message() << std::endl;
                }
            });
        }

        void handle_receive() {
            auto maybe_msg = protocol_.parse(acc_packet_);
            if (maybe_msg) {
                auto msg = maybe_msg.value();

                if (name_.empty()) {
                    join_handler_->handle(this->shared_from_this(), msg);
                } else {
                    room_->handle(this->shared_from_this(), msg);
                    game_handler_->handle(this->shared_from_this(), msg);
                }
            }

            begin_read();
        }

        boost::asio::ip::tcp::socket socket_;

        ChatRoomBase<Msg, MsgProtocol> *room_;
        JoinHandlerBase<Msg, MsgProtocol> *join_handler_;
        GameHandlerBase<Msg, MsgProtocol> *game_handler_;

        std::vector<uint8_t> input_buf_ = std::vector<uint8_t>(512);
        std::vector<uint8_t> acc_packet_;

        MsgProtocol protocol_;

        // name of the player. Empty at first. Need to be set before anything else. :)
        std::string name_;
    };

    using TcpConnection = TcpConnectionBase<Message, message_protocol>;

}
