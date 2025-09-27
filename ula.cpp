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

static const string DATA_MEM_PATH = "./dados_etapa3_tarefa1.txt";
static const string REGS_PATH = "./registradores_etapa3_tarefa1.txt";
static const string MIC_PATH = "./micro_instrucoes_etapa3_tarefa1.txt";
static vector<u32> DATA_MEM(16,0u);

static void iload(int x) {
    ofstream fout(MIC_PATH, ios::app);
    if (!fout.is_open()) {
        cerr << "Erro ao abrir arquivo de saída\n";
        return;
    }

    fout << "00110100100000000000101\n";
    
    for (int i = 0; i<x; i++) {
        fout << "00111001100000000000000\n";
    }

    fout << "00111000000000001010000\n";
    fout << "00110101000001000000100\n";
    fout << "00110100000000001100100\n";
    fout << "00110100001000000000000\n";

    fout.close();
}

void dup() {
    ofstream fout(MIC_PATH, ios::app);
    if (!fout.is_open()) {
        cerr << "Erro ao abrir arquivo de saída\n";
        return;
    }

    fout << "00110101000001000000100\n";
    fout << "00110100000000001000100\n";
    fout << "00110100000000010100111\n";

    fout.close();
}

void bipush(string s) {
    ofstream fout(MIC_PATH, ios::app);
    if (!fout.is_open()) {
        cerr << "Erro ao abrir arquivo de saída\n";
        return;
    }
    
    i32 valor = static_cast<i32>(btod(s));
    H = valor;

    fout << "00110101000001001000100\n";
    fout << "00000000000000000010000\n";
    fout << "00111000001000010100000\n";

    fout.close();
}

static void load_data_mem(){
    ifstream f(DATA_MEM_PATH);
    if(!f.is_open()) return;
    string line;
    for (int i = 0; i < 16 && getline(f,line); i++){
        string bits;
        for (char c : line){
            if (c == '0' || c == '1')
                bits.push_back(c);
        }
        if (bits.size() == 32){
            DATA_MEM[i] = static_cast<u32>(btod(bits));
        } else{
            cerr << "Linha de dados invalida: '" << line << "'\n";
            DATA_MEM[i] = 0u;
        }

    }
    f.close();
}


static void save_data_mem(){
    ofstream f(DATA_MEM_PATH, ios::trunc);
    if(!f.is_open()){
        cerr << "Erro ao salvar memoria em "<< DATA_MEM_PATH << endl;
        return;
    }
    for (int i =0; i < 16; i++){
        f << toBin(static_cast<u32>(DATA_MEM[i])) << endl;// talvez seja bom tirar o static cast ja que DATA_MEM ja é u32
    }
    f.close();
}

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
        return "";
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
    dup();
    H = OPC = CPP = SP = PC = MDR = MAR = MBRU = 0;
    MBR = 0;
    SP = MAR = 4;
    TOS = 8;
    LV = 1;

    ifstream fin(MIC_PATH);
    if (!fin.is_open()) {
        cerr << "Erro ao abrir arquivo de entrada\n";
        return 1;
    }

    ofstream fout(REGS_PATH);
    if (!fout.is_open()) {
        cerr << "Erro ao abrir arquivo de saída\n";
        return 1;
    }

    load_data_mem();

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

        cout << "IR: " << ir << " C: " << c_bus << " B: " << b_bus << " MEM: " << wr << endl;

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

        bool WRITE = (wr.size() >= 1 && wr[0] == '1');
        bool READ  = (wr.size() >= 2 && wr[1] == '1');
        bool mem_conflict = WRITE && READ;
        string memLog;

        int mar_idx = static_cast<int>(MAR);
        bool mar_ok = (mar_idx >= 0 && mar_idx < 8);


        if (mem_conflict) {
            memLog = "Erro: WRITE e READ simultaneos.";
        } else if (READ) {
            if (mar_ok) {
                MDR = static_cast<i32>(DATA_MEM[mar_idx]);
                memLog = "READ: MDR <- M[" + to_string(MAR) + "]";
            } else {
                memLog = "READ ignorado: MAR fora de [0,7] (MAR=" + to_string(MAR) + ")";
            }
        } else if (WRITE) {
            if (mar_ok) {
                DATA_MEM[mar_idx] = static_cast<u32>(MDR);
                save_data_mem();
                memLog = "WRITE: M[" + to_string(MAR) + "] <- MDR";
            } else {
                memLog = "WRITE ignorado: MAR fora de [0,7] (MAR=" + to_string(MAR) + ")";
            }
        } else {
            memLog = "Sem operacao de memoria.";
        }

        fout << "============================================================\n";
        fout << "Cycle " << pc << "\n";
        if (!r.shiftConflict)
        {
            fout << "B = " << rgsb[selB] << endl;
            fout << "C = " << rgsc << endl;
            fout << "MEM = " << wr << " -> " << memLog << endl;
            fout << "\nRegisters:\n";
            fout << "MAR = " << MAR << " (" << toBin(static_cast<u32>(MAR)) << ")\n";
            fout << "MDR = " << MDR << " (" << toBin(static_cast<u32>(MDR)) << ")\n";
            fout << "PC  = " << PC  << " (" << toBin(static_cast<u32>(PC))  << ")\n";
            fout << "MBR = " << MBR << " (" << toBin(static_cast<i8>(MBR)).substr(24, 31) << ")\n";  
            fout << "SP  = " << SP  << " (" << toBin(static_cast<u32>(SP))  << ")\n";
            fout << "LV  = " << LV  << " (" << toBin(static_cast<u32>(LV))  << ")\n";
            fout << "CPP = " << CPP << " (" << toBin(static_cast<u32>(CPP)) << ")\n";
            fout << "TOS = " << TOS << " (" << toBin(static_cast<u32>(TOS)) << ")\n";
            fout << "OPC = " << OPC << " (" << toBin(static_cast<u32>(OPC)) << ")\n";  
            fout << "H   = " << H   << " (" << toBin(static_cast<u32>(H))   << ")\n";
        }
        else {
            if(r.shiftConflict)
                fout << "> Error, invalid control signals.\n\n";
            if(mem_conflict)
                fout << "> Error, simultaneous memory READ and WRITE.\n";
        }
        ++pc;
    }
    fin.close();
    fout.close();
    return 0;
}
