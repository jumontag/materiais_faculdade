#include <stdio.h>
#define tamanho 1000
int comparacoes=0, trocas=0;

void quick_sort(int vetor[], int baixo, int alto) {
    if (baixo < alto) {
        int pivo = vetor[alto]; // Escolhe o último elemento como pivô
        int i = baixo-1;

        for (int j = baixo; j < alto; j++) {
            comparacoes++;
            if (vetor[j] < pivo) {
                trocas++;
                i++;
                int temp = vetor[i];
                vetor[i] = vetor[j];
                vetor[j] = temp;
            }
        }
        // Depois de ordenar o vetor, o pivô é posto na posição correta
        trocas++;
        int temp = vetor[i + 1];
        vetor[i + 1] = vetor[alto];
        vetor[alto] = temp;

        int indice_pivo = i+1;

        quick_sort(vetor, baixo, indice_pivo - 1);
        quick_sort(vetor, indice_pivo + 1, alto);
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

    quick_sort(vetor, 0, tamanho-1); // Ordenação

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