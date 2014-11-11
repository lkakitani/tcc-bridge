#include <string>
#include <bitset>
using namespace std;

string hexToBin(string hex);
string decToBin(int number);
const string currentDateTime();
void setServletId(bitset<256> &payload, string servletId);
void setRecipient(bitset<256> &payload, string recipient);
void setUnixTime(bitset<256> &payload, string unixTime);
void setColor(bitset<256> &payload, string hexColor);
int main_mod();

