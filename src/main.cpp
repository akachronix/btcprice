#include <iostream>
#include <fstream>
#include <memory>
#include <curl/curl.h>
#include "json/json.h"

static size_t writeToString(void* ptr, size_t size, size_t count, void* stream) {
    ((std::string*)stream)->append((char*)ptr, 0, size * count);
    return size * count;
}

static int curlGetData(const char* url, std::string& response)
{
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cout << "Error initializing CURL\n";
        return EXIT_FAILURE;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    if (curl_easy_perform(curl) != CURLE_OK) {
        std::cout << "Error getting data from Coindesk\n";
        return EXIT_FAILURE;
    }

    curl_easy_cleanup(curl);
    return EXIT_SUCCESS;
}

int main(int argc, const char* argv[])
{
    std::string coindesk;
    curlGetData("https://api.coindesk.com/v1/bpi/currentprice.json", coindesk);

    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    if (!reader->parse(coindesk.c_str(), coindesk.c_str() + static_cast<int>(coindesk.length()), &root, &err)) {
        std::cout << "Error parsing JSON\n";
        return EXIT_FAILURE;
    }

    std::string lastUpdated = root.get("time", 0).get("updated", 0).asString();
    std::cout << "Data last updated: " << lastUpdated << '\n';
    
    float btcPrice = root.get("bpi", 0).get("USD", 0).get("rate_float", 0).asFloat();
    std::cout << "1 BTC = $" << btcPrice << '\n';

    if (argc > 1) {
        float btcAmount = std::stof(argv[1]);
        std::cout << "The value of " << btcAmount << " BTC is $" << btcAmount * btcPrice << "\n";
    }

    return EXIT_SUCCESS;
}