#include <bits/stdc++.h>
#include <utils.h>
using namespace std;
using u32 = uint32_t;
using i32 = int32_t;
using i8 = int8_t;

i32 H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
i8 MBR;

string toBin(u32 x)
{
    string s;
    s.reserve(32);
    for (int i = 31; i >= 0; --i)
        s.push_back(((x >> i) & 1u) ? '1' : '0');
    return s;
}

struct ULARes
{
    u32 s;
    u32 sd;
    int carry;
    int N;
    int Z;
    bool shiftConflict;
};

ULARes ula(i32 A, i32 B,
           bool SLL8, bool SRA1,
           int F0, int F1, int ENA, int ENB, int INVA, int INC)
{
    u32 ua = ENA ? static_cast<u32>(A) : 0u;
    if (INVA)
        ua = ~ua;
    u32 ub = ENB ? static_cast<u32>(B) : 0u;

    u32 op;
    if (F0 == 0 && F1 == 0)
        op = ua & ub;
    else if (F0 == 0 && F1 == 1)
        op = ua | ub;
    else if (F0 == 1 && F1 == 0)
        op = ~ub;
    else
    { // F0==1 && F1==1 -> soma
        uint64_t sum = (uint64_t)ua + (uint64_t)ub + (INC ? 1ULL : 0ULL);
        u32 s = static_cast<u32>(sum & 0xFFFFFFFFu);
        int carry = static_cast<int>((sum >> 32) & 1u);

        u32 sd = s;
        bool conflict = (SLL8 && SRA1);
        if (!conflict)
        {
            if (SLL8)
                sd = (sd << 8);
            else if (SRA1)
            {
                // deslocamento aritmético: replicar bit de sinal
                if (sd & 0x80000000u)
                    sd = (sd >> 1) | 0x80000000u;
                else
                    sd = sd >> 1;
            }
        }
        int Z = (sd == 0u) ? 1 : 0;
        int N = ((sd & 0x80000000u) != 0u) ? 1 : 0;
        return {s, sd, carry, N, Z, conflict};
    }

    uint64_t tmp = (uint64_t)op + (INC ? 1ULL : 0ULL);
    u32 s = static_cast<u32>(tmp & 0xFFFFFFFFu);
    int carry = static_cast<int>((tmp >> 32) & 1u);

    u32 sd = s;
    bool conflict = (SLL8 && SRA1);
    if (!conflict)
    {
        if (SLL8)
            sd = (sd << 8);
        else if (SRA1)
        {
            if (sd & 0x80000000u)
                sd = (sd >> 1) | 0x80000000u;
            else
                sd = sd >> 1;
        }
    }
    int Z = (sd == 0u) ? 1 : 0;
    int N = ((sd & 0x80000000u) != 0u) ? 1 : 0;
    return {s, sd, carry, N, Z, conflict};
}

i32 decodificador_4_para9(string &b_bus)
{
    // Converte os 4 bits de binário para decimal
    int sel = static_cast<int>(btod(b_bus));

    // Seleciona o registrador que vai para o barramento B , ou seja, o que vai pasar informaçao pa a ULA
    switch (sel)
    {
    case 0:
        return H;
    case 1:
        return OPC;
    case 2:
        return TOS;
    case 3:
        return CPP;
    case 4:
        return LV;
    case 5:
        return SP;
    case 6:
        return PC;
    case 7:
        return MDR;
    case 8:
        return MAR;
    default:
        return 0; // código inválido
    }
}
// Escreve nos regstring c_buses habilitados
// Recebe:
// - c_bus: 9 bits, cada bit habilita um registrador
// - valor: saída da ULA (C-bus)
// Escreve nos registradores habilitados
void selc_bus9bits(int c_bus, i32 valor)
{
    if (c_bus & (1 << 0))
        H = valor;
    if (c_bus & (1 << 1))
        OPC = valor;
    if (c_bus & (1 << 2))
        TOS = valor;
    if (c_bus & (1 << 3))
        CPP = valor;
    if (c_bus & (1 << 4))
        LV = valor;
    if (c_bus & (1 << 5))
        SP = valor;
    if (c_bus & (1 << 6))
        PC = valor;
    if (c_bus & (1 << 7))
        MDR = valor;

    if (c_bus & (1 << 8))
        MAR = valor;
}

int main(int argc, char **argv)
{
    // string BIN_A; getline(cin, BIN_A);
    // string BIN_B; getline(cin, BIN_B);
    // i32 A = BIN_A[0] == '1' ? - btod(BIN_A) : btod(BIN_A);
    i32 A = -1;
    i32 B = 1;

    if (argc >= 4)
    {
        long long tmpA = stoll(argv[2], nullptr, 0);
        long long tmpB = stoll(argv[3], nullptr, 0);
        A = static_cast<i32>(tmpA);
        B = static_cast<i32>(tmpB);
    }

    ifstream fin("./in.txt");
    if (!fin.is_open())
    {
        cerr << "Erro ao abrir arquivo de entrada\n";
        return 1;
    }

    ofstream fout("./out.txt");
    if (!fin.is_open())
    {
        cerr << "Erro ao abrir arquivo de saída\n";
        return 1;
    }

    fout << "Start of Program\n";

    string raw;
    int pc = 1;
    while (getline(fin, raw))
    {

        string instr;
        string ir = raw.substr(0, 7);
        string c_bus = raw.substr(8, 16);
        string b_bus = raw.substr(17);

        for (char c : raw)
            if (c == '0' || c == '1')
                instr.push_back(c);
        if (instr.size() != 8)
            continue;

        bool SLL8 = instr[0] == '1';
        bool SRA1 = instr[1] == '1';
        int F0 = instr[2] - '0';
        int F1 = instr[3] - '0';
        int ENA = instr[4] - '0';
        int ENB = instr[5] - '0';
        int INVA = instr[6] - '0';
        int INC = instr[7] - '0';

        ULARes r = ula(A, B, SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC);
        fout << "============================================================\n";
        fout << "Cycle " << pc << "\n";
        if (!r.shiftConflict)
        {
            fout << "PC = " << pc << "\n";
            fout << "IR = " << instr << "\n";
            fout << "b  = " << toBin(static_cast<u32>(B)) << "\n";
            fout << "a  = " << toBin(static_cast<u32>(A)) << "\n";
            fout << "s  = " << toBin(r.s) << "\n";
            fout << "sd = " << toBin(r.sd) << "\n";
            fout << "n = " << r.N << "\n";
            fout << "z = " << r.Z << "\n";
            fout << "co= " << r.carry << "\n";
        }
        else
        {
            fout << "> Error, invalid control signals.\n";
            fout << "\n";
        }
        ++pc;
    }
    fin.close();
    return 0;
}
