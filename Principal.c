//aqui irá ficar o código principal que irá chamar os outros arquivos.

//esse são os includes padrões.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//esse são os includes para chamar as outras questões, acho esse metodo mais organizado e prático.
#include "Bibliotecas/csvUtil.h"
#include "Principais/questao5.h"
#include "Principais/questao11.h"
#include "Principais/ddxquestao5.h"
#include "Principais/ddxquestao11.h"

static int lerOpcao(void){
    // Função para ler a opção do usuário de forma segura
    char buffer[20];        // buffer temporário para leitura
    if (!fgets(buffer, sizeof(buffer), stdin)) return -1; // erro na leitura
    char *fim = NULL; // ponteiro para o final da conversão
    long opc = strtol(buffer, &fim, 10);
    if (fim == buffer) return -1; // não foi possível converter para número
    return (int)opc;
}


int main(){
    
    int opcaoDeUsuario;
    /* ALTERAÇÃO: o professor informa onde está a pasta de CSVs (arquivoscsvs) */
    char caminhoDados[512];

    printf("Digite o caminho da pasta de dados (ex: /home/usuario/arquivoscsvs)\n");
    printf("Se estiver na pasta do projeto, apenas pressione ENTER para usar 'arquivoscsvs': ");
    if (fgets(caminhoDados, sizeof(caminhoDados), stdin)) {
        removerQuebraLinha(caminhoDados);

        if (caminhoDados[0] == '\0') {
            definirDiretorioDados("arquivoscsvs"); /* padrão */
        } else {
            definirDiretorioDados(caminhoDados);
        }
    }

    while(1){
        printf("Escolha uma questao para executar:(digite um numero correspondente ao numero ligado a questao) \n");
        printf("1 - Questao 5\n");
        printf("2 - Questao 11\n");
        printf("3 - Ddx Questao 5\n");
        printf("4 - Ddx Questao 11\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
 
        opcaoDeUsuario = lerOpcao();// lendo a opção do usuário

            // verificando a opção escolhida, esta simples por enquanto, porem se sobrar tempo irei melhorar.
        if (opcaoDeUsuario == 0) {  
            printf("Saindo do programa.\n");
            break;
        }
        else if (opcaoDeUsuario == 1) {
            executarQuestao5();
        }
        else if (opcaoDeUsuario == 2) {
            executarQuestao11();
        }
        else if (opcaoDeUsuario == 3) {
            executarDdxQuestao5();
        }
        else if (opcaoDeUsuario == 4) {
            executarDdxQuestao11();
        }
        else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }
    
    return 0;
}