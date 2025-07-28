#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    explicit WebSocketSession(net::ip::tcp::socket socket)
        : ws_(std::move(socket)) {}

    void run(beast::http::request<beast::http::dynamic_body> req) {
        // Accept the WebSocket handshake
        ws_.async_accept(req,
            beast::bind_front_handler(
                &WebSocketSession::on_accept,
                shared_from_this()));
    }

private:
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;

    void on_accept(beast::error_code ec) {
        if (ec) {
            std::cerr << "WebSocket accept failed: " << ec.message() << "\n";
            return;
        }

        do_read();
    }

    void do_read() {
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &WebSocketSession::on_read,
                shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == websocket::error::closed) return;
        if (ec) {
            std::cerr << "Read error: " << ec.message() << "\n";
            return;
        }

        std::string msg = beast::buffers_to_string(buffer_.data());
        std::cout << "Received: " << msg << "\n";

        // Echo back
        ws_.text(ws_.got_text());
        ws_.async_write(
            buffer_.data(),
            beast::bind_front_handler(
                &WebSocketSession::on_write,
                shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            std::cerr << "Write error: " << ec.message() << "\n";
            return;
        }

        buffer_.consume(buffer_.size()); // Clear buffer
        do_read(); // Continue reading
    }
};