#include "csvUtil.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define TAM_CAMINHO 1024

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




/* comparação sem diferenciar maiúsculas/minúsculas */
static int iguaisSemCase(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

int obterNocPorNomePais(const char *nomePais, char *nocSaida, int tamNocSaida) {
    FILE *arq = abrirArquivoDados("clean-data/noc_regions.csv", "r");
    if (!arq) return 0;

    char linha[65536];
    char *campos[32];

    if (!fgets(linha, sizeof(linha), arq)) {
        fclose(arq);
        return 0;
    }

    removerQuebraLinha(linha);
    int total = separarCsv(linha, campos, 32);

    int idxNoc = encontrarIndiceColuna(campos, total, "NOC");
    int idxRegion = encontrarIndiceColuna(campos, total, "region");

    if (idxNoc < 0 || idxRegion < 0) {
        fclose(arq);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arq)) {
        removerQuebraLinha(linha);
        int n = separarCsv(linha, campos, 32);
        if (n <= idxRegion || n <= idxNoc) continue;

        if (iguaisSemCase(campos[idxRegion], nomePais)) {
            strncpy(nocSaida, campos[idxNoc], tamNocSaida - 1);
            nocSaida[tamNocSaida - 1] = '\0';
            fclose(arq);
            return 1;
        }
    }

    fclose(arq);
    return 0;
}

/* Guarda o diretório base (ex: "/home/.../arquivoscsvs" ou "arquivoscsvs") */
static char diretorioDados[TAM_CAMINHO] = "arquivoscsvs";

/* Remove barra final para evitar "//" ao montar caminho */
static void removerBarraFinal(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '/' || s[n - 1] == '\\')) {
        s[n - 1] = '\0';
        n--;
    }
}

void definirDiretorioDados(const char *diretorioBase) {
    if (!diretorioBase || diretorioBase[0] == '\0') return;

    strncpy(diretorioDados, diretorioBase, sizeof(diretorioDados) - 1);
    diretorioDados[sizeof(diretorioDados) - 1] = '\0';

    removerBarraFinal(diretorioDados);
}

FILE *abrirArquivoDados(const char *caminhoRelativo, const char *modo) {
    if (!caminhoRelativo || !modo) return NULL;

    char caminhoCompleto[TAM_CAMINHO];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", diretorioDados, caminhoRelativo);

    return fopen(caminhoCompleto, modo);
}