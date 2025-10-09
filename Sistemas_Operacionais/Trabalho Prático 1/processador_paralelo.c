#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32 // Se estiver compilando no Windows, adicionar a biblioteca windows.h
#include <windows.h>
#endif

// Definições necessárias para que a biblioteca stb_image implemente suas funções
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/* A função pthread_create permite apenas a passagem de um argumento vazio,
mas podemos usar uma struct para passar as informações necessárias*/
typedef struct {
    int inicio;
    int fim;
    int canais;
    unsigned char *img_original;
    unsigned char *img_final;
} argumentos_thread;

// Função que será executada pelas threads
void *filtro_escala_cinza(void *args) {
    argumentos_thread *thread = (argumentos_thread *)args;
    int i;
    for (i=thread->inicio;i<thread->fim;i+=thread->canais) {
        // Fórmula de luminância
        unsigned char valor_cinza = (0.299*thread->img_original[i] + 0.587*thread->img_original[i+1] + 0.114*thread->img_original[i+2]);
        // Define todos os canais do pixel de saída com o mesmo valor de cinza
        thread->img_final[i] = valor_cinza; // R
        thread->img_final[i+1] = valor_cinza; // G
        thread->img_final[i+2] = valor_cinza; // B

        // Copia o canal alfa/transparente, se existir
        if (thread->canais == 4)
            thread->img_final[i+3]=thread->img_original[i+3];
    }
    pthread_exit(NULL); //Saindo da thread
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <imagem_de_entrada> <imagem_de_saida> <quantidade de threads>\n", argv[0]);
        return 1;
    }
    char *caminho_entrada = argv[1]; //Imagem a ser processada
    char *caminho_saida = argv[2]; // Imagem final
    int num_threads = atoi(argv[3]); // Convertendo o char de número de threads para um inteiro (ASCII to integer)

    if (num_threads<1)
        num_threads=1; // Executa com no mínimo uma thread
    int largura, altura, canais; // Variáveis para armazenar as dimensões e canais da imagem

    printf("\nProcessando a imagem '%s' com %d thread(s)...\n", caminho_entrada, num_threads);

    // Início da medição do tempo de execução
    #ifdef _WIN32
        // Código para Windows
        LARGE_INTEGER frequency;
        LARGE_INTEGER inicio, fim;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&inicio);
    #else
        // Código para Linux/POSIX
        struct timespec inicio, fim;
        clock_gettime(CLOCK_MONOTONIC, &inicio);
    #endif

    unsigned char *img = stbi_load(caminho_entrada, &largura, &altura, &canais, 0); // Carrega a imagem pela biblioteca stb_image
    if (img == NULL) {
        fprintf(stderr, "Erro ao carregar a imagem. Verifique o caminho.\n");
        return 1;
    }

    int tamanho_img = (int)largura * altura * canais; // Calcula o tamanho total da imagem em bytes
    unsigned char *img_cinza = malloc(tamanho_img); // Aloca memória para a imagem em preto e branco
    if (img_cinza == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a imagem em escala de cinza.\n");
        stbi_image_free(img);
        return 1;
    }

    // Alocando memória para as threads
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Erro ao alocar memória para as threads.\n");
        stbi_image_free(img);
        free(img_cinza);
        return 1;
    }
    // Alocando memória para os argumentos das threads
    argumentos_thread *thread_args = malloc(num_threads * sizeof(argumentos_thread));
    if (thread_args == NULL) {
        fprintf(stderr, "Erro ao alocar memória para os argumentos das threads.\n");
        stbi_image_free(img);
        free(img_cinza);
        return 1;
    }

    // Controlando as linhas que cada thread irá processar
    int linhas_por_thread=altura/num_threads;
    int resto_linhas=altura%num_threads;
    int linha_atual=0;
    int linhas_da_thread_atual;

    for (int i=0; i<num_threads; i++) {
        if(i<resto_linhas) // As i primeiras threads processam uma linha a mais, até que o resto "acabe"
            linhas_da_thread_atual=linhas_por_thread + 1;
        else
            linhas_da_thread_atual=linhas_por_thread;
    
        thread_args[i].inicio = linha_atual*largura*canais; // Cálculo do byte inicial
        thread_args[i].fim = thread_args[i].inicio + (linhas_da_thread_atual*largura*canais); // Cálculo do byte final
        thread_args[i].canais = canais;
        thread_args[i].img_original = img; // Endereço da imagem original
        thread_args[i].img_final = img_cinza; // Endereço da imagem em escala de cinza
        
        // Criando a thread para executar a função de conversão
        if (pthread_create(&threads[i], NULL, filtro_escala_cinza, &thread_args[i])) {
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return 1;
        }
        linha_atual+=linhas_da_thread_atual; // Atualiza para a próxima thread
    }

    // Aguarda todas as threads terminarem
    for (int i=0; i<num_threads; i++) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Erro ao unir thread %d\n", i);
            return 1;
        }
    }
   
    // Salva a imagem processada no disco
    stbi_write_jpg(caminho_saida, largura, altura, canais, img_cinza, 100);

    // Marcando o tempo final de execução e o tempo total
    double tempo_execucao_ms;
    #ifdef _WIN32
        // Código para Windows
        QueryPerformanceCounter(&fim);
        tempo_execucao_ms = (double)(fim.QuadPart - inicio.QuadPart) * 1000.0 / (double)frequency.QuadPart;
    #else
        // Código para Linux/POSIX
        clock_gettime(CLOCK_MONOTONIC, &fim);
        tempo_execucao_ms = (fim.tv_sec - inicio.tv_sec) * 1000.0;
        tempo_execucao_ms += (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
    #endif

    // Libera a memória alocada
    stbi_image_free(img);
    free(img_cinza);
    free(threads);
    free(thread_args);
   
    printf("\nImagem convertida com sucesso e salva em '%s'\n", caminho_saida);
    printf("\nTempo de execucao (multithreading): %.2f ms\n\n", tempo_execucao_ms);
    printf("- - - - - - - - - - - - - - - - -\n\n");
    return 0;
}