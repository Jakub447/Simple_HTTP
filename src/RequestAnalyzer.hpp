#ifndef REQUEST_ANALYZER_H
#define REQUEST_ANALYZER_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "HTTP_connection_info.hpp"
#include "HttpHeaders.hpp"

namespace HTTP_Server
{
	class RequestAnalyzer
	{
		public:
		RequestAnalyzer(const std::string& raw_request) : raw_request(raw_request) {}

		bool parse_request();
		std::vector<std::string> get_header(const std::string& key) const;

		std::string get_body() const;

		bool has_header(const std::string &key) const;
		bool validate_cache(const std::string& clientETag, const std::string& currentETag) const;

		std::string get_URI(){return req_info.URI;}
		int get_prot(){return req_info.prot_ver;}
		HTTPMethod get_method(){return req_info.method;}
		HTTP_request_info get_info(){return req_info;}
		HTTPHeaders get_headers(){return request_headers;}
		


		private:
		std::string raw_request;
		HTTPHeaders request_headers;
		std::string body;
		HTTP_request_info req_info;

		void parse_request_line(const std::string &line);

	};
}

#endif //REQUEST_ANALYZER_H