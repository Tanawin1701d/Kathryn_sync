#include <iostream>
#include <string>
#include <curl/curl.h>
#include <chrono>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    size_t realsize = size * nmemb;
    data->append(ptr, realsize);
    return realsize;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();


    CURL* curl;
    CURLcode res;
    std::string body;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/api/messages/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        else
            std::cout << body.size() << std::endl;

        curl_easy_cleanup(curl);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Print the elapsed time
    std::cout << "Elapsed time: " << elapsed.count() << " sec" << std::endl;

    return 0;
}