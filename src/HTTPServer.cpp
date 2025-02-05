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

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "HTTPServer.hpp"
#include "RequestAnalyzer.hpp"
#include "ResponseBuilder.hpp"
#include "../liblogger/liblogger.hpp"
#include "error_codes.hpp"

namespace HTTP_Server
{

	// Constructor to initialize port and buffer_size with default values
	HTTPServer::HTTPServer(int port, int buffer_size, std::string root_directory,  SSL_CTX* ctx)
		: port(port), buffer_size(buffer_size), server_socket(-1), root_directory(root_directory), ctx(nullptr) {}

	

	// Helper function to set a socket as non-blocking
	static void set_non_blocking(int socket)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		int flags = fcntl(socket, F_GETFL, 0);
		fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	}

	static void init_openssl()
	{
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
	}

	static void cleanup_openssl()
	{
		EVP_cleanup();
	}

	static SSL_CTX *create_context()
	{
		const SSL_METHOD *method;
		SSL_CTX *ctx;

		method = TLS_server_method();
		ctx = SSL_CTX_new(method);
		if (!ctx)
		{
			perror("Unable to create SSL context");
			ERR_print_errors_fp(stderr);
			exit(EXIT_FAILURE);
		}
		return ctx;
	}

	void HTTPServer::configure_context()
	{
		if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
			SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
		{
			ERR_print_errors_fp(stderr);
			exit(EXIT_FAILURE);
		}
	}

	int HTTPServer::server_init()
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");

		lib_logger::Logger::Instance().Set_log_level(lib_logger::LogLevel::DEBUG);
		//lib_logger::Logger::Instance().Set_max_file_size(1024 * 1024);
		//lib_logger::Logger::Instance().Set_output_file("log-1.txt");

		std::string test_string = "world";
		lib_logger::LOG(lib_logger::LogLevel::TRACE, "Hello, %s! This is a test.", test_string.c_str());

		lib_logger::LOG(lib_logger::LogLevel::TRACE,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::INFO,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::WARNING,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::ERROR,"this is a test");
		lib_logger::LOG(lib_logger::LogLevel::CRITICAL,"this is a test");

		init_openssl();

		ctx = create_context();
		configure_context();

		// Create a socket
		server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (server_socket == -1)
		{

			lib_logger::LOG(lib_logger::LogLevel::ERROR,"Failed to create socket");
			return APP_ERR_SOCK_CREAT;

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
			return APP_ERR_SOCK_OPT;
		}

		// Bind the socket to the specified port
		if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		{
			lib_logger::LOG(lib_logger::LogLevel::ERROR,"Binding failed: %s", strerror(errno));
			close(server_socket);
			return APP_ERR_BIND;
		}

		// Listen for incoming connections
		if (listen(server_socket, 10) == -1)
		{
			lib_logger::LOG(lib_logger::LogLevel::ERROR,"Listen failed");
			close(server_socket);
			return APP_ERR_LISTEN;
		}

		poll_fds.push_back({server_socket, POLLIN, 0}); // Add server_fd to poll

		lib_logger::LOG(lib_logger::LogLevel::INFO,"Server is running on port: %d...", port);
		return APP_ERR_OK;

	}

	void HTTPServer::run()
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
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
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		sockaddr_in client_addr{};
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
		if (client_fd >= 0)
		{

			SSL *ssl = SSL_new(ctx);
			SSL_set_fd(ssl, client_fd);

			if (SSL_accept(ssl) <= 0)
			{
				ERR_print_errors_fp(stderr);
				return;
			}
			else
			{
				//const char reply[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, HTTPS!";
				//SSL_write(ssl, reply, strlen(reply));
			}

			fcntl(client_fd, F_SETFL, O_NONBLOCK); // Non-blocking client socket

			poll_fds.push_back({client_fd, POLLIN, 0});
			clients[client_fd] = {client_fd, std::chrono::steady_clock::now(), true, ssl};

			lib_logger::LOG(lib_logger::LogLevel::DEBUG,"Accepted new connection: %d", client_fd);
		}
	}

	void HTTPServer::handle_client_request(int client_fd, ResponseCache &response_cache)
	{
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		char buffer[1024];
		SSL* ssl = clients[client_fd].ssl;
		//int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		int bytes_read = SSL_read(ssl, buffer, sizeof(buffer) - 1);

		if (bytes_read <= 0)
		{
			remove_client(client_fd); // Remove if read error or closed connection
			return;
		}

		buffer[bytes_read] = '\0';
		std::string request(buffer);

		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"======================================================Received request:======================================================");

		std::string http_req(request);
		RequestAnalyzer analyzer(http_req);
		analyzer.parse_request();

		//analyzer.get_headers().print_all_headers();
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"HTTP ver: %d", analyzer.get_prot());
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"HTTP method: %s",  http_method_to_string(analyzer.get_method()).c_str());
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"URI: %s", analyzer.get_URI().c_str());

		ResponseBuilder resp_builder(analyzer.get_info(), root_directory, analyzer.get_headers(), analyzer.get_body());
	
		int ret = 0;
		std::unique_ptr<CacheEntry> cache_entry;
		bool is_served_from_cache = false;
		if ((ret = resp_builder.handle_HTTP_request(response_cache, cache_entry, is_served_from_cache, analyzer.get_body())) != 0)
		{
			lib_logger::LOG(lib_logger::LogLevel::ERROR,"aborting handle_HTTP_request error: %d", ret);
			return;
		}

		if ((ret = resp_builder.prepare_headers(response_cache, cache_entry, is_served_from_cache)) != 0)
		{
			lib_logger::LOG(lib_logger::LogLevel::ERROR,"aborting prepare_headers error: %d", ret);
			return;
		}

		if ((ret = resp_builder.prepare_full_message()) != 0)
		{
			lib_logger::LOG(lib_logger::LogLevel::ERROR,"aborting prepare_full_message error: %d", ret);
			return;
		}

		//if (send(client_fd, resp_builder.get_headers().c_str(), resp_builder.get_headers().size(), MSG_NOSIGNAL) == -1)
		if (SSL_write(ssl, resp_builder.get_headers().c_str(), resp_builder.get_headers().size()) == -1)
		{
			if (errno == EPIPE || errno == ECONNRESET)
			{
				lib_logger::LOG(lib_logger::LogLevel::ERROR,"sending headers: Broken pipe or connection reset");
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
				//ssize_t bytesSent = send(client_fd, resp_builder.get_body().c_str() + totalSent, resp_builder.get_body().size() - totalSent, MSG_NOSIGNAL);
				int  bytesSent = SSL_write(ssl, resp_builder.get_body().c_str() + totalSent, resp_builder.get_body().size() - totalSent);
				if (bytesSent == -1)
				{
					if (errno == EPIPE || errno == ECONNRESET)
					{
						lib_logger::LOG(lib_logger::LogLevel::ERROR,"sending body: Broken pipe or connection reset");
						remove_client(client_fd);
						break;
					}
					// Handle error
					lib_logger::LOG(lib_logger::LogLevel::ERROR,"Error state of the connection");
					break; // Exit the loop on error
				}
				totalSent += bytesSent; // Increment total sent by the number of bytes sent
			}
		}

		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"=========================Request served:===========================");
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"Status: %s code: %d", resp_builder.get_resp_status().c_str(), resp_builder.get_resp_code());

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
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		auto now = std::chrono::steady_clock::now();

		// Iterate through clients and close those that exceed CLIENT_TIMEOUT
		for (auto it = clients.begin(); it != clients.end();)
		{
			int client_fd = it->second.fd;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.last_active).count() > CLIENT_TIMEOUT)
			{
				lib_logger::LOG(lib_logger::LogLevel::DEBUG,"Connection timed out: %d", client_fd);

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
		lib_logger::LOG(lib_logger::LogLevel::TRACE,"");
		if (clients.find(client_fd) == clients.end())
			return; // Skip if client already removed

		close(client_fd); // Close the client connection
		lib_logger::LOG(lib_logger::LogLevel::DEBUG,"Closed connection: %d", client_fd);

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
		for (const auto& client : clients) {
			SSL_shutdown(client.second.ssl);
			SSL_free(client.second.ssl);
			close(client.second.fd);
		}
		close(server_socket);
		SSL_CTX_free(ctx);
		cleanup_openssl();
	}
}