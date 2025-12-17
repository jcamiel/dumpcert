#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
        return 1;
    }

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "curl_easy_init() failed\n");
        return 1;
    }

    /* Use URL from command-line argument */
    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    struct curl_certinfo *certinfo = NULL;
    res = curl_easy_getinfo(curl, CURLINFO_CERTINFO, &certinfo);

    if ((res == CURLE_OK) && certinfo) {
        for (int i = 0; i < certinfo->num_of_certs; i++) {
            printf("=== Certificate %d ===\n", i);

            struct curl_slist *slist;
            for (slist = certinfo->certinfo[i]; slist; slist = slist->next) {
                printf("%s\n", slist->data);
            }
            printf("\n");
        }
    } else {
        fprintf(stderr, "No certificate info available\n");
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}