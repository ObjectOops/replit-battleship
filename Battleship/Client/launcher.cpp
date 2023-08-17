#include <iostream>
#include <string>
#include <unistd.h>

#define XOR_BITS 251

const char32_t REPL_PROXY_URL [] {147, 143, 143, 139, 136, 193, 212, 212, 185, 154, 143, 143, 151, 158, 136, 147, 146, 139, 168, 158, 137, 141, 158, 137, 213, 154, 151, 158, 131, 130, 154, 148, 200, 213, 137, 158, 139, 151, 213, 152, 148, XOR_BITS};
const char32_t SERVER_KEY [] {202, 201, 203, 194, 159, 152, 159, 207, 154, 159, 202, 154, 157, 206, 194, 207, 200, 203, 207, 153, 200, 153, 152, 202, 154, 195, 153, 158, 202, 200, 201, 202, 153, 159, 158, 194, 158, 200, 200, 205, 152, 157, 157, 153, 200, 194, 205, 195, 158, 194, 159, 195, 207, 159, 195, 201, 157, 158, 152, 158, 203, 157, 159, 159, XOR_BITS};
const char32_t REPL_SERVER_SOURCE [] {147, 143, 143, 139, 136, 193, 212, 212, 137, 158, 139, 151, 146, 143, 213, 152, 148, 150, 212, 187, 186, 151, 158, 131, 162, 154, 148, 200, 212, 185, 154, 143, 143, 151, 158, 136, 147, 146, 139, 168, 158, 137, 141, 158, 137, 216, 150, 154, 146, 149, 213, 139, 130, XOR_BITS};
// const char32_t ORIGIN [] {200, 202, 152, 203, 153, 204, 202, 201, 214, 153, 159, 203, 194, 214, 207, 204, 200, 154, 214, 153, 207, 202, 159, 214, 204, 206, 194, 202, 158, 200, 194, 206, 203, 205, 202, 201, XOR_BITS};
const char32_t CHECK_SUM [] {202, 158, 157, 203, 194, 158, 203, 153, 158, 206, 206, 204, 200, 157, 159, 202, 195, 201, 207, 204, 202, 204, 158, 195, 206, 195, 207, 158, 206, 206, 153, 159, 201, 205, 205, 152, 153, 200, 154, 204, 195, 195, 200, 153, 203, 158, 206, 205, 159, 157, 202, 201, 157, 207, 203, 153, 195, 159, 195, 194, 201, 194, 201, 152, 219, 219, 153, 154, 143, 143, 151, 158, 136, 147, 146, 139, XOR_BITS};

using namespace std;

string unscramble(const char32_t *arr) {
    string original;
    for (int i {}; arr[i] != XOR_BITS; ++i) original += arr[i] ^ XOR_BITS;
    return original;
}

int main() {
    string id {getenv("REPL_ID")};// , origin {unscramble(ORIGIN)};
    string checksum {unscramble(CHECK_SUM)};
    system("sh compile.sh");
    FILE *pipe {popen("/bin/sha256sum battleship", "r")};
    char bin_h [2048];
    fgets(bin_h, 2048, pipe);
    pclose(pipe);
    if (/*(id.substr(id.rfind("-source-") + 8) != origin && id != origin) || */(checksum + '\n' != bin_h)) return 0;
    setenv("REPL_PROXY_URL", unscramble(REPL_PROXY_URL).c_str(), 0);
    setenv("SERVER_KEY", unscramble(SERVER_KEY).c_str(), 0);
    setenv("REPL_SERVER_SOURCE", unscramble(REPL_SERVER_SOURCE).c_str(), 0);
    execl("battleship", "battleship", nullptr);
    return 0;
}