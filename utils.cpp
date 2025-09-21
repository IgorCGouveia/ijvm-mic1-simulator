#include <bits/stdc++.h>
#include <utils.h>

using namespace std;

long long btod(const string& b) {
    long long d = 0;
    for (size_t i = 0; i < b.size(); i++) {
        if (b[i] == '1') d += 1 << (b.size() - i - 1);
    }
    return d;
}

/* string toBin(int32_t x)
{
    string s;
    s.reserve(32);
    for (int i = 31; i >= 0; --i)
        s.push_back(((x >> i) & 1) ? '1' : '0');
    return s;
} */

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


/*
24
20
26
44
60
61
57
53
63
54
59
12
28
16
49
50*/


