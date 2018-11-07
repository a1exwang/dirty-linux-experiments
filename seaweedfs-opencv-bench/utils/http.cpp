#include <iostream>

#include "http.h"
#include "curl/curl.h"


using namespace std;


size_t write_body(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    auto builder = (vector<char>*)userp;

    char *p = (char*)contents;
    for (int i = 0; i < realsize; ++i) {
        builder->push_back(p[i]);
    }

    return realsize;
}


long download(const std::string &url, vector<char> &buf) {
    auto curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        //curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        /* enable TCP keep-alive for this transfer */
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);

        auto res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
        } else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
        curl = NULL;
        return response_code;
    } else {
        cerr << "curl failed" << endl;
    }
    return -1;
}
