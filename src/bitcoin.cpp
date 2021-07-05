#include "bitcoin.hpp"

#include <string>
#include <memory>
#include <exception>

#include <curl/curl.h>
#include "json/json.h"

static size_t writeToString(void* ptr, size_t size, size_t count, void* stream) {
    ((std::string*)stream)->append((char*)ptr, 0, size * count);
    return size * count;
}

Bitcoin::Bitcoin() {
    refresh();
}

int Bitcoin::refresh() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return 1;   // Error initializing CURL
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.coinstats.app/public/v1/coins?currency=USD");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode e = curl_easy_perform(curl);

    if (e != CURLE_OK) {
        return 2;   // Error getting data from Coinstats
    }

    curl_easy_cleanup(curl);

    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    reader->parse(response.c_str(), response.c_str() + static_cast<int>(response.length()), &root, &err);
    for (const Json::Value& coin : root["coins"]) {
        if (coin["id"].asString() == "bitcoin") {
            priceUSD = coin["price"].asDouble();
        }
    }

    return 0;
}

double Bitcoin::getCurrentValueOfBTC() {
    return btcAmount * priceUSD;
}

void Bitcoin::addBTCAmount(double amount) {
    btcAmount += amount;
}

void Bitcoin::setBTCAmount(double amount) {
    btcAmount = amount;
}

void Bitcoin::setPriceInUSD(double price) {
    priceUSD = price;
}

double Bitcoin::getPriceInUSD() {
    return priceUSD;
}

double Bitcoin::getBTCAmount() {
    return btcAmount;
}