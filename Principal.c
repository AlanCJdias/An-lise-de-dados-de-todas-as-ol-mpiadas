// Aqui fica o código principal que chama os outros arquivos (as “questões” do projeto).

#include <stdio.h>   // printf, fgets
#include <stdlib.h>  // strtol
#include <string.h>  // funções utilitárias de string (padrão, mesmo que pouco usado aqui)

// Includes dos módulos do projeto
#include "Bibliotecas/csvUtil.h"             // definirDiretorioDados, removerQuebraLinha
#include "Principais/pesoMedio.h"            // executarPesoMedio
#include "Principais/primeiraMedalha.h"      // executarPrimeiraMedalha (veja observação abaixo)
#include "Principais/pesoMedioEsporte.h"     // executarPesoMedioEsporte
#include "Principais/evolucaoPais.h"         // executarEvolucaoPais

/*
  lerOpcao:
  Lê a escolha do usuário de forma segura usando fgets + strtol.
 
  Por que isso é melhor que scanf?
  - fgets lê a linha inteira (incluindo espaços e evita travar se vier texto).
  - strtol tenta converter; se falhar, conseguimos detectar.
 
  Retorno:
  - número digitado (int) se converter com sucesso
  - -1 se houver erro ou se o usuário digitou algo não numérico
 */
static int lerOpcao(void) {
    char buffer[20];  // buffer temporário (suficiente para opções pequenas)
    if (!fgets(buffer, sizeof(buffer), stdin)) return -1; // falha de leitura

    char *fim = NULL; // vai apontar para onde parou a conversão
    long opc = strtol(buffer, &fim, 10);

    // Se fim == buffer, não converteu nenhum dígito
    if (fim == buffer) return -1;

    return (int)opc;
}

int main() {

    int opcaoDeUsuario;

    /*
      O professor/usuário informa onde está a pasta com os CSVs.
      A função definirDiretorioDados() (csvUtil.c) guarda isso internamente.
     
      A partir daí, abrirArquivoDados("results/results.csv","r")
      vai abrir "<diretorioDados>/results/results.csv".
     */
    char caminhoDados[512];

    printf("Digite o caminho da pasta de dados (ex: /home/usuario/arquivoscsvs)\n");
    printf("Se estiver na pasta do projeto, apenas pressione ENTER para usar 'arquivoscsvs': ");

    if (fgets(caminhoDados, sizeof(caminhoDados), stdin)) {
        // remove \n e \r do final
        removerQuebraLinha(caminhoDados);

        // Se o usuário só apertou ENTER, usa diretório padrão
        if (caminhoDados[0] == '\0') {
            definirDiretorioDados("arquivoscsvs");
        } else {
            // Caso contrário usa o caminho digitado
            definirDiretorioDados(caminhoDados);
        }
    }

    // Loop infinito do menu — só sai quando a opção for 0
    while (1) {
        printf("\nEscolha uma questao para executar (digite o numero correspondente):\n");
        printf("1 - Peso médio dos atletas\n");
        printf("2 - Primeira medalha de um país\n");
        printf("3 - Peso médio de um esporte em um ano\n");
        printf("4 - Evolução do número de medalhas de um país\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        // Lê a opção do usuário com segurança
        opcaoDeUsuario = lerOpcao();

        // Menu de decisão (if/else encadeado)
        if (opcaoDeUsuario == 0) {
            printf("Saindo do programa.\n");
            break;
        }
        else if (opcaoDeUsuario == 1) {
            // Chama o módulo que calcula peso médio dos medalhistas por edição
            executarPesoMedio();
        }
        else if (opcaoDeUsuario == 2) {
            /*
              Chama o módulo que encontra a primeira medalha de um país.
             
              OBS: verifique se no seu .h/.c o nome está exatamente:
              - executarPrimeiraMedalha()
              ou
              - executarPrimeiraMedalPais()
              porque se estiver diferente, vai dar erro de link/compilação.
             */
            executarPrimeiraMedalha();
        }
        else if (opcaoDeUsuario == 3) {
            // Chama o módulo que calcula peso médio por esporte e por sexo em um ano
            executarPesoMedioEsporte();
        }
        else if (opcaoDeUsuario == 4) {
            // Chama o módulo que mostra a evolução de medalhas do país nas 10 edições após a primeira medalha
            executarEvolucaoPais();
        }
        else {
            // Qualquer outro número (ou erro de leitura) cai aqui
            printf("Opcao invalida. Tente novamente.\n");
        }
    }

    return 0;
}
