//para uma determinada competição(um esporte em específico), determine o peso médio dos atletas masculinos e atletas femininos.

#include <stdio.h>
#include <string.h>
#include "ddxquestao5.h"

int contemElemento(int *lista, int tamanho, int valor) {
    for (int i = 0; i < tamanho; i++) {
        if (lista[i] == valor)
            return 1;//percorre toda a lista, se tiver na lista, retorna um, se não, retorna 0.
    }
    return 0;
}

int proximoCampo(char **ptr, char **campo) {
    char *p = *ptr;
    int dentroAspas = 0; // Flag para controlar se estamos dentro de aspas(o CSV tem muitas aspas).

    *campo = p; 

    while (*p) { // Enquanto não chegar ao final da string
        if (*p == '"') { // se p for aspas .
            dentroAspas = !dentroAspas; //alterar o valor da variável para informar que está entre aspas.
        } else if (*p == ',' && !dentroAspas) {// se p for igual a , e estiver fora das aspas.
            *p = '\0';
            *ptr = p + 1; // Atualiza ptr para apontar para o próximo campo
            return 1; // aqui ele já achou 0 campo inteiro.
        }
        p++; // Avança para o próximo caractere
    }

    *ptr = p;  // Atualiza ptr para o final da string, ou seja, a parte que foi pega no campo foi "removida".
    return 1;
}

int executarDdxQuestao5(void){
   

    FILE *arquivo = fopen("results.csv", "r");
    if (!arquivo) {
        perror("Erro ao abrir results.csv");
        return 1;
    }
    
    //aqui que ficará as respostas importantes para a questão.
    int qtdatlMasc = 0, qtdatlFem = 0;
    float somaatlMasc = 0, somaatlFem = 0;

    char modalidadeBuscar[50];

    

    // Entrada da modalidade
    printf("Digite a modalidade: ");
    fgets(modalidadeBuscar, sizeof(modalidadeBuscar), stdin);
    modalidadeBuscar[strcspn(modalidadeBuscar, "\n")] = '\0';

    // Pula o cabeçalho do CSV
    fgets(linha, sizeof(linha), arquivo);

    // Leitura do arquivo, lendo cada atleta até o final do arquivo
    while (fgets(linha, sizeof(linha), arquivo)) {
        sscanf(linha, "%49[^;];%49[^;];%c;%f",
               nome, modalidade, &sexo, &peso);
        //filtro para buscar apenas a modalidade digitada
        if (strcmp(modalidade, modalidadeBuscar) == 0) {
            // separa os atletas e soma os pesos
            if (sexo == 'M') {
                somaatlMasc += peso;
                qtdatlMasc++;
            } else if (sexo == 'F') {
                somaatlFem += peso;
                qtdatlFem++;
            }
        }
    }

    fclose(arquivo);

    // Resultados
    printf("\nModalidade: %s\n", modalidadeBuscar);

    if (qtdatlMasc > 0)
        printf("Peso medio masculino: %.2f kg\n", somaatlMasc / qtdatlMasc);
    else
        printf("Nao ha atletas masculinos nessa modalidade.\n");

    if (qtdatlFem > 0)
        printf("Peso medio feminino: %.2f kg\n", somaatlFem / qtdatlFem);
    else
        printf("Nao ha atletas femininas nessa modalidade.\n");

    return 0;
}
