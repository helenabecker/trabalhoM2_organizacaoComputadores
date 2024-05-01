#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Alunas: Helena Becker Piazera e Izabela Andreazza

string* aumentarVetor(string* a, int tam, int novo_tam) {
    string* aux = new string[novo_tam];
    for (int i = 0; i < tam; i++) {
        aux[i] = a[i];
    }
    delete[]a;
    return aux;
}

int contar_linhas_arquivo(string nome_arquivo) {
    ifstream arquivo;
    arquivo.open(nome_arquivo);
    int contador = 0;
    string linha;

    if (arquivo.is_open()) {
        while (getline(arquivo, linha)) {
            if (linha.empty() == false) {
                contador++;
            }
        }
        arquivo.close();
        return contador;
    }
    else return -1;
}

void ler_arquivo(string vetor[], int tam, string nome_arquivo) {
    ifstream arquivo;
    arquivo.open(nome_arquivo);
    string linha, invertida;

    if (arquivo.is_open()) {
        for (int i = 0; i < tam; i++) {
            getline(arquivo, linha);

            if (!linha.empty()) {
                invertida = "";
                for (int j = 31; j >= 0; j--) { //inverte string
                    invertida += linha[j];
                }
                vetor[i] = invertida;
            }
        }
        arquivo.close();
    }
    else
        cout << "\nErro ao abrir o arquivo\n";
}

void escrever_arquivo(string vetor[], int tam, string nome_arquivo) {
    ofstream arquivo;
    string linha, invertida;

    arquivo.open(nome_arquivo);
    if (arquivo.is_open()) {
        for (int i = 0; i < tam; i++) {
            linha = vetor[i];
            invertida = "";
            for (int j = 31; j >= 0; j--) {
                invertida += linha[j];
            }
            arquivo << invertida << endl;
        }
        arquivo.close();
    }
    else cout << "\nErro ao abrir o arquivo\n";
}

void mostrar_binarios(string vetor[], int tam) {
    cout << "\n\nConteudo do vetor: " << endl;
    for (int i = 0; i < tam; i++) {
        cout << "\tLinha " << i << ": " << vetor[i] << endl;
    }
    cout << endl;
}

char identificar_opcode(string linha) {
    string opcode = "";
    for (int i = 0; i <= 6; i++) {
        opcode += linha[i];
    }
    if (opcode == "1100110") //formato R
        return 'R';
    if (opcode == "1100011") //formato B 
        return 'B';
    if (opcode == "1100010") //formato S 
        return 'S';
    if (opcode == "1111011") //formato J 
        return 'J';
    if (opcode == "1110100" || opcode == "1110110") //formato U
        return 'U';
    else
        return 'I'; //formato I
}

string identificar_rd(string linha) {
    string rd = "";
    for (int i = 7; i <= 11; i++) {
        rd += linha[i];
    }
    return rd;
}

string identificar_rs(string linha, int bit_comeco, int bit_final) {
    string rs = "";
    for (int i = bit_comeco; i <= bit_final; i++) {
        rs += linha[i];
    }
    return rs;
}

void indentificar_partes(string linha, char &opcode, string& rd, string& rs1, string& rs2) {
    //identificar opcode
    opcode = identificar_opcode(linha);

    //identidicar rd, rs1 e rs2 (se tiver) de acordo com o formato da instrução
    if (opcode == 'R') {
        rd = identificar_rd(linha);
        rs1 = identificar_rs(linha, 15, 19);
        rs2 = identificar_rs(linha, 20, 24);
    }
    else if (opcode == 'I') {
        rd = identificar_rd(linha);
        rs1 = identificar_rs(linha, 15, 19);
        rs2 = "";
    }
    else if (opcode == 'B') {
        rd = "";
        rs1 = identificar_rs(linha, 15, 19);
        rs2 = identificar_rs(linha, 20, 24);
    }
    else if (opcode == 'S') {
        rs1 = identificar_rs(linha, 15, 19);
        rs2 = identificar_rs(linha, 20, 24);
    }
    else if (opcode == 'U') {
        rd = identificar_rd(linha);
        rs1 = "";
        rs2 = "";
    }
    else if (opcode == 'J') {
        rd = identificar_rd(linha);
        rs1 = "";
        rs2 = "";
    }
}

void adicionarNOP(string vetor[], int tamanho, int posicao) {
    for (int i = tamanho - 1; i > posicao; --i) {
        vetor[i] = vetor[i - 1];
    }
    vetor[posicao] = "11001100000000000000000000000000";
}

double calcular_desempenho(double ideal, double tempo_real) {
    return tempo_real / ideal;
}

double calcular_CPI(int n_instrucoes) {
    return (5 + (1 * n_instrucoes - 1)) / n_instrucoes;
}

double calcular_tempo_execucao(int n_instrucoes, double tempo_clock) {
    double cpi = calcular_CPI(n_instrucoes);
    return n_instrucoes * cpi * tempo_clock;
}


int main()
{
    string arq_abrir = "teste.txt";
    string arq_salvar = "pipeline_NOP.txt";

    int* tam = new int;
    *tam = contar_linhas_arquivo(arq_abrir);
    if (*tam == -1)
        return 0;

    int num_ideal = *tam;

    string* binarios = new string[*tam];
    ler_arquivo(binarios, *tam, arq_abrir); //ler arquivo com instruções em binario

    char opcode, opcode_a, opcode_b;
    string rd, rs1, rs2;
    string rd_a, rs1_a, rs2_a;
    string rd_b, rs1_b, rs2_b;
    bool conflito_a = false, conflito_b = false;
    int i = 0;

    while (i < *tam - 1) {
        //pega uma instrução e identifica as informações importantes
        indentificar_partes(binarios[i], opcode, rd, rs1, rs2);

        if (opcode == 'R' || opcode == 'I') { //tipos que guardam informações no rd
            
            //pega instrução A (i + 1) e suas informações
            indentificar_partes(binarios[i + 1], opcode_a, rd_a, rs1_a, rs2_a);

            //se ainda houevrem poisções no vetor, pega instrução B (i + 2) e suas informações
            if (i < *tam - 2) {
                indentificar_partes(binarios[i + 2], opcode_b, rd_b, rs1_b, rs2_b);
            }

            conflito_a = false;
            conflito_b = false; 

            if (opcode_a == 'U' || opcode_a == 'J') { //U ou J: compara rd com rd
                if (rd == rd_a) {
                    conflito_a = true;
                    binarios = aumentarVetor(binarios, *tam, *tam + 2);
                    *tam = *tam + 2;
                    adicionarNOP(binarios, *tam, i + 1);
                    adicionarNOP(binarios, *tam, i + 2);
                    i = i + 3; //avançar para além do conflito
                }
            }
            if (opcode_b == 'U' || opcode_b == 'J') { //U ou J: compara rd com rd
                if (rd == rd_b) {
                    conflito_b = true;
                    binarios = aumentarVetor(binarios, *tam, *tam + 1);
                    *tam = *tam + 1;
                    adicionarNOP(binarios, *tam, i + 1);
                    i = i + 2; //avançar para além do conflito
                }
            }
            if (opcode_b != 'U' && opcode_b != 'J' && opcode_a != 'U' && opcode_a != 'J') { //outros formatos: compara rd com rs1 e rs2
                if (rd == rs1_a || rd == rs2_a) {
                    conflito_a = true;
                    binarios = aumentarVetor(binarios, *tam, *tam + 2);
                    *tam = *tam + 2;
                    adicionarNOP(binarios, *tam, i + 1);
                    adicionarNOP(binarios, *tam, i + 2);
                    i = i + 3; //avançar para além do conflito
                }
                else if (rd == rs1_b || rd == rs2_b) {
                    conflito_b = true;
                    binarios = aumentarVetor(binarios, *tam, *tam + 1);
                    *tam = *tam + 1;
                    adicionarNOP(binarios, *tam, i + 1);
                    i = i + 2; //avançar para além do conflito
                }
            }
            if (!conflito_a && !conflito_b) { //se não houver conflitos, segue para a proxima linha
                i++;
            }
        }
        else { //segue para a proxima linha
            i++;
        }
    }

    //criar arquivo com inserção de NOP
    escrever_arquivo(binarios, *tam, arq_salvar);

    //calculo do desempenho a partir do tempo de clock inserido
    double tempo_clock = 0;
    do {
        cout << "\nTempo de Clock: ";
        cin >> tempo_clock;
    } while (tempo_clock < 0);

    int num_instrucoes = *tam;

    double tempo_ideal = calcular_tempo_execucao(num_ideal, tempo_clock);
    double tempo_real = calcular_tempo_execucao(num_instrucoes, tempo_clock);
    double desempenho = calcular_desempenho(tempo_ideal, tempo_real);

    cout << "\n\tDesempenho ideal: " << tempo_ideal << endl;
    cout << "\tDesempenho com solucao de conflito: " << tempo_real << endl;
    cout << "\tPipeline ideal eh " << desempenho << " vezes melhor" << endl << endl;
    
    //limpa memória alocada
    delete[]binarios; 
    delete tam;
}