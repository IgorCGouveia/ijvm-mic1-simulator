#include <utils.h>

using namespace std;

int btod (string b) {
  int d = 0;

  for (int i=0; i<b.size(); i++) {
    if (b[i] == '1') d += pow(2, b.size() - i - 1);
  }

  return d;
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


