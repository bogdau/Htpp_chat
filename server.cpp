#include "server.h"

Server::Server(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

void Server::check_deadline()
{
    auto self = shared_from_this();

    deadline_.async_wait(
        [self](boost::beast::error_code ec)
        {
            if (!ec)
            {
                self->socket_.close(ec);
            }
        });
}

void Server::start()
{
    read_request();
    check_deadline();
}

void Server::create_response()
{
    if (request_.target() == "/count")
    {
        response_.set(boost::beast::http::field::content_type, "text/html");
        boost::beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Request count</title></head>\n"
            << "<body>\n"
            << "<h1>Request count</h1>\n"
            << "<p>There have been "
            << 1
            << " requests so far.</p>\n"
            << "</body>\n"
            << "</html>\n";
    }
    else if (request_.target() == "/time")
    {
        response_.set(boost::beast::http::field::content_type, "text/html");
        boost::beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Current time</title></head>\n"
            << "<body>\n"
            << "<h1>Current time</h1>\n"
            << "<p>The current time is "
            << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
            << " seconds since the epoch.</p>\n"
            << "</body>\n"
            << "</html>\n";
    }
    else
    {
        response_.result(boost::beast::http::status::not_found);
        response_.set(boost::beast::http::field::content_type, "text/plain");
        boost::beast::ostream(response_.body()) << "File not found\r\n";
    }
}

void Server::process_request()
{
    response_.version(request_.version());
    response_.keep_alive(false);

    switch (request_.method())
    {
    case boost::beast::http::verb::get:
        response_.result(boost::beast::http::status::ok);
        response_.set(boost::beast::http::field::server, "Beast");
        create_response();
        break;

    default:
        // We return responses indicating an error if
        // we do not recognize the request method.
        response_.result(boost::beast::http::status::bad_request);
        response_.set(boost::beast::http::field::content_type, "text/plain");
        boost::beast::ostream(response_.body())
            << "Invalid request-method '"
            << std::string(request_.method_string())
            << "'";
        break;
    }

    write_response();
}

void Server::read_request()
{
    auto self = shared_from_this();
    
    Server::user_count += 1;
    user_id.push_back(Server::user_count);
    for(auto itr: user_id){
        std::cout << itr << " " ;
    }
    std::cout << std::endl;
    std::cout << "New user was connected!" << std::endl;
    // boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint remote_endpoint = socket_.remote_endpoint();
    std::cout << "Client connected from: " << remote_endpoint.address().to_string() << ":" << remote_endpoint.port() << std::endl;

    boost::beast::http::async_read(
        socket_,
        buffer_,
        request_,
        [self](boost::beast::error_code ec,
               std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);
            if (!ec)
                self->process_request();
        });
}

void Server::write_response()
{
    auto self = shared_from_this();

    response_.content_length(response_.body().size());

    boost::beast::http::async_write(
        socket_,
        response_,
        [self](boost::beast::error_code ec, std::size_t)
        {
            self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            self->deadline_.cancel();
        });
}
