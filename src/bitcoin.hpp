#pragma once

class Bitcoin
{
private:
    double priceUSD = 0.0;
    double btcAmount = 0.0;

public:
    Bitcoin();
    int refresh();
    double getCurrentValueOfBTC();
    void addBTCAmount(double amount);
    void setBTCAmount(double amount);
    void setPriceInUSD(double price);
    double getPriceInUSD();
    double getBTCAmount();
};