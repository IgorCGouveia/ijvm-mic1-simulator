#include <bits/stdc++.h>
#include <utils.h>
using namespace std;
using u32 = uint32_t;
using i32 = int32_t;
using i8 = int8_t;

i32 H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR , MBRU;
i8 MBR;

string rgsc;
string rgsb[] = { "MDR", "PC", "MBR", "MBRU", "SP", "LV", "CPP", "TOS", "OPC" };

struct ULARes {
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
    else {
        uint64_t sum = (uint64_t)ua + (uint64_t)ub + (INC ? 1ULL : 0ULL);
        u32 s = static_cast<u32>(sum & 0xFFFFFFFFu);
        int carry = static_cast<int>((sum >> 32) & 1u);

        u32 sd = s;
        bool conflict = (SLL8 && SRA1);
        if (!conflict) {
            if (SLL8)
                sd = (sd << 8);
            else if (SRA1) {
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
    if (!conflict) {
        if (SLL8)
            sd = (sd << 8);
        else if (SRA1) {
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

// retorna índice 0..8 a partir dos 4 bits do barramento B
int decodificador_idx(const string &b_bus)
{
    int sel = static_cast<int>(btod(b_bus));
    if (sel < 0 || sel > 8) {
        cerr << "Decodificador 4 para 9 Invalido (indice): " << sel << endl;
        return -1;
    }
    return sel;
}

i32 valor_reg_por_indice(int sel)
{
    switch (sel) {
    case 0: return MDR;
    case 1: return PC;
    case 2: return MBR;
    case 3: return MBRU;
    case 4: return SP;
    case 5: return LV;
    case 6: return CPP;
    case 7: return TOS;
    case 8: return OPC;
    default: return 0;
    }
}

// Escreve nos regstring c_buses habilitados
void selc_bus9bits(string c_bus, i32 valor)
{
    if (c_bus[0] == '1') {
        H = valor;
        rgsc += "H, ";
    }
    if (c_bus[1] == '1') {
        OPC = valor;
        rgsc += "OPC, ";
    }
    if (c_bus[2] == '1') {
        TOS = valor;
        rgsc += "TOS, ";
    }
    if (c_bus[3] == '1') {
        CPP = valor;
        rgsc += "CPP, ";
    }
    if (c_bus[4] == '1') {
        LV = valor;
        rgsc += "LV, ";
    }
    if (c_bus[5] == '1') {
        SP = valor;
        rgsc += "SP, ";
    }
    if (c_bus[6] == '1') {
        PC = valor;
        rgsc += "PC, ";
    }
    if (c_bus[7] == '1'){       
        MDR = valor;
        rgsc += "MDR, ";
    }    
    if (c_bus[8] == '1'){       
        MAR = valor;
        rgsc += "MAR, ";
    }    
}

string get_data(string &path, int line) {
    int count = 0;
    string content_line;

    ifstream f(path);
    if(!f.is_open()) {
        cerr << "Erro ao abrir arquivo de entrada\n";
        return;
    }

    while (getline(f, content_line)) {
        if (count == line) break;
        count++;
    }
    
    f.close();
    
    return content_line;
}

int main(int argc, char **argv)
{
    OPC = CPP = LV = SP = PC = MDR = MAR = MBRU = 0;
    MBR = -127;
    TOS = 2;
    H = 1;

    ifstream fin("./in.txt");
    if (!fin.is_open()) {
        cerr << "Erro ao abrir arquivo de entrada\n";
        return 1;
    }

    ofstream fout("./out.txt");
    if (!fout.is_open()) {
        cerr << "Erro ao abrir arquivo de saída\n";
        return 1;
    }

    fout << "Start of Program\n";

    string raw;
    int pc = 1;
    while (getline(fin, raw))
    {
        rgsc.clear();

        if (raw.size() < 21) {
            cerr << "Linha muito curta: '" << raw << "'\n";
            continue;
        }

        string instr;
        string ir = raw.substr(0, 8);
        string c_bus = raw.substr(8, 9);
        string wr = raw.substr(17, 2);
        string b_bus = raw.substr(19, 4);

        cout << "IR: " << ir << " C: " << c_bus << " B: " << b_bus << endl;

        for (char c : ir)
            if (c == '0' || c == '1')
                instr.push_back(c);
        if (instr.size() != 8)
            continue;

        int selB = decodificador_idx(b_bus);
        if (selB < 0) {
            fout << "Invalid B selector\n";
            ++pc;
            continue;
        }

        i32 B = selB == 3 ? valor_reg_por_indice(selB-1) :  valor_reg_por_indice(selB);

        bool SLL8 = instr[0] == '1';
        bool SRA1 = instr[1] == '1';
        int F0 = instr[2] - '0';
        int F1 = instr[3] - '0';
        int ENA = instr[4] - '0';
        int ENB = instr[5] - '0';
        int INVA = instr[6] - '0';
        int INC = instr[7] - '0';

        ULARes r = ula(H, B, SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC);

        bool considera_sinal = (selB != 3);

        if (considera_sinal) selc_bus9bits(c_bus, r.sd);
        else selc_bus9bits(c_bus, btod(toBin(r.sd).substr(24, 31)));

        

        fout << "============================================================\n";
        fout << "Cycle " << pc << "\n";
        if (!r.shiftConflict)
        {
            fout << "B = " << rgsb[selB] << endl;
            fout << "C = " << rgsc << endl;
            fout << "\nRegisters:\n";
            fout << "H   = " << H   << " (" << toBin(static_cast<u32>(H))   << ")\n";
            fout << "OPC = " << OPC << " (" << toBin(static_cast<u32>(OPC)) << ")\n";
            fout << "TOS = " << TOS << " (" << toBin(static_cast<u32>(TOS)) << ")\n";
            fout << "CPP = " << CPP << " (" << toBin(static_cast<u32>(CPP)) << ")\n";
            fout << "LV  = " << LV  << " (" << toBin(static_cast<u32>(LV))  << ")\n";
            fout << "SP  = " << SP  << " (" << toBin(static_cast<u32>(SP))  << ")\n";
            fout << "PC  = " << PC  << " (" << toBin(static_cast<u32>(PC))  << ")\n";
            fout << "MDR = " << MDR << " (" << toBin(static_cast<u32>(MDR)) << ")\n";
            fout << "MAR = " << MAR << " (" << toBin(static_cast<u32>(MAR)) << ")\n";
            fout << "MBR = " << MBR << " (" << toBin(static_cast<i8>(MBR)).substr(24, 31) << ")\n";
           // fout << "MBRU = " << MBRU << " (" << toBin(static_cast<u32>(MBRU)) << ")\n";
        }
        else {
            fout << "> Error, invalid control signals.\n\n";
        }
        ++pc;
    }
    fin.close();
    fout.close();
    return 0;
}
