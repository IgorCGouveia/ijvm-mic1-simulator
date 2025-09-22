#include <bits/stdc++.h>
#include <utils.h>

using namespace std;

long long btod(const string& b, bool signedMode) {
    long long d = 0;

    if (!signedMode) {
        for (size_t i = 0; i < b.size(); i++) {
            if (b[i] == '1') d += 1LL << (b.size() - i - 1);
        }
    } else {
        bool negativo = (b[0] == '1');
        for (size_t i = 0; i < b.size(); i++) {
            if (b[i] == '1') d += 1LL << (b.size() - i - 1);
        }
        if (negativo) {
            d -= 1LL << b.size(); 
        }
    }
    return d;
}


string toBin(int32_t x)
{
    std::string s;  //CONSIDERA O SINAL
    s.reserve(32);                  // 32 bits, inclusive o de sinal
    // converte para uint32_t só para manipular os bits
    uint32_t bits = static_cast<uint32_t>(x);
    for (int i = 31; i >= 0; --i)
        s.push_back(((bits >> i) & 1u) ? '1' : '0');
    return s;
}
