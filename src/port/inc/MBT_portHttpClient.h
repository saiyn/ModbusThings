#ifndef _MBT_PORT_HTTP_CLIENT_H_
#define _MBT_PORT_HTTP_CLIENT_H_


int httpclient_request(const char *URI, const void *header, const char *post_data, unsigned char **response);


int httpclient_request_header_add(void **request_header, const char *fmt, ...);


void httpclient_free(char *);


int httpclient_get_file(const char* URI, const char* filename);


int httpclient_post_file(const char* URI, const char* filename,
        const char* form_data);

#endif

