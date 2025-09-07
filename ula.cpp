#include <utils.h>
#include <fstream>
#include <bitset>

using namespace std;

pair<string, int> execULA(int opcode, int a, int b, int inv, int sll8, int sra1) {
    int result = 0;
    int iout = 0;
    switch (opcode) {
        case 24: result = a; break;
        case 20: result = b; break;
        case 26: result = !a; break;
        case 44: result = !b; break;
        case 60: result = a + b; break;
        case 61: result = a + b + 1; break;
        case 57: result = a + 1; break;
        case 53: result = b + 1; break;
        case 63: result = b - a; break;
        case 54: result = b - 1; break;
        case 59: result = -a; break;
        case 12: result = a && b; break; 
        case 28: result = a || b; break;
        case 16: result = 0; break;
        case 49: result = 1; break;
        default: result = -1; break;
    }

    if (sll8) result = result<<1;
    
    if (sra1) result = result>>1;

    if(result > 1) iout = 1;
    return {bitset<1>(result).to_string(), iout};
}

int main(int argc, char* argv[]) {
  int A, B, INV; cin >> A >> B >> INV;
  int PC = 1; int SLL8; int SRA1;

  ifstream input ("./in.txt");
  if(!input){
      printf("erro ao abrir arquivo");
      return -1;
  }

  ofstream output ("./out.txt");
  if(!input){
      printf("erro ao abrir arquivo");
      return -1;
  }

  string l;
  while (getline(input, l)) {
    SLL8 = l[0] - '0';
    SRA1 = l[1] - '0';
    cout << "OP:" << btod(l.substr(2)) << endl;
    pair<string, int> result = execULA(btod(l.substr(2)), A, B, INV, SLL8, SRA1);
    cout << result.first << ", " << result.second << endl;

    output << "PC=" << PC
      << " IR=" << l
      << " A=" << A
      << " B=" << B
      << " S=" << result.first
      << " Vai-um=" << result.second
      << " Z= " << ((result.first[0] - '0') ? 0 : 1)
      << " N= ?"
      << endl;

    PC++;
  }

  input.close();
    
  return 0;
}