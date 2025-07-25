#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>

class Server : public std::enable_shared_from_this<Server>
{
private:
    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_{8192}; // The buffer for performing reads
    boost::beast::http::request<boost::beast::http::dynamic_body> request_; // The request message.
    boost::beast::http::response<boost::beast::http::dynamic_body> response_; // The response message.
    boost::asio::steady_timer deadline_{
        socket_.get_executor(), std::chrono::seconds(60)}; // The timer for putting a deadline on connection processing.

    std::vector<unsigned int> user_id;
    static inline unsigned int user_count = 0;
public:
    Server(boost::asio::ip::tcp::socket socket);
    void start();

    void read_request();
    void process_request();
    void create_response();
    void write_response();

    void check_deadline();

};