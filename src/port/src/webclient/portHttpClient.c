#include "portHttpClient.h"

#include "webclient.h"


int httpclient_get_file(const char* URI, const char* filename)
{
	return webclient_get_file(URI, filename);
}

int httpclient_post_file(const char* URI, const char* filename, const char* form_data)
{
	
	return webclient_post_file(URI, filename, form_data);
}


int httpclient_request(const char *URI, const char *header, const char *post_data, unsigned char **response)
{
	return webclient_request(URI, header, post_data, response);
}


int httpclient_request_header_add(char **request_header, const char *fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	
	webclient_request_header_add(request_header, fmt, args);
	
	va_end(args);
	
	
	return 0;
}



void httpclient_free(char *c)
{
	web_free(c);
}
