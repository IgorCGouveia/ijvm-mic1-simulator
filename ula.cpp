#include <utils.h>
#include <fstream>
#include <bitset>

#define MAX 2147483647
#define MIN -2147483647

// 1 - ENTRADAS E SÁIDAS DE 32 BITS
// INSTRUCAO DE 8 BITS

using namespace std;

pair<string, long long int> execULA(int opcode, long long int a, long long int b, int sll8, int sra1)
{
  long long int result = 0;
  int iout = 0;
  switch (opcode)
  {
  case 24:
    result = a;
    break;
  case 52:
    result = b;
    break;
  case 26:
    result = !a;
    break;
  case 44:
    result = !b;
    break;
  case 60:
    result = a + b;
    break;
  case 57:
    result = a + 1;
    break;
  case 53:
    result = b + 1;
    break;
  case 12:
    result = a && b;
    break;
  case 28:
    result = a | b;
    break;
  case 16:
    result = 0;
    break;
  case 49:
    result = 1;
    break;
  default:
    result = -1;
    break;
  }

  if (sll8)
    result = result << 8;

  if (sra1)
    result = result >> 1;

  cout << result << endl;

  if (result > MAX || result < MIN)
    iout = 1;

  // Para adição (a + b):
  if ((a > 0 && b > 0 && result < 0) || (a < 0 && b < 0 && result >= 0))
    iout = 1;

  // Para subtração (b - a):
  if ((b >= 0 && a < 0 && result < 0) || (b < 0 && a >= 0 && result > 0))
    iout = 1;

  return {bitset<32>(result).to_string(), iout};
}

int main(int argc, char *argv[])
{
  string A, B;
  cin.ignore();
  getline(cin, A);
  getline(cin, B);
  int PC = 1;
  int SLL8;
  int SRA1;

  ifstream input("./in.txt");
  if (!input)
  {
    printf("erro ao abrir arquivo");
    return -1;
  }

  ofstream output("./out.txt");
  if (!input)
  {
    printf("erro ao abrir arquivo");
    return -1;
  }

  string l;
  while (getline(input, l))
  {
    SLL8 = l[0] - '0';
    SRA1 = l[1] - '0';
    cout << "OP:" << btod(l.substr(2)) << endl;
    pair<string, long long int> result = execULA(btod(l.substr(2)), btod(A), btod(B), SLL8, SRA1);
    cout << result.first.substr(1) << ", " << result.second << endl;

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