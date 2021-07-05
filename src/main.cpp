#include <iostream>
#include <cmath>
#include <curses.h>

#ifdef WIN32
    #include <synchapi.h>
#else
    #include <unistd.h>
#endif

#include "bitcoin.hpp"

static void mssleep(unsigned int ms) {
#ifdef WIN32
    Sleep(ms);
#else
    usleep(1000 * ms);
#endif
}

static void clearLineYX(int y, int x) {
    int oldy, oldx;             // to store where you are
    getyx(stdscr, oldy, oldx);  // save current pos
    move(y, x);                 // move to begining of line
    clrtoeol();                 // clear line
    move(oldy, oldx);           // move back to where you were
}

static double roundN(double value, unsigned int decimalPlaces) {
    const double multiplier = std::pow(10.0, decimalPlaces);
    return std::round(value * multiplier) / multiplier;
}

int main(int argc, const char* argv[])
{
    Bitcoin btc;

    if (argc > 1) // terminal UI
    {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-btcsetprice") == 0) {
                btc.setPriceInUSD(atof(argv[i + 1]));
                std::cout << "BTC Price has been set to: $" << btc.getPriceInUSD() << '\n';
            }

            if (strcmp(argv[i], "-btcprice") == 0) {
                std::cout << "1 BTC = $" << btc.getPriceInUSD() << '\n';
            }

            if (strcmp(argv[i], "-usdtobtc") == 0) {
                const double dollars = atof(argv[i + 1]);
                std::cout << "$" << dollars << " in BTC is: " << dollars / btc.getPriceInUSD() << " BTC\n"; 
            }

            if (strcmp(argv[i], "-btc") == 0) {
                btc.setBTCAmount(atof(argv[i + 1]));
            }
        }

        if (btc.getBTCAmount() != 0) {
            std::cout << "The value of " << btc.getBTCAmount() << " BTC is $" << roundN(btc.getCurrentValueOfBTC(), 2)<< '\n';
        }
    }

    else // ncurses
    {
        initscr();      // initializes screen
        cbreak();       // disables line buffering
        noecho();       // this makes keyboard input not show up in terminal
        curs_set(0);    // makes cursor invisible
        clear();        // clears terminal

        // unsigned int maxlines = LINES - 1;
        // unsigned int maxcols = COLS - 1;

        bool running = true;
        while (running) 
        {
            mvaddstr(0, 0, "btcprice v0.1");
            mvaddstr(1, 0, "by chronix");
            mvaddstr(3, 0, "1) Get Bitcoin Price");
            mvaddstr(4, 0, "2) Set Bitcoin Price");
            mvaddstr(5, 0, "3) USD to BTC");
            mvaddstr(6, 0, "4) BTC to USD");
            mvaddstr(7, 0, "5) Refresh Prices");
            
            refresh();

            char sel = (char)getch();
            clearLineYX(9, 0);

            switch (sel)
            {
            case '1':
            {
                attron(A_BOLD);
                mvprintw(9, 0, "1 BTC = $%f", btc.getPriceInUSD());
                attroff(A_BOLD);
                break;
            }
            
            case '2':
            {
                attron(A_BOLD);
                mvaddstr(9, 0, "Enter a new price for Bitcoin calculations: ");

                echo();
                refresh();
                
                char* input = new char[128];
                getstr(input);
                
                noecho();
                refresh();

                btc.setPriceInUSD(atof(input));

                clearLineYX(9, 0);
                mvprintw(9, 0, "Bitcoin price has been set to $%f", btc.getPriceInUSD());

                attroff(A_BOLD);
                delete[] input;
                break;
            }
            
            case '3':
            {
                attron(A_BOLD);
                mvaddstr(9, 0, "Enter the amount of USD: ");
                
                echo();
                refresh();
                
                char* input = new char[128];
                getstr(input);
                const double usdAmount = atof(input);

                noecho();
                refresh();

                clearLineYX(9, 0);
                mvprintw(9, 0, "The value of $%f USD is %f BTC", usdAmount, roundN(usdAmount / btc.getPriceInUSD(), 2));

                attroff(A_BOLD);
                delete[] input;
                break;
            }
            
            case '4':
            {
                attron(A_BOLD);
                mvaddstr(9, 0, "Enter the amount of Bitcoin: ");

                echo();
                refresh();

                char* input = new char[128];
                getstr(input);
                btc.setBTCAmount(atof(input));

                noecho();
                refresh();

                clearLineYX(9, 0);
                mvprintw(9, 0, "The value of %f BTC is $%f", btc.getBTCAmount(), roundN(btc.getCurrentValueOfBTC(), 2));

                attroff(A_BOLD);
                delete[] input;
                break;
            }
            
            case '5':
            {
                btc.refresh();
                attron(A_BOLD);
                mvprintw(9, 0, "Bitcoin price has been reset to $%f", btc.getPriceInUSD());
                attroff(A_BOLD);
                break;
            }

            case 'e':
                running = false;
                break;
            }

            refresh();
            mssleep(100);
        }

        endwin();
    }
}