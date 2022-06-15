
#include "curl/curl.h"
#include "MBT_portHttpClient.h"


int httpclient_request_header_add(void **request_header, const char *fmt, ...)
{
    char tmp[256] = {0};

    struct curl_slist *cs = *(struct curl_slist **)request_header;

    va_list args;

    va_start(args, fmt);

    vnsprintf(tmp, sizeof(tmp), fmt, args);

    va_end(args);

    *cs = curl_slist_append(*cs, tmp);


    return 0;

}

static size_t curl_stringBodyWrite(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    char *recv = (char *)userdata;

    strncat(recv, ptr, size*nmemb);

    return size*nmemb;
}

int httpclient_request(const char *URI, const void *header, const char *post_data, unsigned char **response)
{
    long code = 0;
    struct curl_slist *shead = (struct curl_slist *)header;
    CURL* curl = curl_easy_init();
    if(!curl){
        return CURLE_FAILED_INIT;
    }

    char *recv_str = m_malloc(CURL_MAX_WRITE_SIZE);
    if(!recv_str){
        return -2;
    }

    memset(recv_str, 0, CURL_MAX_WRITE_SIZE);

    curl_easy_setopt(curl, CURLOPT_URL, URI);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, shead);


    if(post_data){

        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_stringBodyWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_str);

    int rc = curl_easy_perform(curl);

    curl_esay_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    if(shead){
        curl_slist_free_all(shead);
    }


    *response = recv_str;

    curl_easy_cleanup(curl);

    return rc;
}


void httpclient_free(char *c)
{
    (void)c;
}