#include <iostream>
#include <fstream>
#include <memory>
#include <curl/curl.h>
#include "json/json.h"

static size_t writeToString(void* ptr, size_t size, size_t count, void* stream) {
    ((std::string*)stream)->append((char*)ptr, 0, size * count);
    return size * count;
}

int main(int argc, const char* argv[])
{
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cout << "Error initializing CURL\n";
        return EXIT_FAILURE;
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.coindesk.com/v1/bpi/currentprice.json");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    if (curl_easy_perform(curl) != CURLE_OK) {
        std::cout << "Error getting data from Coindesk\n";
        return EXIT_FAILURE;
    }

    curl_easy_cleanup(curl);

    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    if (!reader->parse(response.c_str(), response.c_str() + static_cast<int>(response.length()), &root, &err)) {
        std::cout << "Error parsing JSON\n";
        return EXIT_FAILURE;
    }
    
    float btcPrice = root.get("bpi", 0).get("USD", 0).get("rate_float", 0).asFloat();
    std::cout << "1 BTC = $" << btcPrice << '\n';

    if (argc > 1) {
        float btcAmount = std::stof(argv[1]);
        std::cout << "The value of " << btcAmount << " BTC is $" << btcAmount * btcPrice << "\n";
    }

    return EXIT_SUCCESS;
}