# Trabalho prático - Programação multithread

O presente trabalho teve como objetivo aplicar os conceitos de programação concorrente em um programa que transforma imagens para uma versão em escala de cinza.

O código usa uma imagem de entrada e permite escolher a quantidade de threads que processarão a imagem.

O algoritmo desenvolvido calcula o tempo de processamento, permitindo comparar o desempenho de diferentes quantidades de threads.

## Instruções para a execução do código:
### No Linux:

Compilar: gcc processador_paralelo.c -o processador_paralelo.exe -lm -pthread

Executar: ./processador_paralelo.exe imagem_de_entrada.jpg imagem_escala_cinza.jpg número de threads

### No Windows: 

Compilar: gcc processador_paralelo.c -o processador_paralelo.exe -pthread

Executar: ./processador_paralelo.exe imagem_de_entrada.jpg imagem_escala_cinza.jpg número de threads

