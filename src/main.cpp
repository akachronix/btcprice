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

    CURLcode e = curl_easy_perform(curl);

    if (e != CURLE_OK) {
        std::cout << "Error getting data from Coindesk: " << curl_easy_strerror(e) << '\n';
        return EXIT_FAILURE;
    }

    curl_easy_cleanup(curl);
    return EXIT_SUCCESS;
}

static Json::Value parseJson(std::string& rawJsonData)
{
    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    if (!reader->parse(rawJsonData.c_str(), rawJsonData.c_str() + static_cast<int>(rawJsonData.length()), &root, &err)) {
        std::cout << "Error parsing JSON\n";
        return EXIT_FAILURE;
    }

    return root;
}

int main(int argc, const char* argv[])
{
    std::string coindesk;
    curlGetData("https://api.coindesk.com/v1/bpi/currentprice.json", coindesk);

    Json::Value parsedJson = parseJson(coindesk);
    
    std::string lastUpdated = parsedJson.get("time", 0).get("updated", 0).asString();
    std::cout << "Data last updated: " << lastUpdated << '\n';
    
    float btcPriceUSD = parsedJson.get("bpi", 0).get("USD", 0).get("rate_float", 0).asFloat();
    std::cout << "1 BTC = $" << btcPriceUSD << '\n';

    if (argc > 1) 
    {
        int offset = 0;

        bool setPrice = strcmp(argv[1], "-price") ? false : true;
        if (setPrice) {
            btcPriceUSD = atof(argv[2]);
            std::cout << "Price has been set to: $" << btcPriceUSD << '\n';
            offset += 2;
        }

        float btcAmount = 0.0;
        for (int i = 1 + offset; i < argc; i++) {
            btcAmount += atof(argv[i]);
        }

        std::cout << "The value of " << btcAmount << " BTC is $" << btcAmount * btcPriceUSD << "\n";
    }

    return EXIT_SUCCESS;
}