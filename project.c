
/**
 * Um programa para gerir parques de estacionamento.
 * @file ex.c
 * @author ist1109778
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_MATRICULA 9
#define TAMANHO_DATA 11
#define TAMANHO_HORA 6

#define MAXPARKS 20
#define MAXMATRICULAS 10000
/** Tamanho máximo dos buffers lidos pelo user input. */
#define MAXSIZE 6001 
#define MAX_REGISTROS 10000


int matricula_existe_no_parque(char nomeparque[],char matricula[]);
int existe_parque(char nomeparque[]);
int calcula_diferenca_tempo(char data_entrada[], char hora_entrada[], char data_saida[], char hora_saida[]);
int valida_data(const char *data,const char*hora);
void encontra_data_e_hora_recentes(char nome[], char *data_recente, char *hora_recente);
void criarParque(char nome[], int capacidade, float valor15, float valor15_apos1hora, float valormax_diario);
void comando_e(char comando[]);
void valida_carro(char nomeparque[], char matricula[], char data[], char hora[]);
int valida_matricula(const char *matricula);
void entra_carro(char nomeparque[], char matricula[], char data[], char hora[]);
int encontra_matricula(char matricula[]);
void mostra_faturacao(char nome_parque[],char data[]);
void listar_parques();
void listar_parques_restantes();
void remove_carro(int index_parque, int index_matricula, char data_saida[],char hora_saida[]);

typedef struct{
    char **nomes_parque;
    char **datas_entrada;
    char **datas_saida;
    char **matriculas;
    char **horas_saida;
    char **horas_entrada;
    char **faturacao;
    int num_registros;
    int num_matriculas;

}FaturacaoDiaria;
FaturacaoDiaria faturacao_diaria;

void calcula_faturacao_parque(char *nome_parque);
typedef struct {
    char nome[MAXSIZE];
    int capacidade;
    char matriculas[MAXMATRICULAS][TAMANHO_MATRICULA];
    char datas_entrada[MAXMATRICULAS][TAMANHO_DATA];
    char horas_entrada[MAXMATRICULAS][TAMANHO_HORA];
    char datas_saida[TAMANHO_DATA]; 
    char horas_saida[TAMANHO_HORA];
    float valor15;
    float valor15_apos1hora;
    float valormax_diario;
    float faturacao;
    int lugares_disponiveis;
} ParqueEstacionamento;

int num_parques = 0;
ParqueEstacionamento parques[MAXPARKS];

struct DataHora {
    int dia, mes, ano, hora, minuto;
};
const int SEGUNDOS_EM_DIA = 86400;

typedef struct {
    char **matriculas;
    char **nomes_parque;
    char **datas_entrada;
    char **horas_entrada;
    char **horas_saida;
    char **datas_saida;
    int num_registros;
    int num_matriculas;
} Registro;
Registro registro;

typedef struct{
char data_atual[TAMANHO_DATA];
char hora_atual[TAMANHO_DATA];

}Relogio;
Relogio relogio;
/**
   Inicializa as strings do struct relógio como strings nulas.
*/
void inicializar_relogio(){
    strcpy(relogio.data_atual, "");
    strcpy(relogio.hora_atual, "");
}

void atualiza_relogio(char nova_data[],char nova_hora[]){
    strcpy(relogio.data_atual,nova_data);
    strcpy(relogio.hora_atual,nova_hora);
    
}

/**
   Devolve o indice onde está armazenado o parque 
   @param nome nome do parque
   @return o indice ou -1 caso este nao exista.
*/
int devolve_index_parque(char nome[]){
    for (int i = 0; i < MAXPARKS; i++) {
        if (strcmp(parques[i].nome, nome) == 0) {
            return i;
            
        }
    }
    return -1;
}

/**
   Verifica se o parque existe ou não no sistema
   @param nome nome do parque
   @return 1 se existir, 0 se não existir
*/
int existe_parque(char nome[]) {
    if (devolve_index_parque(nome) == -1) {
        return 0;
    } else {
        return 1;
    }
}

/**
   Elimina todos os movimentos do registo (para o comando v) que contiverem o nome do parque que foi removido
   @param nome_parque nome do parque
   @return 1 se existir, 0 se não existir
*/
void eliminar_movimentos_por_parque(Registro *registro, char *nome_parque) {
    int i, j;
    for (i = 0; i < registro->num_registros; i++) {
        if (strcmp(registro->nomes_parque[i], nome_parque) == 0) {
            for (j = i; j < registro->num_registros - 1; j++) {
                registro->matriculas[j] = registro->matriculas[j + 1];
                registro->nomes_parque[j] = registro->nomes_parque[j + 1];
                registro->datas_entrada[j] = registro->datas_entrada[j + 1];
                registro->horas_entrada[j] = registro->horas_entrada[j + 1];
                registro->horas_saida[j] = registro->horas_saida[j + 1];
                registro->datas_saida[j] = registro->datas_saida[j + 1];
            }
            registro->num_registros--;
            i--;
        }
    }
}

/**
   Elimina todos os movimentos do registo (para o comando v) que contiverem o nome do parque que foi removido
   @param nome_parque nome do parque
   @return 1 se existir, 0 se não existir
*/
void remover_parque(char nome[]) {
    if (!existe_parque(nome)) {
        printf("%s: no such parking.\n", nome);
    }
    
    int indice_para_remover = devolve_index_parque(nome);

    strcpy(parques[indice_para_remover].nome, "");
    parques[indice_para_remover].capacidade = 0;
    parques[indice_para_remover].valor15 = 0;
    parques[indice_para_remover].valor15_apos1hora = 0;
    parques[indice_para_remover].valormax_diario = 0;

    for (int j = 0; j < parques[indice_para_remover].capacidade; j++) {
        strcpy(parques[indice_para_remover].matriculas[j], "");
        parques[indice_para_remover].lugares_disponiveis = 0;
    }
    
    eliminar_movimentos_por_parque(&registro, nome);
    num_parques--;
    
    listar_parques_restantes();
}

/**
   Inicializa o sistema, colocando todos os parques num estado vazio.
   Isso inclui limpar os nomes, capacidade e valores associados a cada parque, 
   garantindo que o sistema comece num estado limpo.
*/
void inicializar_sistema() {
    for (int i = 0; i < MAXPARKS; i++) {
        strcpy(parques[i].matriculas[0], "");
        strcpy(parques[i].nome, "");
        parques[i].capacidade = 0;
        parques[i].valor15 = 0;
        parques[i].valor15_apos1hora = 0;
        parques[i].valormax_diario = 0;
        parques[i].lugares_disponiveis = 0;
    }
}

/**
   Encontra o índice do primeiro parque vazio no vetor de parques.
   Retorna -1 se nenhum parque vazio for encontrado.
   @return O índice do parque vazio ou -1 se nenhum for encontrado.
*/
int encontrar_index_parque_vazio() {
    for (int i = 0; i < MAXPARKS; i++) {
        if (strcmp(parques[i].nome, "") == 0) {
            return i;
        }
    }
    return -1;
}

/**
   Cria um novo parque com as informações fornecidas.
   @param nome Nome do novo parque.
   @param capacidade Capacidade máxima de veículos do parque.
   @param valor15 Valor de cobrança inicial até 1 hora.
   @param valor15_apos1hora Valor de cobrança após 1 hora.
   @param valormax_diario Valor máximo de cobrança diária.
*/
void criarParque(char nome[], int capacidade, float valor15, float valor15_apos1hora, float valormax_diario) {
    // Encontra o índice do primeiro parque vazio
    int i = encontrar_index_parque_vazio();


    for (size_t c=0;c<strlen(nome);c++){
        if (!isalpha(nome[c])){
            printf("invalid parking name.\n");
            return;
        }
    }
    
    // Verifica se não há mais parques disponíveis
    if (i == -1) {
        printf("too many parks.\n");
        return;
    }
    
    // Verifica se já existe um parque com o mesmo nome
    for (int j = 0; j < MAXPARKS; j++) {
        if (strcmp(parques[j].nome, nome) == 0) {
            printf("%s: parking already exists.\n", nome);
            return;
        }
    }
    
    // Verifica se a capacidade é válida
    if (capacidade <= 0) {
        printf("%d: invalid capacity.\n", capacidade);
        return;
    }
    
    // Verifica se os valores de cobrança são válidos
    if (valor15 <= 0 || valor15_apos1hora <= valor15 || valormax_diario <= valor15_apos1hora) {
        printf("invalid cost.\n");
        return;
    }
    
    // Limpa as matrículas antigas no parque, caso existam
    for (int j = 0; j < capacidade; j++) {
        if (strcmp(parques[i].matriculas[j], "") != 0) {
            strcpy(parques[i].matriculas[j], "");
        }
    }
    
    // Define as informações do novo parque
    strcpy(parques[i].nome, nome);
    parques[i].capacidade = capacidade;
    parques[i].valor15 = valor15;
    parques[i].valor15_apos1hora = valor15_apos1hora;
    parques[i].valormax_diario = valormax_diario;
    parques[i].lugares_disponiveis = capacidade;

    // Incrementa o número total de parques
    num_parques++;
}


/**
   Calcula a faturação total para um determinado parque.
   @param nome_parque Nome do parque para o qual a faturação será calculada.
*/
void calcula_faturacao_parque(char *nome_parque) {
    // Iterar sobre os registros de faturação diária
    for (int i = 0; i < faturacao_diaria.num_registros; i++) {
        // Verificar se o nome do parque corresponde ao parque atual
        if (strcmp(faturacao_diaria.nomes_parque[i], nome_parque) == 0) {
            // Verificar se esta data de saída já foi processada anteriormente
            int data_processada = 0;
            for (int j = 0; j < i; j++) {
                // Verificar se a data de saída e o nome do parque correspondem a registros anteriores
                if (strcmp(faturacao_diaria.datas_saida[j], faturacao_diaria.datas_saida[i]) == 0 &&
                    strcmp(faturacao_diaria.nomes_parque[j], nome_parque) == 0) {
                    data_processada = 1; // A data de saída já foi processada anteriormente
                    break;
                }
            }

            // Se a data de saída não foi processada anteriormente
            if (!data_processada) {
                // Calcular a faturação total para esta data de saída
                double faturacao_data = atof(faturacao_diaria.faturacao[i]);
                for (int j = i + 1; j < faturacao_diaria.num_registros; j++) {
                    // Verificar se os registros correspondem à mesma data de saída e ao mesmo parque
                    if (strcmp(faturacao_diaria.datas_saida[i], faturacao_diaria.datas_saida[j]) == 0 &&
                        strcmp(faturacao_diaria.nomes_parque[i], faturacao_diaria.nomes_parque[j]) == 0) {
                        faturacao_data += atof(faturacao_diaria.faturacao[j]); // Adicionar à faturação total
                    }
                }

                // Imprimir a data de saída e a faturação total para esta data de saída
                printf("%s %.2f\n", faturacao_diaria.datas_saida[i], faturacao_data);
            }
        }
    }
}

/**
   Implementação do comando 'f' para processar operações relacionadas à faturação.
   Este comando pode ser usado para calcular a faturação total de um parque em um determinado dia ou para mostrar a faturação de um parque em uma data específica.
*/
void comando_f() {
    char nome_parque[MAXSIZE] = ""; // Variável para armazenar o nome do parque
    char data[TAMANHO_DATA] = "";   // Variável para armazenar a data
    char str[MAXSIZE];              // String para armazenar a entrada do usuário
    int rc;                         // Variável para armazenar o resultado do sscanf

    // Ler a entrada do usuário
    if (fgets(str, MAXSIZE, stdin) == NULL) {
        return; // Retorna se a entrada estiver vazia
    }

    int tem_aspas = 0;
    // Verificar se a entrada contém aspas
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '"') {
            tem_aspas = 1;
            break;
        }
    }

    // Processar a entrada com ou sem aspas
    if (tem_aspas) {
        // Ler a entrada formatada com aspas
        rc = sscanf(str, " \"%6000[^\"]\" %s", nome_parque, data);
        // Verificar se o parque existe
        if (!existe_parque(nome_parque)) {
            printf("%s: no such parking.\n", nome_parque);
            return;
        }   
        // Executar a função de acordo com a quantidade de argumentos lidos
        if (rc == 1) {
            calcula_faturacao_parque(nome_parque); // Calcular a faturação total do parque
        } else if (rc == 2) {
            mostra_faturacao(nome_parque, data); // Mostrar a faturação do parque para uma data específica
        }
    } else {
        // Ler a entrada formatada sem aspas
        rc = sscanf(str, "%s %s", nome_parque, data);
        // Verificar se o parque existe
        if (!existe_parque(nome_parque)) {
            printf("%s: no such parking.\n", nome_parque);
            return;
        }
        // Executar a função de acordo com a quantidade de argumentos lidos
        if (rc == 1) {
            calcula_faturacao_parque(nome_parque); // Calcular a faturação total do parque
        } else if (rc == 2) {
            mostra_faturacao(nome_parque, data); // Mostrar a faturação do parque para uma data específica
        }
    }
}


/**
   Valida um formato de hora no formato "hh:mm".
   @param horas String contendo a hora no formato "hh:mm" a ser validada.
   @return 1 se a hora for válida, 0 caso contrário.
*/
int valida_hora(char horas[]) {
    int hora, minuto;
    // Extrair hora e minuto da string utilizando sscanf
    sscanf(horas, "%d:%d", &hora, &minuto);
    
    // Verificar se a hora e o minuto estão dentro dos limites válidos
    if (hora < 0 || hora >= 24 || minuto < 0 || minuto > 59) {
        return 0; // Retorna 0 se a hora não for válida
    }
    return 1; // Retorna 1 se a hora for válida
}


/**
   Valida a entrada de um carro em um parque.
   @param nomeparque Nome do parque onde o carro entrará.
   @param matricula Matrícula do carro.
   @param data Data de entrada do carro.
   @param hora Hora de entrada do carro.
*/
void valida_carro(char nomeparque[], char matricula[], char data[], char hora[]) {
    // Verificar se o parque existe
   
    if (!existe_parque(nomeparque)) {
        printf("%s: no such parking.\n", nomeparque);
        return;
    }
    
    // Obter o índice do parque
    int index_parque = devolve_index_parque(nomeparque);
    
    // Verificar se o parque está cheio
    if (parques[index_parque].lugares_disponiveis == 0) {
        printf("%s: parking is full.\n", nomeparque);
        return;
    }
    
    // Verificar se a data e hora de entrada são válidas
    if (!valida_data(data, hora) || !valida_hora(hora)) {
        return;
    }
    
    // Verificar se a matrícula já está presente no sistema
    if (encontra_matricula(matricula) == 1) {
        printf("%s: invalid vehicle entry.\n", matricula);
        return;
    }
    
    // Verificar se a matrícula é válida
    if (!valida_matricula(matricula)) {
        return;
    }

    // Registra a entrada do carro no parque
    entra_carro(nomeparque, matricula, data, hora);
}



/**
   Encontra um lugar vazio no parque especificado pelo índice.
   @param indice_parque Índice do parque onde deseja-se encontrar um lugar vazio.
   @return O índice do lugar vazio encontrado ou -1 se nenhum estiver disponível.
*/
int encontra_lugar_vazio(int indice_parque) {
    // Iterar sobre os lugares do parque
    for (int j = 0; j < parques[indice_parque].capacidade; j++) {
        // Verificar se o lugar está vazio
        if (strcmp(parques[indice_parque].matriculas[j], "") == 0) {
            // Retorna o índice do lugar vazio encontrado
            return j;
        }
    }
    // Retorna -1 se nenhum lugar vazio for encontrado
    return -1;
}


/**
   Regista a entrada de um carro num parque.
   @param nomeparque Nome do parque onde o carro entrará.
   @param matricula Matrícula do carro.
   @param data Data de entrada do carro.
   @param hora Hora de entrada do carro.
*/
void entra_carro(char nomeparque[], char matricula[], char data[], char hora[]) {
    // Obter o índice do parque
    int i = devolve_index_parque(nomeparque);
    
    // Verificar se o parque existe
    if (i != -1) {
        int j;
        // Verificar se a matrícula já está presente no parque
        for (j = 0; j < parques[i].capacidade; j++) {
            if (strcmp(parques[i].matriculas[j], matricula) == 0) {
                return; // Retorna se a matrícula já estiver presente no parque
            }
        }
        
        // Verificar se há lugar vazio no parque
        if (encontra_lugar_vazio(i) == -1) {
            printf("erro\n"); // Mensagem de erro se não houver lugar vazio
            return;
        }
        
        // Encontra o índice do lugar vazio
        int indice_para_colocar = encontra_lugar_vazio(i);
        
        // Registra a matrícula, data e hora de entrada do carro no parque
        strcpy(parques[i].matriculas[indice_para_colocar], matricula);
        strcpy(parques[i].datas_entrada[indice_para_colocar], data);
        strcpy(parques[i].horas_entrada[indice_para_colocar], hora);
        parques[i].lugares_disponiveis--; // Decrementa o número de lugares disponíveis
        
        // Imprime o nome do parque e o número de lugares disponíveis após a entrada do carro
        printf("%s %d\n", parques[i].nome, parques[i].lugares_disponiveis);
        
        // Atualiza o relógio interno do sistema
        atualiza_relogio(data, hora);
        
        return;
    }
    
    // Se o parque não existir, imprime uma mensagem de erro
    printf("%s: no such parking.", nomeparque);
}

/**
   Adiciona uma nova entrada ao registo de movimentos.
   @param registro Ponteiro para a estrutura de registro de movimentos.
   @param matricula Matrícula do veículo.
   @param nome_parque Nome do parque onde o veículo entrou.
   @param data_entrada Data de entrada do veículo.
   @param hora_entrada Hora de entrada do veículo.
   @param data_saida Data de saída do veículo.
   @param hora_saida Hora de saída do veículo.
*/
void adicionar_entrada(Registro *registro, const char *matricula, const char *nome_parque, const char *data_entrada, const char *hora_entrada, const char *data_saida, const char *hora_saida) {
    // Alocar memória para a nova matrícula
    registro->matriculas = realloc(registro->matriculas, (registro->num_matriculas + 1) * sizeof(char *));
    if (registro->matriculas == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }
    registro->matriculas[registro->num_matriculas] = strdup(matricula);
    
    // Alocar memória para os demais dados da entrada
    registro->nomes_parque = realloc(registro->nomes_parque, (registro->num_registros + 1) * sizeof(char *));
    registro->datas_entrada = realloc(registro->datas_entrada, (registro->num_registros + 1) * sizeof(char *));
    registro->datas_saida = realloc(registro->datas_saida, (registro->num_registros + 1) * sizeof(char *));
    registro->horas_entrada = realloc(registro->horas_entrada, (registro->num_registros + 1) * sizeof(char *));
    registro->horas_saida = realloc(registro->horas_saida, (registro->num_registros + 1) * sizeof(char *));
    
    // Verificar se a alocação de memória foi bem-sucedida
    if (registro->nomes_parque && registro->datas_entrada && registro->datas_saida && registro->horas_entrada && registro->horas_saida) {
        // Alocar memória e copiar os dados para as novas entradas
        registro->nomes_parque[registro->num_registros] = strdup(nome_parque);
        registro->datas_entrada[registro->num_registros] = strdup(data_entrada);
        registro->datas_saida[registro->num_registros] = strdup(data_saida);
        registro->horas_entrada[registro->num_registros] = strdup(hora_entrada);
        registro->horas_saida[registro->num_registros] = strdup(hora_saida);
        
        // Incrementar o número de registros e de matrículas
        registro->num_registros++;
        registro->num_matriculas++;
    } else {
        printf("Erro ao alocar memória.\n");
    }
}

/**
   Lista todos os parques existentes, mostrando seus nomes, capacidade e número de lugares disponíveis.
*/
void listar_parques() {
    // Iterar sobre todos os parques
    for (int i = 0; i < MAXPARKS; i++) {
        // Verificar se o parque atual está ocupado
        if (strcmp(parques[i].nome, "") != 0) {
            // Imprimir o nome, capacidade e lugares disponíveis do parque
            printf("%s %d %d\n", parques[i].nome, parques[i].capacidade, parques[i].lugares_disponiveis);
        }
    }
    return;
}

/**
   Lista os parques restantes por ordem alfabética.
*/
void listar_parques_restantes() {
    // Ordenar os parques por ordem alfabética
    for (int i = 0; i < MAXPARKS; i++) {
        for (int j = i + 1; j < MAXPARKS; j++) {
            // Verificar se o nome do parque na posição i é maior que o nome do parque na posição j
            if (strcmp(parques[i].nome, parques[j].nome) > 0 && strcmp(parques[i].nome, "") != 0) {
                // Trocar os parques de posição se estiverem fora de ordem
                ParqueEstacionamento temp = parques[i];
                parques[i] = parques[j];
                parques[j] = temp;
            }
        }
    }
    
    // Imprimir os parques ordenados
    for (int i = 0; i < MAXPARKS; i++) {
        if (strcmp(parques[i].nome, "") != 0) {
            printf("%s\n", parques[i].nome);
        }
    }
}


/**
   Arredonda um número para um determinado número de casas decimais.
   @param numero O número a ser arredondado.
   @param casas_decimais O número de casas decimais desejado.
   @return O número arredondado.
*/
float arredonda(float numero, int casas_decimais) {
    // Calcula o fator de multiplicação
    float fator = 1;
    for (int i = 0; i < casas_decimais; i++) {
        fator *= 10;
    }
    // Multiplica o número pelo fator de multiplicação
    float numero_multiplicado = numero * fator;
    // Obtém a parte inteira e a parte decimal
    int parte_inteira = (int)numero_multiplicado;
    float parte_decimal = numero_multiplicado - parte_inteira;
    // Arredonda para cima se a parte decimal for maior ou igual a 0.5
    if (parte_decimal >= 0.5) {
        parte_inteira++;
    }
    // Calcula o número arredondado e retorna
    float numero_arredondado = parte_inteira / fator;
    return numero_arredondado;
}

/**
   Calcula o número de períodos de 15 minutos em um determinado número de minutos.
   @param minutos O número de minutos.
   @return O número de períodos de 15 minutos.
*/
int periodos_de_15_minutos(int minutos) {
    int n_periodos = 0;
    // Calcula o número de períodos de 15 minutos
    while (minutos > 15) {
        minutos -= 15;
        n_periodos += 1;
    }
    n_periodos += 1; // Incrementa um período para o restante dos minutos
    return n_periodos;
}

/**
   Calcula a faturação para um determinado período de estacionamento em um parque.
   @param index_parque Índice do parque.
   @param data_entrada Data de entrada do veículo.
   @param hora_entrada Hora de entrada do veículo.
   @param data_saida Data de saída do veículo.
   @param hora_saida Hora de saída do veículo.
   @return A faturação calculada.
*/
float calcula_faturacao(int index_parque, char data_entrada[],char hora_entrada[], char data_saida[],char hora_saida[]) {
    float faturacao = 0;
    // Obter os valores de faturação do parque
    float valor15 = parques[index_parque].valor15;
    float valor15_apos1hora = parques[index_parque].valor15_apos1hora;
    float valormax_diario = parques[index_parque].valormax_diario;
    
    // Calcular a diferença de tempo em minutos
    int diferenca_minutos = calcula_diferenca_tempo(data_entrada, hora_entrada, data_saida, hora_saida);
    // Verificar se a diferença é zero
    if (!diferenca_minutos) return 0;
    // Verificar se a diferença é maior que um dia
    if (diferenca_minutos > 1440) {
        faturacao += valormax_diario;
        diferenca_minutos -= 1440;
        while (diferenca_minutos > 1440) {
            faturacao += valormax_diario;
            diferenca_minutos -= 1440;
        }
    }
    // Entrar no cálculo diário
    if (diferenca_minutos == 60) {
        faturacao += valor15 * 4;
        return arredonda(faturacao, 2);
    }
    if (diferenca_minutos > 60) {
        float custo_diario_atual = 0;
        int n_periodos_15 = 0;
        custo_diario_atual += valor15 * 4;
        diferenca_minutos -= 60;
        n_periodos_15 = periodos_de_15_minutos(diferenca_minutos);
        custo_diario_atual += valor15_apos1hora * n_periodos_15;
        // Verificar se o custo diário ultrapassa o valor máximo diário
        if (custo_diario_atual > valormax_diario) {
            faturacao += valormax_diario;
            return arredonda(faturacao, 2);
        } else {
            faturacao += custo_diario_atual;
            return arredonda(faturacao, 2);
        }
    }
    // Calcular a faturação para o restante dos minutos
    while (diferenca_minutos > 15) {
        faturacao += valor15;
        diferenca_minutos -= 15;
    }
    faturacao += valor15; // Adicionar o último período de 15 minutos
    return arredonda(faturacao, 2);
}
int matricula_existe_no_parque(char nomeparque[],char matricula[]){
int i= devolve_index_parque(nomeparque);
//printf("procurando no parque %s\n",parques[i].nome);

int j;
for(j=0; j<parques[i].capacidade; j++){
    //printf("matricula numero: %i -> %s \n\n",j,parques[i].matriculas[j]);
    if (strcmp(parques[i].matriculas[j], matricula) == 0) {
        return j;
    }
}
//matricula nao existe no parque
return -1;
}


void remove_carro(int index_parque, int index_matricula, char data_saida[],char hora_saida[]) {
    int hora_entrada, minuto_entrada;
    int hora_saida_int, minuto_saida;
    int i =index_parque;
    int j =index_matricula;
    atualiza_relogio(data_saida,hora_saida);
    strcpy(parques[i].datas_saida, data_saida);
    strcpy(parques[i].horas_saida, hora_saida);
    
    strcpy(parques[i].matriculas[j],"");
    strcpy(parques[i].datas_entrada[j],"");
    strcpy(parques[i].horas_entrada[j],"");
    
    parques[i].lugares_disponiveis++;
    sscanf(parques[i].horas_entrada[j], "%d:%d", &hora_entrada, &minuto_entrada);
    sscanf(hora_saida, "%d:%d", &hora_saida_int, &minuto_saida);
                    
    return;
    
    
}

void adiciona_faturacao(FaturacaoDiaria *faturacao_diaria, const char *matricula, const char *nome_parque, const char *data_saida, const char *hora_entrada,const char *hora_saida,const char *faturacao) {
    // Alocar memória para a nova matrícula
    faturacao_diaria->matriculas = realloc(faturacao_diaria->matriculas, (faturacao_diaria->num_matriculas + 1) * sizeof(char *));
    if (faturacao_diaria->matriculas == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }
    faturacao_diaria->matriculas[faturacao_diaria->num_matriculas] = strdup(matricula);
    faturacao_diaria->horas_entrada = realloc(faturacao_diaria->horas_entrada, (faturacao_diaria->num_registros + 1) * sizeof(char *));
    faturacao_diaria->nomes_parque = realloc(faturacao_diaria->nomes_parque, (faturacao_diaria->num_registros + 1) * sizeof(char *));
    faturacao_diaria->datas_saida = realloc(faturacao_diaria->datas_saida, (faturacao_diaria->num_registros + 1) * sizeof(char *));
    faturacao_diaria->horas_saida = realloc(faturacao_diaria->horas_saida, (faturacao_diaria->num_registros + 1) * sizeof(char *));
    faturacao_diaria->faturacao = realloc(faturacao_diaria->faturacao, (faturacao_diaria->num_registros + 1) * sizeof(char *));
    
if (faturacao_diaria->nomes_parque && faturacao_diaria->datas_saida && faturacao_diaria->horas_saida && faturacao_diaria->faturacao) {
    // Alocar memória e copiar os dados
    faturacao_diaria->faturacao[faturacao_diaria->num_registros] = strdup(faturacao);
    faturacao_diaria->nomes_parque[faturacao_diaria->num_registros] = strdup(nome_parque);
    faturacao_diaria->datas_saida[faturacao_diaria->num_registros] = strdup(data_saida);
    faturacao_diaria->horas_saida[faturacao_diaria->num_registros] = strdup(hora_saida);
    faturacao_diaria->horas_entrada[faturacao_diaria->num_registros] = strdup(hora_entrada);
        
    // Incrementar o número de registros e de matrículas
    faturacao_diaria->num_registros++;
    faturacao_diaria->num_matriculas++;
} else {
    printf("Erro ao alocar memória.\n");
}
}


void verifica_remove_carro(char nomeparque[], char matricula[], char data_saida[], char hora_saida[]) {
    if (!existe_parque(nomeparque)) {
        printf("%s: no such parking.\n", nomeparque);
        return;
    }
    if(!valida_matricula(matricula)){
        return;
    }

    int j = matricula_existe_no_parque(nomeparque, matricula);
    if (j == -1) {
        
        printf("%s: invalid vehicle exit.\n", matricula);
        // a matricula nao existe no parque
        return;
    }

    char faturacao_str[100];
    float faturacao = 0;
    int i;
    i = devolve_index_parque(nomeparque);

    int hora_entrada, minuto_entrada;
    int hora_saida_int, minuto_saida;      

    sscanf(parques[i].horas_entrada[j], "%d:%d", &hora_entrada, &minuto_entrada);
    sscanf(hora_saida, "%d:%d", &hora_saida_int, &minuto_saida);

    if (!valida_data(data_saida, hora_saida)) {
        return;
    }

    if (hora_entrada >= 24 || minuto_entrada >= 60 || hora_saida_int >= 24 || minuto_saida >= 60) {
        printf("invalid date.\n");
        return;
    }

    faturacao = calcula_faturacao(i, parques[i].datas_entrada[j], parques[i].horas_entrada[j], data_saida, hora_saida);

    //adiciona ao registo este movimento

    sprintf(faturacao_str, "%.2f", faturacao);
    adicionar_entrada(&registro, matricula, nomeparque, parques[i].datas_entrada[j], parques[i].horas_entrada[j], data_saida, hora_saida);
    adiciona_faturacao(&faturacao_diaria, matricula, nomeparque, data_saida, parques[i].horas_entrada[j], hora_saida, faturacao_str);


    printf("%s %s %02d:%02d %s %02d:%02d %.2f\n", matricula, parques[i].datas_entrada[j], hora_entrada, minuto_entrada, data_saida, hora_saida_int, minuto_saida, faturacao);
    remove_carro(i, j, data_saida, hora_saida);
    return;
}


int compara_datas(const char *data1,const char *horas1, const char *data2,const char*horas2) {
    // Extrai os elementos de data1
    int dia1, mes1, ano1, hora1, minuto1;
    sscanf(data1, "%d-%d-%d", &dia1, &mes1, &ano1);
    sscanf(horas1,"%d:%d",&hora1,&minuto1);
    // Extrai os elementos de data2
    int dia2, mes2, ano2, hora2, minuto2;
    sscanf(data2, "%d-%d-%d", &dia2, &mes2, &ano2);
    sscanf(horas2,"%d:%d",&hora2,&minuto2);
    
    // Compara ano
    if (ano1 > ano2){
        return 0;
    }
        
    else if (ano1 < ano2)
        return 1;

    // Compara mês
    if (mes1 > mes2)
        return 0;
    else if (mes1 < mes2)
        return 1;

    // Compara dia
    if (dia1 > dia2)
        return 0;
    else if (dia1 < dia2)
        return 1;
    
    if (hora1 > hora2)
        return 2;
    else if (hora1 < hora2)
        return 3;

    // Compara minuto
    if (minuto1 > minuto2)
        return 2;
    else if (minuto1 < minuto2)
        return 3;

    // Se as datas forem iguais
    return 1;
    
}
int segundos_desde_epoca(const struct DataHora* dt) {
    const size_t dias_no_mes[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int ano = 1970;
    int mes = 1;
    int dia = 1;
    int total = 0;

    while (ano < dt->ano) {
            total += 365 * SEGUNDOS_EM_DIA;

        ano++;
    }

    while (mes < dt->mes) {
        total += dias_no_mes[mes] * SEGUNDOS_EM_DIA;
        mes++;
    }

    total += (dt->dia - dia) * SEGUNDOS_EM_DIA;
    total += 60 * 60 * dt->hora; // horas para segundos
    total += 60 * dt->minuto;    // minutos para segundos
    return total;
}

int calcula_diferenca_tempo(char data_in[], char hora_in[], char data_out[], char hora_out[]) {
    struct DataHora dt1, dt2;
    sscanf(data_in, "%d-%d-%d", &dt1.dia, &dt1.mes, &dt1.ano);
    sscanf(hora_in, "%d:%d", &dt1.hora, &dt1.minuto);
    sscanf(data_out, "%d-%d-%d", &dt2.dia, &dt2.mes, &dt2.ano);
    sscanf(hora_out, "%d:%d", &dt2.hora, &dt2.minuto);

    // Converter data e hora para segundos desde a época
    int segundos1 = segundos_desde_epoca(&dt1);
    int segundos2 = segundos_desde_epoca(&dt2);

    int diferenca_em_segundos = abs(segundos1 - segundos2);
    int diferenca_em_minutos = diferenca_em_segundos / 60;
    return diferenca_em_minutos;
}


int valida_data(const char *data, const char *hora) {
    

    // Verifica se a data e hora mais recentes não estão vazias
    if (strcmp(relogio.data_atual, "") != 0 && strcmp(relogio.hora_atual, "") != 0) {
        // Compare as datas e horas
        int resultado_comparacao = compara_datas(relogio.data_atual, relogio.hora_atual, data, hora);
        if (resultado_comparacao == 0 || resultado_comparacao == 2) {
            printf("invalid date.\n");
            return 0;
        }
    }

    // Verifica a estrutura da data
    if (data[2] != '-' || data[5] != '-') {
        printf("invalid date.\n");
        return 0;
    }

    // Verifica se cada parte da data é um número válido
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5)
            continue; // Ignora os caracteres de separação
        if (!isdigit(data[i])) {
            printf("invalid date.\n");
            return 0;
        }
    }

    // Converte dia, mês e ano para inteiros
    int dia = atoi(data);
    int mes = atoi(data + 3);
    int ano = atoi(data + 6);

    // Verifica se o dia está dentro do intervalo correto
    if (dia < 1 || dia > 31) {
        printf("invalid date.\n");
        return 0;
    }

    // Verifica se o mês está dentro do intervalo correto
    if (mes < 1 || mes > 12) {
        printf("invalid date.\n");
        return 0;
    }
    if (mes == 2) {
        if (dia > 28) {
            printf("invalid date.\n");
            return 0;
        }
    }
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        if (dia > 30) {
            printf("invalid date.\n");
            return 0;
        }
    }

    // Verifica se o ano está dentro do intervalo correto
    if (ano < 1000 || ano > 9999) {
        printf("invalid date.\n");
        return 0;
    }
    
    return 1;
}


int encontra_matricula(char matricula[]) {
    for (int i = 0; i < MAXPARKS; i++) {
        for (int j = 0; j < parques[i].capacidade; j++) {
            if (strcmp(matricula, parques[i].matriculas[j]) == 0) {
                adicionar_entrada(&registro, matricula, parques[i].nome, parques[i].datas_entrada[j],parques[i].horas_entrada[j], "","");
                return 1;
                
                
            }
        }
    }
    return 0;
    
}
int existe_registo_matricula(const Registro *registro, const char *matricula) {
    int i;
    for (i = 0; i < registro->num_matriculas; i++) {
        
        if (strcmp(registro->matriculas[i], matricula) == 0) {
            return 1;
        }
    }
    return 0;
}

int valida_matricula(const char *matricula) {
    int i;
    int tam_matricula = strlen(matricula);

    // Verifica se o tamanho da matrícula é válido
    if (tam_matricula != 8) {
        printf("%s: invalid licence plate.\n", matricula);
        return 0;
    }

    // Verifica a estrutura da matrícula
    if (matricula[2] != '-' || matricula[5] != '-') {
        printf("%s: invalid licence plate.\n", matricula);
        return 0;
    }

    
    
    // Verifica se há pelo menos um par de letras e um par de dígitos
    int letras = 0;
    for (i = 0; i < 8; i++) {
        if (isalpha(matricula[i]))
            letras++;
        
    }
    if (letras ==6) {
        printf("%s: invalid licence plate.\n", matricula);
        return 0;
    }
    if(isalpha(matricula[0])!=isalpha(matricula[1]) || isalpha(matricula[3])!=isalpha(matricula[4]) || isalpha(matricula[6])!=isalpha(matricula[7])){
        printf("%s: invalid licence plate.\n", matricula);
        return 0;
    }
    return 1;
}

int detectaAspas(const char *string) {
    while (*string != '\0') {
        if (*string == '\"') {
            return 1; // Retorna 1 se encontrar uma aspa
        }
        string++;
    }
    return 0; // Retorna 0 se não encontrar aspas
}

void removerAspas(char *string) {
    int i, j;
    int len = strlen(string);

    // Percorre a string
    for (i = 0, j = 0; i < len; i++) {
        // Se o caracter atual não for uma aspa, copia-o para a nova posição
        if (string[i] != '\"') {
            string[j++] = string[i];
        }
    }

    // Adiciona o terminador nulo ao final da nova string
    string[j] = '\0';
}


void comando_r(char comando[]) {

    if (strcmp(comando, "r") == 0) {
        char nextChar;
        do {
            nextChar = getchar();
        } while (isspace(nextChar)); // Ignorar espaços em branco

        if (nextChar == '\n') {
            return;
        } else {
            ungetc(nextChar, stdin);
            char str[MAXSIZE * 5]; // Aumentando o tamanho para lidar com a entrada completa
            if (fgets(str, MAXSIZE * 5, stdin) == NULL) {
                return;
            }
            
            
            // Remover nova linha, se presente
            str[strcspn(str, "\n")] = '\0';

            if (detectaAspas(str)) {
                removerAspas(str);
            }
            if (!existe_parque(str)) {
                printf("%s: no such parking.\n", str);
                return;
            }

            remover_parque(str);
        }
    }
}

void comando_p(char comando[]) {
    
    if (strcmp(comando, "p") == 0) {
        char nextChar = getchar();
        if (nextChar == '\n') {
            listar_parques();
        } else {
            ungetc(nextChar, stdin);
            char str[MAXSIZE * 5]; // Aumentando o tamanho para lidar com a entrada completa
            if (fgets(str, MAXSIZE * 5, stdin) == NULL) {
                return;
            }
            char nome[MAXSIZE];
            float capacidade;
            float valor15, valor15_apos1hora, valormax_diario;
            if (sscanf(str, "%6000s %f %f %f %f", nome, &capacidade, &valor15, &valor15_apos1hora, &valormax_diario) != 5) {
            sscanf(str, " \"%6000[^\"]\" %f %f %f %f", nome, &capacidade, &valor15, &valor15_apos1hora, &valormax_diario);
            }
            

            if (detectaAspas(nome)) {
                removerAspas(nome);
            }
            if (existe_parque(nome)) {
                printf("%s: parking already exists.\n",nome);
                return;
            }
            // Verifica se a capacidade tem casas decimais
            if (capacidade != (int)capacidade) {
                printf("%.2f: invalid capacity.\n", capacidade);
                return;
            }
            int capacidade_int = (int)capacidade;
            if (capacidade <= 0) {
                printf("%i: invalid capacity.\n",capacidade_int);
                return;
            }
            
            
            criarParque(nome, (int)capacidade, valor15, valor15_apos1hora, valormax_diario);
        }
    }
}
void comando_s(char comando[]){

    if (strcmp(comando, "s") == 0) {
        char nextChar = getchar();
        if (nextChar == '\n') {
            printf("Missing parameters\n");
            return;
        } else {
            ungetc(nextChar, stdin);
            char str[MAXSIZE * 5]; // Aumentando o tamanho para lidar com a entrada completa
            if (fgets(str, MAXSIZE * 5, stdin) == NULL) {
                return;
            }

            char nomeparque[MAXSIZE];
            char matricula[TAMANHO_MATRICULA];
            char data[TAMANHO_DATA];
            char hora[TAMANHO_HORA];

            if (sscanf(str, "%6000s %8s %10s %5s", nomeparque, matricula, data, hora) != 3) {
            sscanf(str, " \"%6000[^\"]\" %8s %10s %5s", nomeparque, matricula, data, hora);
            }
            
            verifica_remove_carro(nomeparque, matricula, data, hora);
        }
    }
}
void trocar_elementos(char **arr, int i, int j) {
    char *temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// Função para ordenar os dados pelo nome do parque
void bubble_sort(Registro *registro) {
    for (int i = 0; i < registro->num_registros - 1; i++) {
        for (int j = 0; j < registro->num_registros - i - 1; j++) {
            if (strcmp(registro->nomes_parque[j], registro->nomes_parque[j + 1]) > 0) {
                trocar_elementos(registro->nomes_parque, j, j + 1);
                trocar_elementos(registro->datas_entrada, j, j + 1);
                trocar_elementos(registro->horas_entrada, j, j + 1);
                trocar_elementos(registro->datas_saida, j, j + 1);
                trocar_elementos(registro->horas_saida, j, j + 1);
                trocar_elementos(registro->matriculas, j, j + 1);
            }
        }
    }
}
void bubble_sort2(FaturacaoDiaria *faturacao_diaria) {
    for (int i = 0; i < faturacao_diaria->num_registros - 1; i++) {
        for (int j = 0; j < faturacao_diaria->num_registros - i - 1; j++) {
            int hora_j, minuto_j;
            sscanf(faturacao_diaria->horas_saida[j], "%d:%d", &hora_j, &minuto_j);

            int hora_j1, minuto_j1;
            sscanf(faturacao_diaria->horas_saida[j + 1], "%d:%d", &hora_j1, &minuto_j1);

            int tempo_total_j = hora_j * 60 + minuto_j;
            int tempo_total_j1 = hora_j1 * 60 + minuto_j1;

            if (tempo_total_j >= tempo_total_j1) {
                trocar_elementos(faturacao_diaria->nomes_parque, j, j + 1);
                trocar_elementos(faturacao_diaria->datas_saida, j, j + 1);
                trocar_elementos(faturacao_diaria->horas_saida, j, j + 1);
                trocar_elementos(faturacao_diaria->matriculas, j, j + 1);
                trocar_elementos(faturacao_diaria->faturacao, j, j + 1);
            }
        }
    }
}

void mostra_faturacao(char nome_parque[],char data[]) {
    
    bubble_sort2(&faturacao_diaria);
    int hora_saida, minuto_saida;
    
    for (int i = 0; i < faturacao_diaria.num_registros; i++) {
       
        if (strcmp(faturacao_diaria.datas_saida[i], data) == 0 && strcmp(faturacao_diaria.nomes_parque[i],nome_parque)==0) {
            sscanf(faturacao_diaria.horas_saida[i],"%d:%d",&hora_saida,&minuto_saida);
            
        printf("%s %02d:%02d %s\n",faturacao_diaria.matriculas[i],hora_saida,minuto_saida, faturacao_diaria.faturacao[i]);
               
    }
    }
    return;
    
}

void imprime_entradas_registo(char matricula[]) {
    bubble_sort(&registro);
    int hora_entrada, minuto_entrada;
    int hora_saida ,minuto_saida;
    int imprimiu_algo = 0; // Variável de controlo para verificar se algo foi impresso
    
    for (int i = 0; i < registro.num_registros; i++) {
        if (strcmp(registro.matriculas[i], matricula) == 0) {
            sscanf(registro.horas_entrada[i], "%d:%d", &hora_entrada, &minuto_entrada);
            sscanf(registro.horas_saida[i], "%d:%d", &hora_saida, &minuto_saida);
            
            if (strcmp(registro.datas_saida[i], "") == 0) {
                printf("%s %s %02d:%02d\n", registro.nomes_parque[i], registro.datas_entrada[i], hora_entrada, minuto_entrada);
                imprimiu_algo = 1; // Indica que algo foi impresso
            } else {
                printf("%s %s %02d:%02d %s %02d:%02d\n", registro.nomes_parque[i], registro.datas_entrada[i], hora_entrada, minuto_entrada, registro.datas_saida[i], hora_saida, minuto_saida);
                imprimiu_algo = 1; // Indica que algo foi impresso
            }
        }
    }
    
    // Verificar se nada foi impresso
    if (!imprimiu_algo) {
        printf("%s: no entries found in any parking.\n", matricula);
    }
}
void comando_v() {
    char comando[MAXSIZE];
    scanf("%6000s", comando);
    if(!valida_matricula(comando))return;
    encontra_matricula(comando);
    if(!existe_registo_matricula(&registro,comando)){
        printf("%s: no entries found in any parking.\n",comando);
        return;
    }
    imprime_entradas_registo(comando);
    
}


// Função para liberar a memória alocada para todos os parques de estacionamento


int eh_hora_valida(const char hora[]) {
    // Verifica se a string de hora é nula
    if (hora == NULL) {
        printf("invalid date.\n");
        return 0;
    }

   if (strlen(hora) != 5 && strlen(hora)!=4) {
        
        return 0;
    }

    // Verifica se todos os caracteres são dígitos numéricos e se os dois pontos estão na posição correta
    for (int i = 0; i < 5; i++) {
        
            if (hora[i] == ':') {
                break;

            }
            
    }

    // Separar a hora e os minutos
    int hora_int, minuto;
    sscanf(hora, "%d:%d", &hora_int, &minuto);

    // Verifica se a hora e os minutos estão dentro dos limites válidos
    if (hora_int < 0 || hora_int > 23 || minuto < 0 || minuto > 59) {
        return 0;
    }

    return 1; // Retorna 1 se a hora for válida
}


double montante_pago_por_carro(char matricula[]){
    
    double montante =0;
    for (int i = 0; i < faturacao_diaria.num_registros; i++) {
        
        if (strcmp(faturacao_diaria.matriculas[i], matricula) == 0) {
            montante += atof(faturacao_diaria.faturacao[i]);
          
    }
    
    }
    return montante;
}


void comando_u(char comando[]) {


    if (strcmp(comando, "u") == 0) {
        char nextChar = getchar();
        if (nextChar == '\n') {
            
            return;
            
        } else {
            
            ungetc(nextChar, stdin);
            char str[MAXSIZE * 5]; 
            
            if (fgets(str, MAXSIZE * 5, stdin) == NULL) {
                
                return;
            }
            
            char matricula[TAMANHO_MATRICULA];
            
            sscanf(str, " %8s", matricula);
            if(!valida_matricula(matricula)){
                return;
            }
            
            printf("%0.2f\n",montante_pago_por_carro(matricula));
        }
    }
}



void comando_e(char comando[]) {


    if (strcmp(comando, "e") == 0) {
        char nextChar = getchar();
        if (nextChar == '\n') {
            
            return;
            
        } else {
            
            ungetc(nextChar, stdin);
            char str[MAXSIZE * 5]; // Aumentando o tamanho para lidar com a entrada completa
            
            if (fgets(str, MAXSIZE * 5, stdin) == NULL) {
                
                return;
            }
            char nomeparque[MAXSIZE];
            char matricula[TAMANHO_MATRICULA];
            char data[TAMANHO_DATA];
            char hora[TAMANHO_HORA];
            if (sscanf(str, "%6000s %8s %10s %5s", nomeparque, matricula, data, hora) != 3) {
            sscanf(str, " \"%6000[^\"]\" %8s %10s %5s", nomeparque, matricula, data, hora);
            }
            

            valida_carro(nomeparque, matricula, data, hora);
        }
    }
}

void comando_c(char comando[]){
    printf("[P] -> Criar Parque (nome, int capacidade, valor15, valor15_apos1hora, valormax_diario)\n");
    printf("[E] -> Entrar carro (nomeparque, matricula, data, hora)\n");
}


void check_input(char comando[]) {
    
    if (strcmp(comando, "p") == 0) {
        comando_p(comando);
    } else if (strcmp(comando, "e") == 0) {
        comando_e(comando);
    } else if (strcmp(comando, "v") == 0) {
        comando_v();
    
    } else if (strcmp(comando, "s") == 0){
        comando_s(comando);
    }
    else if (strcmp(comando, "f") == 0){
        comando_f();
    }
    else if (strcmp(comando, "r") == 0){
        comando_r(comando);
    }
    else if (strcmp(comando, "u") == 0){
        comando_u(comando);
    }else if(strcmp(comando ,"c")==0){
        comando_c(comando);
    }
}

int main() {
    inicializar_relogio();
    inicializar_sistema();
    while (1) {
        char comando[MAXSIZE];
        scanf("%100s", comando);
        if (strcmp(comando, "q") == 0){
            
            break;
        }
    
        check_input(comando);

    }

    return 0;
}
