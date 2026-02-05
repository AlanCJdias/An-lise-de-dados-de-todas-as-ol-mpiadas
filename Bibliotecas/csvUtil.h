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

/* Retornar 1 se a medalha e valida*/
int medalhaValida(const char *medalha);

/* O enum vai agrupar os tipos de medalhas*/
typedef enum { 

    medalhaNenhum = 0,
    medalhaOuro,
    medalhaPrata,
    medalhaBronze

} TipoMedalha;

/* Identifica o tipo de medalha a partir do texto */
TipoMedalha indetificarTipoMedalha(const char *medalha);


