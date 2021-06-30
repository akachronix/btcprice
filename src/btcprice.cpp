#include <iostream>
#include <fstream>
#include <memory>
#include <cmath>

#include <curl/curl.h>
#include "json/json.h"

static double roundN(double value, unsigned int decimalPlaces) {
    const double multiplier = std::pow(10.0, decimalPlaces);
    return std::round(value * multiplier) / multiplier;
}

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
    std::string coinstats;
    curlGetData("https://api.coinstats.app/public/v1/coins?currency=USD", coinstats);

    Json::Value parsedJson = parseJson(coinstats);

    double btcPriceUSD = 0.0, ethPriceUSD = 0.0;
    for (const Json::Value& coin : parsedJson["coins"]) {
        if (coin["id"].asString() == "bitcoin") {
            btcPriceUSD = coin["price"].asDouble();
        }

        if (coin["id"].asString() == "ethereum") {
            ethPriceUSD = coin["price"].asDouble();
        }
    }

    std::cout << "1 BTC = $" << btcPriceUSD << '\n';
    std::cout << "1 ETH = $" << ethPriceUSD << '\n';

    if (argc > 1) 
    {
        double btcAmount = 0.0, ethAmount = 0.0;
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-btcprice") == 0) {
                btcPriceUSD = atof(argv[i + 1]);
                std::cout << "BTC Price has been set to: $" << btcPriceUSD << '\n';
            }

            if (strcmp(argv[i], "-ethprice") == 0) {
                ethPriceUSD = atof(argv[i + 1]);
                std::cout << "ETH Price has been set to: $" << ethPriceUSD << '\n';
            }

            if (strcmp(argv[i], "-usdtobtc") == 0) {
                const double dollars = atof(argv[i + 1]);
                std::cout << "$" << dollars << " in BTC is: " << dollars / btcPriceUSD << " BTC\n"; 
            }

            if (strcmp(argv[i], "-usdtoeth") == 0) {
                const double dollars = atof(argv[i + 1]);
                std::cout << "$" << dollars << " in ETH is: " << dollars / ethPriceUSD << " ETH\n";
            }

            if (strcmp(argv[i], "-btc") == 0) {
                btcAmount += atof(argv[i + 1]);
                std::cout << atof(argv[i + 1]) << " BTC added to balance.\n";
            }

            if (strcmp(argv[i], "-eth") == 0) {
                ethAmount += atof(argv[i + 1]);
                std::cout << atof(argv[i + 1]) << " ETH added to balance.\n";
            }
        }

        if (btcAmount != 0) {
            const double btcValue = roundN(btcAmount * btcPriceUSD, 2);
            std::cout << "The value of " << btcAmount << " BTC is $" << btcValue << '\n';
        }

        if (ethAmount != 0) {
            const double ethValue = roundN(ethAmount * ethPriceUSD, 2);
            std::cout << "The value of " << ethAmount << " ETH is $" << ethValue << '\n';
        }
    }

    return EXIT_SUCCESS;
}