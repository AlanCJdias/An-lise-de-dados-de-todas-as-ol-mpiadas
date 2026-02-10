#pragma once
#include <stdio.h>
#include <stddef.h>

/* Remove \n e \r do final da linha */
void removerQuebraLinha(char *linha);

/*
  Separa uma linha CSV em campos, respeitando aspas.
  - Modifica a linha (coloca '\0' nos separadores)
  - Retorna quantos campos foram encontrados
*/
int separarCsv(char *linha, char **campos, int maxCampos);

/* Procura índice de coluna no cabeçalho (comparação exata) */
int encontrarIndiceColuna(char **camposCabecalho, int totalCampos, const char *nomeColuna);

/*
  Extrai o ano e a estação a partir do campo "Games" (ex: "2016 Summer")
  - retorna 1 se conseguiu extrair, 0 se falhar
*/
int extrairAnoEEstacaoGames(const char *games, int *ano, char *estacao3, size_t tamEstacao3);

/* procura o NOC pelo nome do país (coluna region) no noc_regions.csv
   retorna 1 se achou e copia o NOC para nocSaida */
int obterNocPorNomePais(
    const char *nomePais,
    char *nocSaida,
    int tamNocSaida
);

/* Define o diretório base onde ficam as pastas results/, athletes/, clean-data/ */
void definirDiretorioDados(const char *diretorioBase);

/* Abre um arquivo dentro do diretório base.
   Ex: abrirArquivoDados("results/results.csv", "r") */
FILE *abrirArquivoDados(const char *caminhoRelativo, const char *modo);

