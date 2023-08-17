#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <unistd.h>
#include "graphics.h"
#include "network.h"
#include "board.h"

#define DELAY 1000000

using std::unordered_map;
using std::vector;
using std::string;
using std::thread;
using std::cout;
using std::cin;

assetGroup assets {loadAssets("assets.txt")};

string input(int n) {
    COOKED();
    cout << DECTCEM_H;
    string s;
    while (s.empty()) {
        getline(cin, s);
        for (size_t i {}; i < s.size(); ++i) {
            if (i < s.size() - 2 && (s.substr(i, 3) == "\e[A" || s.substr(i, 3) == "\e[B")) {
                s.erase(i, 3);
                --i;
            }
            else if (!isalnum(s.at(i))) s.at(i) = '+';
        }
    }
    cout << DECTCEM_L;
    RAW();
    return s.substr(0, n);
}

string reparse(const string &s) {
    string sc {s};
    for (char &c : sc) if (c == '+') c = ' ';
    return sc;
}

void blank() {
    char t;
    while ((cin >> t) && t != 'c');
}

void awaitCancel(bool *b) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    while (!*b) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        if (select(1, &fds, nullptr, nullptr, &tv) < 0) break;
        else if (FD_ISSET(STDIN_FILENO, &fds)) {
            char t;
            cin >> t;
            if (t == 'c') break;
        }
        usleep(100000);
    }
    *b = true;
}

string startMenu();

enum {LIKE = '0', JOIN = '1', HOST = '2', RULES = '3', EXIT = '4'};

char mainMenu(const string &username);

void likeMenu(string &likeResponse);

void joinMenu(const string &username);

void hostMenu(const string &username);

void rulesMenu();

void startGame(const string &code, const string &username, const string &opponent, bool host);

int main() {
    cout << CSI "?1049h";
    string username {startMenu()};
    string likeResponse;
    char choice;
    do {
        choice = mainMenu(username);
        switch (choice) {
            case LIKE: {likeMenu(likeResponse); break;}
            case JOIN: {joinMenu(username); break;}
            case HOST: {hostMenu(username); break;}
            case RULES: {rulesMenu(); break;}
        }
    }while (choice != EXIT);
    COOKED();
    cout << CSI "?1049l";
    cout << CLR BOLD "Thanks for playing!\n" RESET DECTCEM_H;
    return 0;
}

string startMenu() {
    RAW();
    cout << CLR DECTCEM_L;
    fadeIn(GETASSET(assets, "$BATTLESHIP"), 5, 5, SLOW);
    fadeIn("Connecting to Server...", 16, 19, SLOW);
    if (!testConnection()) {
        COOKED();
        cout << CSI "?1049l" << CLR BOLD "Failed to connect to server. If this issue persists after a few attempts, please try again later.\n";
        cout << RESET DECTCEM_H;
        exit(0);
    }
    fadeOut("Connecting to Server...", 16, 19, SLOW);
    linearIn("Username: ", 9, 19, MEDIUM);
    string username {input(16)};
    fadeOut(GETASSET(assets, "$BATTLESHIP"), 5, 5, SLOW);
    linearOut("Username: ", 9, 19, MEDIUM);
    logConnection(getenv("REPL_OWNER"), username);
    return username;
}

char mainMenu(const string &username) {
    cout << CLR;
    fadeIn(GETASSET(assets, "$HOMESCREEN"), 5, 5, MEDIUM);
    linearIn("Username: " + reparse(username) + R"(

0 - Like this Game
1 - Join a Game
2 - Host a Game
3 - Rules
4 - Exit)", 5, 14, FAST);
    cout << "\r\n\n\n\n\n\n" DIM ITALIC "     Your username will be logged." RESET;
    char choice;
    while ((cin >> choice) && choice != LIKE && choice != JOIN && choice != HOST && choice != RULES && choice != EXIT);
    cout << CLR;
    return choice;
}

void likeMenu(string &likeResponse) {
    if (likeResponse.empty()) cout << BOLD "✨ " << (likeResponse = like(getenv("REPL_OWNER"))) << " ✨" RESET << "\r\n\nPress C to continue.";
    else cout << BOLD "✨ " << likeResponse << " (like counter not incremented) ✨" RESET << "\r\n\nPress C to continue.";
    blank();
}

void joinMenu(const string &username) {
    fadeIn(GETASSET(assets, "$JOIN"), 5, 5, MEDIUM);
    linearIn("Enter a Join Code: ", 5, 18, MEDIUM);
    string joinCode {input(32)};
    string joinInfo {sendJoinRequest(joinCode, username)};
    if (joinInfo.at(0) == '1') {
        string opponent {joinInfo.substr(1)};
        cout << CLR;
        startGame(joinCode, username, opponent, false);
    }else {
        linearIn("The join code provided is invalid. Press C to continue.", 5, 20, FAST);
        blank();
    }
}

void hostMenu(const string &username) {
    fadeIn(GETASSET(assets, "$HOST"), 5, 5, MEDIUM);
    linearIn("Enter a Personal Join Code (16 characters max): ", 5, 18, MEDIUM);
    string joinCode {input(16)};
    cout << "     Join Code: " << joinCode << "\n\n";
    if (createGame(joinCode, username)) {
        linearIn("Waiting for someone to join. Press C to cancel.", 5, 22, FAST);
        bool cancel {false};
        thread ac_thread {awaitCancel, &cancel};
        while (!cancel) {
            string joinInfo {pingGame(joinCode)};
            if (joinInfo.at(0) == '1') {
                cancel = true;
                ac_thread.join();
                string opponent {joinInfo.substr(1)};
                cout << CLR;
                startGame(joinCode, username, opponent, true);
                return;
            }
            usleep(DELAY);
        }
        if (cancel) {
            cancelGame(joinCode);
            ac_thread.join();
        }
    }else {
        linearIn("Your join code was already taken. Press C to continue.", 5, 20, FAST);
        blank();
    }
}

void rulesMenu() {
    const string &rules {GETASSET(assets, "$RULES")};
    size_t rules_len {rules.length()};
    linearIn(rules.substr(0, rules_len / 3 + 117), 1, 1, 1);
    cout << "\r\n\n Press C to Scroll Down";
    blank();
    linearIn(CLR + rules.substr(rules_len / 3 + 118, rules_len / 3 * 2 - rules_len / 3 + 17), 1, 1, 1);
    cout << "\r\n\n Press C to Scroll Down";
    blank();
    linearIn(CLR + rules.substr(rules_len / 3 + 117 + rules_len / 3 * 2 - rules_len / 3 + 19), 1, 1, 1);
    cout << "\r\n\n Press C to continue.";
    blank();
}

void startGame(const string &code, const string &username, const string &opponentraw, bool host) {
    fadeIn(GETASSET(assets, "$CROSSHAIR"), 25, 1, MEDIUM);
    linearIn(RED + reparse(username), static_cast<int>(25 - username.length()), 5, MEDIUM);
    string opponent {reparse(opponentraw)};
    linearIn(BLUE + opponent + RESET, static_cast<int>(32), 5, MEDIUM);
    vector <ship> available {
        {{3, 5}, "Carrier", GETASSET(assets, "$CARRIER_H"), GETASSET(assets, "$CARRIER_V"), 5, 5, false}, 
        {{4, 5}, "Battleship", GETASSET(assets, "$BATTLE_H"), GETASSET(assets, "$BATTLE_V"), 4, 4, false}, 
        {{5, 5}, "Cruiser", GETASSET(assets, "$CRUISER_H"), GETASSET(assets, "$CRUISER_V"), 3, 3, false}, 
        {{5, 5}, "Submarine", GETASSET(assets, "$SUBMARINE_H"), GETASSET(assets, "$SUBMARINE_V"), 3, 3, false}, 
        {{6, 5}, "Destroyer", GETASSET(assets, "$DESTROYER_H"), GETASSET(assets, "$DESTROYER_V"), 2, 2, false}
    };
    board::BOARD = GETASSET(assets, "$BOARD");
    board home {14, 7};
    linearIn("Use WASD to move the ship around, E to place, and R to rotate.", 14, 21, FAST);
    for (ship &s : available) home.placeShip(s);
    linearIn("Press a key to continue.", 14, 23, FAST);
    char c;
    cin >> c;
    if (c == '/') {
        ship fish {{1, 1}, "Fishing_Boat", GETASSET(assets, "$FISHING_BOAT"), GETASSET(assets, "$FISHING_BOAT"), 1, 1, false};
        home.placeShip(fish);
    }
    linearIn("Readying...", 14, 25, FAST);
    while (!ready(code, host)) usleep(DELAY);
    linearIn("Readied! Waiting for " BLUE + opponent + RESET " to ready.", 14, 26, FAST);
    while (!pingStart(code)) usleep(DELAY);
    linearIn(GETASSET(assets, "$VOID"), 14, 7, 1);
    home.loc = {0, 7};
    board abroad {32, 7};
    linearIn(GETASSET(assets, "$VERTICAL_LINE"), 28, 7, SLOW);
    bool turn {host};
    while (true) {
        abroad.draw();
        home.draw();
        linearIn(CSI "J\n" + (turn ? "It's " RED "your" RESET " turn! Use WASD to aim and F to fire!" : "Waiting for " BLUE + opponent + RESET " to make a move..."), 0, 20, FAST);
        if (turn) {
            turn = false;
            coord x {abroad.fire()};
            linearIn("Message to Send to " BLUE + opponent + RESET ": ", 0, 23, FAST);
            string m {input(256)};
            linearIn(string {"Fired at "} + static_cast<char>(x.x + 'A' - 1) + (x.y == 10 ? string {"10"} : string {static_cast<char>(x.y + '0')}) + ". Awaiting response...", 0, 25, FAST);
            while (!fire(code, host, x.x - 1, x.y - 1, m)) usleep(DELAY);
            string outcome;
            while ((outcome = pingFire(code, host)).at(0) != '1') usleep(DELAY);
            string return_m {outcome.substr(2)};
            char return_o {outcome.at(1)};
            // 0 - miss, 1 - hit, 2 - sunk, 3 - win
            if (return_o == '0') {
                linearIn("Miss! Guess better?", 0, 28, FAST);
                usleep(4 * DELAY);
                abroad.addMiss(x);
            }else if (return_o == '1') {
                linearIn("Hit! " BLUE + opponent + RESET " was struck!", 0, 28, FAST);
                abroad.addHit(x);
            }else if (return_o == '2') {
                linearIn(BLUE + opponent + RESET "\'s " + return_m + " was sunk!", 0, 28, FAST);
                abroad.addHit(x);
            }else if (return_o == '3') {
                linearIn(BLUE + opponent + RESET "\'s " + return_m + " was sunk!", 0, 28, FAST);
                linearIn(RED "YOU WIN" RESET "\n\nPress C to continue.", 0, 30, SLOW);
                leave(code);
                blank();
                break;
            }
            linearIn("Press C to continue.", 0, 30, FAST);
            blank();
        }else {
            turn = true;
            string incoming;
            while ((incoming = pingIncoming(code, host)).at(0) != '1') usleep(DELAY);
            coord impact {static_cast<short>(incoming.at(1) - '0' + 1), static_cast<short>(incoming.at(2) - '0' + 1)};
            ship *hom {home.hitOrMiss(impact)};
            linearIn(string {"Impact at "} + static_cast<char>(impact.x + 'A' - 1) + (impact.y == 10 ? string {"10"} : string {static_cast<char>(impact.y + '1' - 1)}) + '!', 0, 23, FAST);
            if (hom == nullptr) {
                while (!acknowledge(code, "0")) usleep(DELAY);
                linearIn("MISS! We're safe for now...", 0, 25, FAST);
                home.addMiss(impact);
            }else {
                linearIn(RED "Your " RESET + hom->type + " was hit!", 0, 25, FAST);
                home.addHit(impact);
                if (hom->hp == 0) linearIn("Sunk!", 0, 26, FAST);
                if (home.defeated()) {
                    while (!acknowledge(code, '3' + hom->type)) usleep(DELAY);
                    linearIn(BLUE "DEFEAT" RESET "\nBetter luck next time...\n\nPress C to continue.", 0, 28, SLOW);
                    leave(code);
                    blank();
                    break;
                }else if (hom->hp == 0) {
                    while (!acknowledge(code, '2' + hom->type)) usleep(DELAY);
                }
                else {
                    while (!acknowledge(code, string {'1'})) usleep(DELAY);
                }
            }
            linearIn("Message from " BLUE + opponent + RESET ": " + reparse(incoming.substr(3)) + "\n\nPress C to continue.", 0, 28, FAST);
            blank();
        }
    }
}