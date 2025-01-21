#ifndef RESPONSE_CACHE_HPP
#define RESPONSE_CACHE_HPP

#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <optional>
#include "HttpHeaders.hpp"

namespace HTTP_Server
{
	// Cache entry structure
	struct CacheEntry
	{
		std::string body;
		HTTPHeaders selected_headers;
		std::chrono::time_point<std::chrono::steady_clock> timestamp;
		std::string cached_Etag;
		std::chrono::seconds max_age; // Duration until the cache entry is considered stale
	};

	class ResponseCache
	{
	private:
		std::unordered_map<std::string, CacheEntry> cache;
		std::mutex mutex;

		// Check if a header should be stored in the cache according to RFC 9111
		bool shouldStoreHeader(const std::string &headerName);

	public:
		// Retrieve a cache entry based on key
		std::optional<CacheEntry> get(const std::string &key);

		// Store a new entry in the cache
		void put(const std::string &key, const std::string &body, const HTTPHeaders &headers, const std::string &Etag);

		// Update an existing cache entry
		void update(const std::string &key, const std::string &body, const HTTPHeaders &headers, const std::string &Etag);

		// Update an existing cache entry by appending to the body and headers
		void updateAndAppend(const std::string &key, const std::string &additional_body, const HTTPHeaders &additional_headers, const std::string &Etag);

		// Generate appropriate cache-related response headers
		void generateCacheHeaders(HTTPHeaders &resp_headers, const CacheEntry &entry);

		// Handle cache-related request headers (e.g., If-None-Match, If-Modified-Since)
		bool validateCacheEntry(const std::optional<CacheEntry>& entry, const HTTPHeaders &request_headers);
	};
}

#endif // RESPONSE_CACHE_HP