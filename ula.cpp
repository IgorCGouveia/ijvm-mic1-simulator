#include <utils.h>
#include <fstream>
#include <bitset>

using namespace std;

string execULA(int opcode, int a, int b) {
    int result = 0;
    switch (opcode) {
        case 24: result = a; break;
        case 20: result = b; break;
        case 26: result = ~a; break;
        case 44: result = ~b; break;
        case 60: result = a + b; break;
        case 61: result = a + b + 1; break;
        case 57: result = a + 1; break;
        case 53: result = b + 1; break;
        case 63: result = b - a; break;
        case 54: result = b - 1; break;
        case 59: result = -a; break;
        case 12: result = a & b; break; 
        case 28: result = a || b; break;
        case 16: result = 0; break;
        case 49: result = 1; break;
        default: result = -1; break;
    }
    return bitset<1>(result).to_string();
}

int main(int argc, char* argv[]) {
  int A, B; cin >> A >> B;

  ifstream input ("./in.txt");
  if(!input){
      printf("erro ao abrir arquivo");
      return -1;
  }

  string l;
  while (getline(input, l)) {
    cout << execULA(btod(l), A, B) << endl;
  }

  input.close();
    
  return 0;
}