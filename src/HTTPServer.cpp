#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>
#include <string>
#include <memory>
#include <vector>

#include "HTTPServer.hpp"
#include "RequestAnalyzer.hpp"
#include "ResponseBuilder.hpp"
#include "../liblogger/liblogger.hpp"

namespace HTTP_Server
{

	// Constructor to initialize port and buffer_size with default values
	HTTPServer::HTTPServer(int port, int buffer_size, std::string root_directory)
		: port(port), buffer_size(buffer_size), server_socket(-1), root_directory(root_directory) {}

	// Helper function to set a socket as non-blocking
	static void set_non_blocking(int socket)
	{
		int flags = fcntl(socket, F_GETFL, 0);
		fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	}

	int HTTPServer::server_init()
	{

		lib_logger::Logger::Instance().Set_log_level(lib_logger::LogLevel::TRACE);
		lib_logger::Logger::Instance().Set_max_file_size(1024 * 1024);
		lib_logger::Logger::Instance().Set_output_file("log-1.txt");

		lib_logger::LOG(lib_logger::LogLevel::TRACE,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::INFO,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::WARNING,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::ERROR,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::CRITICAL,"this is a test");

		// Create a socket
		server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (server_socket == -1)
		{
			std::cerr << "Failed to create socket" << std::endl;
			return -1;
		}

		set_non_blocking(server_socket);

		// Configure server address structure
		server_addr.sin_family = AF_INET;		  // IPv4
		server_addr.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
		server_addr.sin_port = htons(port);		  // Port number

		// Set the SO_REUSEADDR option to reuse the port
		int opt = 1;
		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			perror("setsockopt failed");
			close(server_socket);
			return -1;
		}

		// Bind the socket to the specified port
		if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		{
			std::cerr << "Binding failed: " << strerror(errno) << std::endl;
			close(server_socket);
			return -1;
		}

		// Listen for incoming connections
		if (listen(server_socket, 10) == -1)
		{
			std::cerr << "Listen failed" << std::endl;
			close(server_socket);
			return -1;
		}

		poll_fds.push_back({server_socket, POLLIN, 0}); // Add server_fd to poll

		std::cout << "Server is running on port " << port << "..." << std::endl;
		return 0;
	}

	void HTTPServer::run()
	{
		ResponseCache response_cache;

		while (true)
		{
			int poll_count = poll(poll_fds.data(), poll_fds.size(), POLL_TIMEOUT);

			if (poll_count > 0)
			{
				for (size_t i = 0; i < poll_fds.size(); ++i)
				{
					if (poll_fds[i].revents & POLLIN)
					{
						if (poll_fds[i].fd == server_socket)
						{
							// handle new connection
							handle_new_connection();
						}
						else
						{
							// handle existing one!
							handle_client_request(poll_fds[i].fd, response_cache);
						}
					}
				}
			}

			// Close any idle connections that exceed CLIENT_TIMEOUT
			check_for_timeouts();
		}

		// Cleanup
		// close(server_socket);
		// close(epoll_fd);
	}

	void HTTPServer::handle_new_connection()
	{
		sockaddr_in client_addr{};
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
		if (client_fd >= 0)
		{
			fcntl(client_fd, F_SETFL, O_NONBLOCK); // Non-blocking client socket

			poll_fds.push_back({client_fd, POLLIN, 0});
			clients[client_fd] = {client_fd, std::chrono::steady_clock::now(), true};

			std::cout << "Accepted new connection: " << client_fd << std::endl;
		}
	}

	void HTTPServer::handle_client_request(int client_fd, ResponseCache &response_cache)
	{
		char buffer[1024];
		int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

		if (bytes_read <= 0)
		{
			remove_client(client_fd); // Remove if read error or closed connection
			return;
		}

		buffer[bytes_read] = '\0';
		std::string request(buffer);

		std::cout << "======================================================Received request:======================================================\n"
				<< std::endl;

		std::string http_req(request);
		RequestAnalyzer analyzer(http_req);
		analyzer.parse_request();

		//analyzer.get_headers().print_all_headers();

		std::cout << "HTTP ver: " << analyzer.get_prot() << std::endl;
		std::cout << "HTTP method: " << http_method_to_string(analyzer.get_method()) << std::endl;
		std::cout << "URI: " << analyzer.get_URI() << std::endl;

		ResponseBuilder resp_builder(analyzer.get_info(), root_directory, analyzer.get_headers(), analyzer.get_body());
	
		int ret = 0;
		std::unique_ptr<CacheEntry> cache_entry;
		bool is_served_from_cache = false;
		if ((ret = resp_builder.handle_HTTP_request(response_cache, cache_entry, is_served_from_cache, analyzer.get_body())) != 0)
		{
			std::cout << "aborting handle_HTTP_request error: " << ret << std::endl;
			return;
		}

		if ((ret = resp_builder.prepare_headers(response_cache, cache_entry, is_served_from_cache)) != 0)
		{
			std::cout << "aborting prepare_headers error: " << ret << std::endl;
			return;
		}

		if ((ret = resp_builder.prepare_full_message()) != 0)
		{
			std::cout << "aborting prepare_full_message error: " << ret << std::endl;
			return;
		}

		if (send(client_fd, resp_builder.get_headers().c_str(), resp_builder.get_headers().size(), MSG_NOSIGNAL) == -1)
		{
			if (errno == EPIPE || errno == ECONNRESET)
			{
				std::cerr << "Error sending headers: Broken pipe or connection reset" << std::endl;
				remove_client(client_fd);
				return;
			}
			perror("Error sending headers");
		}

		if (resp_builder.get_body().size() > 0)
		{
			ssize_t totalSent = 0;
			while (totalSent < resp_builder.get_body().size())
			{
				ssize_t bytesSent = send(client_fd, resp_builder.get_body().c_str() + totalSent, resp_builder.get_body().size() - totalSent, MSG_NOSIGNAL);
				if (bytesSent == -1)
				{
					if (errno == EPIPE || errno == ECONNRESET)
					{
						std::cerr << "Error sending body: Broken pipe or connection reset" << std::endl;
						remove_client(client_fd);
						break;
					}
					// Handle error
					std::cout << "lol, sending error" << std::endl;
					break; // Exit the loop on error
				}
				totalSent += bytesSent; // Increment total sent by the number of bytes sent
				//std::cout << "lol, sent: " << totalSent << std::endl;
				//std::cout << "lol, left: " << resp_builder.get_body().size() - totalSent << std::endl;
			}
		}

		std::cout << "=========================Request served:===========================\n" << std::endl;
		std::cout << "Status: " << resp_builder.get_resp_status() << " Code: "<< resp_builder.get_resp_code() << std::endl;

		clients[client_fd].last_active = std::chrono::steady_clock::now(); // Update last activity

		// Close non-keep-alive connection after response
		clients[client_fd].keep_alive = analyzer.get_headers().is_connection_closed();
		if (clients[client_fd].keep_alive)
		{
			remove_client(client_fd);
		}
	}

	void HTTPServer::check_for_timeouts()
	{
		auto now = std::chrono::steady_clock::now();

		// Iterate through clients and close those that exceed CLIENT_TIMEOUT
		for (auto it = clients.begin(); it != clients.end();)
		{
			int client_fd = it->second.fd;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.last_active).count() > CLIENT_TIMEOUT)
			{
				std::cout << "Connection timed out: " << client_fd << std::endl;

				// Remove client resources safely, without modifying `clients` in `remove_client`
				remove_client(client_fd);
				it = clients.erase(it); // Erase from the client map after removal
			}
			else
			{
				++it;
			}
		}
	}

	void HTTPServer::remove_client(int client_fd)
	{
		if (clients.find(client_fd) == clients.end())
			return; // Skip if client already removed

		close(client_fd); // Close the client connection
		std::cout << "Closed connection: " << client_fd << std::endl;

		// Remove from poll_fds only, leave the map management to `check_for_timeouts`
		auto poll_it = std::find_if(poll_fds.begin(), poll_fds.end(), [client_fd](const pollfd &pfd)
									{ return pfd.fd == client_fd; });
		if (poll_it != poll_fds.end())
		{
			poll_fds.erase(poll_it);
		}
	}

	HTTPServer::~HTTPServer()
	{
		close(server_socket);
		for (const auto &client : clients)
		{
			close(client.second.fd);
		}
	}
}