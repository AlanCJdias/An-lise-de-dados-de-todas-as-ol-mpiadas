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

/* Conversões seguras (retornam 1 se ok, 0 se falhar) */
int converterInt(const char *texto, int *saida);
int converterLong(const char *texto, long *saida);
int converterDouble(const char *texto, double *saida);

/*
  Extrai o ano e a estação a partir do campo "Games" (ex: "2016 Summer")
  - retorna 1 se conseguiu extrair, 0 se falhar
*/
int extrairAnoEEstacaoGames(const char *games, int *ano, char *estacao3, size_t tamEstacao3);
