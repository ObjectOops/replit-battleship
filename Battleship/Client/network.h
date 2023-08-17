#ifndef NETWORK_H
#define NETWORK_H
#include <stdlib.h>
#include <string>

const std::string domain {getenv("REPL_PROXY_URL")};
const std::string server_key {getenv("SERVER_KEY")};
const std::string proxy {domain + '/' + server_key};

std::string query(const std::string &path, const std::string &args);

bool testConnection();

bool logConnection(const std::string &replUser, const std::string &alias);

std::string like(const std::string &replUser);

std::string sendJoinRequest(const std::string &code, const std::string &user);

bool createGame(const std::string &code, const std::string &user);

std::string pingGame(const std::string &code);

bool cancelGame(const std::string &code);

bool ready(const std::string &code, bool host);

bool pingStart(const std::string &code);

bool leave(const std::string &code);

bool fire(const std::string &code, bool host, short x, short y, const std::string &message);

std::string pingFire(const std::string &code, bool host);

std::string pingIncoming(const std::string &code, bool host);

bool acknowledge(const std::string &code, const std::string &sunk);

#endif