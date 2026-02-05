#include "csvUtil.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void removerQuebraLinha(char *linha) {
    if (!linha) return;
    size_t n = strlen(linha);
    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r')) {
        linha[n - 1] = '\0';
        n--;
    }
}

int separarCsv(char *linha, char **campos, int maxCampos) {
    if (!linha || !campos || maxCampos <= 0) return 0;

    int total = 0;
    int dentroAspas = 0;
    char *inicio = linha;

    for (char *p = linha; *p; p++) {
        if (*p == '"') {
            /* Trata "" como aspas literal dentro do campo */
            if (dentroAspas && p[1] == '"') {
                memmove(p, p + 1, strlen(p));
                continue;
            }
            dentroAspas = !dentroAspas;
        } else if (*p == ',' && !dentroAspas) {
            *p = '\0';
            if (total < maxCampos) campos[total] = inicio;
            total++;
            inicio = p + 1;
        }
    }

    if (total < maxCampos) campos[total] = inicio;
    total++;

    /* Remove aspas externas e espaços simples nas bordas */
    for (int i = 0; i < total && i < maxCampos; i++) {
        char *c = campos[i];
        while (*c == ' ') c++;

        size_t len = strlen(c);
        while (len > 0 && c[len - 1] == ' ') {
            c[len - 1] = '\0';
            len--;
        }

        if (len >= 2 && c[0] == '"' && c[len - 1] == '"') {
            c[len - 1] = '\0';
            campos[i] = c + 1;
        } else {
            campos[i] = c;
        }
    }

    return total;
}

int encontrarIndiceColuna(char **camposCabecalho, int totalCampos, const char *nomeColuna) {
    if (!camposCabecalho || !nomeColuna) return -1;
    for (int i = 0; i < totalCampos; i++) {
        if (camposCabecalho[i] && strcmp(camposCabecalho[i], nomeColuna) == 0) {
            return i;
        }
    }
    return -1;
}

int converterInt(const char *texto, int *saida) {
    if (!texto || !*texto || !saida) return 0;
    char *fim = NULL;
    long v = strtol(texto, &fim, 10);
    if (fim == texto) return 0;
    *saida = (int)v;
    return 1;
}

int converterLong(const char *texto, long *saida) {
    if (!texto || !*texto || !saida) return 0;
    char *fim = NULL;
    long v = strtol(texto, &fim, 10);
    if (fim == texto) return 0;
    *saida = v;
    return 1;
}

int converterDouble(const char *texto, double *saida) {
    if (!texto || !*texto || !saida) return 0;
    char *fim = NULL;
    double v = strtod(texto, &fim);
    if (fim == texto) return 0;
    *saida = v;
    return 1;
}

int extrairAnoEEstacaoGames(const char *games, int *ano, char *estacao3, size_t tamEstacao3) {
    if (!games || !ano || !estacao3 || tamEstacao3 < 2) return 0;

    while (*games == ' ') games++;

    /* ano = primeiros 4 dígitos */
    char anoTxt[5] = {0};
    int i = 0;
    while (i < 4 && isdigit((unsigned char)games[i])) {
        anoTxt[i] = games[i];
        i++;
    }
    if (i != 4) return 0;

    *ano = atoi(anoTxt);

    /* pula espaço(s) */
    const char *p = games + 4;
    while (*p == ' ') p++;

    /* copia "Summer" ou "Winter" */
    size_t j = 0;
    while (*p && *p != ' ' && j + 1 < tamEstacao3) {
        estacao3[j++] = *p++;
    }
    estacao3[j] = '\0';

    return (j > 0);
}

int medalhaValida(const char *medalha) {
    if (!medalha) return 0;
    if (medalha[0] == '\0') return 0;
    if (strcmp(medalha, "NA") == 0) return 0;
    return 1;
}

TipoMedalha identificarTipoMedalha(const char *medalha) {
    if (!medalha) return medalhaNenhum;

    if (strcmp(medalha, "Gold") == 0)   return medalhaOuro;
    if (strcmp(medalha, "Silver") == 0) return medalhaPrata;
    if (strcmp(medalha, "Bronze") == 0) return medalhaBronze;

    return medalhaNenhum;
}