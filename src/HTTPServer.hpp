#ifndef HTTP_MANAGER
#define HTTP_MANAGER

#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <poll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ResponseCache.hpp"
#include "../liblogger/liblogger.hpp"

constexpr int POLL_TIMEOUT = 5000;     // Poll wait timeout (milliseconds)
constexpr int CLIENT_TIMEOUT = 30000;  // Client connection idle timeout (milliseconds)

namespace HTTP_Server
{

	struct ClientConnection
	{
		int fd;											   // Client socket file descriptor
		std::chrono::steady_clock::time_point last_active; // Last active time
		bool keep_alive;								   // Track if connection is persistent
		SSL* ssl;
	};

	class HTTPServer
	{
	public:
		// Constructor to initialize port and buffer_size with default values
		HTTPServer(int port = 8080, int buffer_size = 1024, std::string root_directory = "../www", SSL_CTX* ctx = nullptr);
		~HTTPServer();              // Destructor (add this line)

		void run();
		int server_init();

		// Accessor and mutator methods for port and buffer_size (optional, but useful)
		int get_port() const { return port; }
		void set_port(int new_port) { port = new_port; }

		int get_buffer_size() const { return buffer_size; }
		void set_buffer_size(int new_buffer_size) { buffer_size = new_buffer_size; }

		std::string get_root_directory() const { return root_directory; }

	private:
		int port;		   // Port number, default 8080
		int buffer_size;   // Buffer size, default 1024
		int server_socket; // Server socket descriptor
		// int client_socket; // Client socket descriptor
		std::string root_directory;
		SSL_CTX* ctx;
		struct sockaddr_in server_addr;
		struct sockaddr_in client_addr;
		socklen_t addr_len = sizeof(client_addr);
		std::map<int, ClientConnection> clients; // Track active clients and their last activity
		std::vector<struct pollfd> poll_fds;	 // For handling poll-based connections
		//lib_logger::Logger Log_object;

		void handle_new_connection();			   // Accept new clients
		void handle_client_request(int client_fd, ResponseCache &response_cache); // Process client requests
		void check_for_timeouts();				   // Close inactive connections
		void remove_client(int client_fd);		   // Helper to clean up closed connections
		void configure_context();
	};

}

#endif // HTTP_MANAGER
