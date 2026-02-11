//Determine em qual edição olímpica um dado país conquistou sua primeira medalha e qual foi o tipo da medalha.
//todos os imports necessários.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Bibliotecas/csvUtil.h"  //biblioteca criado para facilitar a manipulação de arquivos CSV.
#include "primeiraMedalha.h" //incluindo o cabeçalho da questão 11
//definindo o tamanho máximo, em bytes de cada linha.
#define MAX_LINE 262144 // vou alterar para 262144, o valor aterior é 1024 

int executarPrimeiraMedalPais(void){

    //está faltando fazer a opção do usuário escolher o arquivo.
    //primeiramente, o mais essencial será abrir o arquivo.
    FILE *arquivo = abrirArquivoDados("results/results.csv", "r");//esse "r" é apenas para indicar que apenas será lido.
    if (arquivo == NULL) {//caso não ache o arquivo, ele deve resultar nessa parte.
        printf("Erro ao abrir o arquivo.\n");
        return 1;// o return faz o código parar.
    }

    int primeiro_ano = 9999;//serve apenas para ser alterado no futuro.
    char primeira_edicao[100] = "";//serve apenas para ser alterado no futuro.

   char linha[MAX_LINE];

    /* Agora o usuário pode digitar o nome do país (ex: Brazil) ou o NOC (ex: BRA) */
    char entradaPais[64];
    char pais_escolhido[4];

    printf("Digite o nome do país (ex: Brazil) ou o NOC (ex: BRA): ");
    if (!fgets(entradaPais, sizeof(entradaPais), stdin)) {
        fclose(arquivo);
        return 1;
    }
    removerQuebraLinha(entradaPais);

    /* Se tiver 3 caracteres, assumimos que já é um NOC */
    if (strlen(entradaPais) == 3) {
        strncpy(pais_escolhido, entradaPais, sizeof(pais_escolhido) - 1);
        pais_escolhido[sizeof(pais_escolhido) - 1] = '\0';
    } else {
        /* Caso contrário, busca o NOC no noc_regions.csv */
        if (!obterNocPorNomePais(entradaPais, pais_escolhido, sizeof(pais_escolhido))) {
            printf("Nao encontrei o país '%s' no noc_regions.csv.\n", entradaPais);
            printf("Dica: tente o nome em inglês (ex: Brazil) ou digite o NOC (ex: BRA).\n");
            fclose(arquivo);
            return 1;
        }
    }

    printf("NOC selecionado: %s\n", pais_escolhido);


    fgets(linha, MAX_LINE, arquivo);   // lê o cabeçalho
    removerQuebraLinha(linha);

    char *camposCabecalho[64];
    int totalCabecalho = separarCsv(linha, camposCabecalho, 64);

    int indiceGames = encontrarIndiceColuna(camposCabecalho, totalCabecalho, "Games");
    int indiceMedal = encontrarIndiceColuna(camposCabecalho, totalCabecalho, "Medal");
    int indiceNoc   = encontrarIndiceColuna(camposCabecalho, totalCabecalho, "NOC");

    if (indiceGames < 0 || indiceMedal < 0 || indiceNoc < 0) {
        printf("Erro: colunas necessárias não encontradas.\n");
        fclose(arquivo);
        return 1;
    }


    while (fgets(linha, MAX_LINE, arquivo)) {   // lê cada linha do arquivo
        //funcao feita para remover quebras de linha. essa função está em csvUtil.c
        removerQuebraLinha(linha);

        char *campos[64];
        int totalCampos = separarCsv(linha, campos, 64);//separa a linha em campos.

        if (totalCampos <= indiceNoc ||
            totalCampos <= indiceMedal ||
            totalCampos <= indiceGames) {   //verifica se os índices são válidos.
            continue;
        }
        //pega os campos necessários.
        char *games = campos[indiceGames];
        char *medal = campos[indiceMedal];
        char *noc   = campos[indiceNoc];

        if (!games || !medal || !noc) continue; //verifica se os campos não são nulos.

        if (strcmp(noc, pais_escolhido) == 0 && strlen(medal) > 0) { //compara se o noc é igual ao país escolhido e se a medalha não é vazia.

            int ano = atoi(games);

            if (ano < primeiro_ano) {
                primeiro_ano = ano;
                strcpy(primeira_edicao, games);
            }
        }
    }

    fclose(arquivo);//fecha o arquivo após averiguar todos os dados

    if (primeiro_ano == 9999) {//se o aprimeiro ano continuar o mesmo, então o país não ganhou medalhas.
        printf("O país %s não ganhou medalhas.\n", pais_escolhido);
    } else {
        printf("Primeira medalha do país %s foi em: %s\n",pais_escolhido, primeira_edicao);
    }

    return 0;
}