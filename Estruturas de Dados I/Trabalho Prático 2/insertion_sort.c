#include <stdio.h>
#define tamanho 1000
int comparacoes=0, trocas=0;

void insertion_sort(int vetor[]) {
    for (int i=1; i<tamanho; i++) {
        int chave=vetor[i];
        int j=i-1;
        while (j>=0) {
            comparacoes++;
            if (vetor[j]>chave) {
                trocas++;
                vetor[j+1]=vetor[j]; // Desloca o maior para a direita
                j--;
            }
            else
                break;
        }
        vetor[j+1]=chave; // Coloca o número no lugar certo
    }
}
// Permitindo a passagem do vetor txt pela linha de comando
int main (int argc, char *argv[]) {
    // Precisamos do nome do programa e do nome do arquivo que contém o vetor
    if (argc != 2) {
        printf("\nUso: %s <arquivo .txt contendo o vetor> \n", argv[0]);
        return 1;
    }
    int vetor[tamanho];
    FILE *arquivo_entrada = fopen(argv[1], "r"); // Lê o arquivo

    if (arquivo_entrada == NULL) {
        printf("\nErro ao abrir o arquivo '%s'\n", argv[1]);
        return 1; // Encerra o programa com erro
    }
    // lê número e vírgula
    for (int i=0; i<tamanho; i++) {
        /* fscanf lê um inteiro e armazena em vetor[i]
        "%d," diz para ler um inteiro e "pular" a vírgula*/
        if (fscanf(arquivo_entrada, "%d,", &vetor[i]) != 1) {
            // Se fscanf não conseguir ler um item, há um erro no arquivo
            printf("\nErro ao ler o item %d do arquivo.\n", i);
            break; 
        }
    }
    fclose(arquivo_entrada);

    insertion_sort(vetor); // Ordenação

    FILE *arquivo_saida = fopen("vetor_ordenado.txt", "w"); // Cria o arquivo de saída
    if (arquivo_saida == NULL) {
        printf("\nErro ao criar o arquivo 'vetor_ordenado.txt'\n");
        return 1;
    }
    for (int i=0; i<tamanho-1; i++) {
        // Adicionando uma vírgula depois de cada número, exceto no último
        fprintf(arquivo_saida, "%d,", vetor[i]);
    }
    fprintf(arquivo_saida, "%d", vetor[tamanho-1]); // Último número fica sem vírgula
    fclose(arquivo_saida);

    printf("\nVetor ordenado e salvo em 'vetor_ordenado.txt'.\n");
    printf("\nQuantidade de comparações: %d\n", comparacoes);
    printf("Quantidade de trocas: %d\n", trocas);
    return 0;
}