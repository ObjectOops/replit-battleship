#include "network.h"

std::string query(const std::string &path, const std::string &args) {
    std::string request {"/bin/curl -s \"" + proxy + '/' + path + '?' + args + '\"'};
    // C-Style piping from Jacob Sorber: https://www.youtube.com/watch?v=8AXEHrQTf3I
    FILE *pipe {popen(request.c_str(), "r")};
    char buffer [2048];
    while (fgets(buffer, 2048, pipe));
    pclose(pipe);
    return std::string {buffer};
}

bool testConnection() {
    return query("connect", "") == "1";
}

bool logConnection(const std::string &replUser, const std::string &alias) {
    return query("log", "replUser=" + replUser + "&alias=" + alias) == "1";
}

std::string like(const std::string &replUser) {
    return query("like", "replUser=" + replUser);
}

std::string sendJoinRequest(const std::string &code, const std::string &user) {
    return query("join", "code=" + code + "&user=" + user);
}

bool createGame(const std::string &code, const std::string &user) {
    return query("new", "code=" + code + "&user=" + user) == "1";
}

std::string pingGame(const std::string &code) {
    return query("sping", "code=" + code);
}

bool cancelGame(const std::string &code) {
    return query("scancel", "code=" + code) == "1";
}

bool ready(const std::string &code, bool host) {
    return query("ready", "code=" + code + "&host=" + (host ? '0' : '1')) == "1";
}

bool pingStart(const std::string &code) {
    return query("gping", "code=" + code) == "1";
}

bool leave(const std::string &code) { // When under naval artillery there is no retreat.
    return query("leave", "code=" + code) == "1";
}

bool fire(const std::string &code, bool host, short x, short y, const std::string &message) {
    return query("fire", "code=" + code + "&host=" + (host ? '0' : '1') + "&x=" + static_cast<char>(x + '0') + "&y=" + static_cast<char>(y + '0') + "&message=" + message) == "1";
}

std::string pingFire(const std::string &code, bool host) {
    return query("fping", "code=" + code + "&host=" + (host ? '0' : '1'));
}

std::string pingIncoming(const std::string &code, bool host) {
    return query("iping", "code=" + code + "&host=" + (host ? '0' : '1'));
}

bool acknowledge(const std::string &code, const std::string &sunk) {
    return query("acknowledge", "code=" + code + "&sunk=" + sunk) == "1";
}