#include "csvUtil.h"
#include <string.h>  // strlen, strcmp, strncpy, memmove
#include <stdlib.h>  // atoi
#include <ctype.h>   // isdigit, tolower

// Tamanho máximo para buffers de caminho (diretório + arquivo)
#define TAM_CAMINHO 1024

/*
 * removerQuebraLinha:
 * Remove caracteres de fim de linha que o fgets normalmente traz:
 *  - '\n' (LF) e '\r' (CR)
 * Faz isso APENAS no final da string, repetindo até não haver mais.
 */
void removerQuebraLinha(char *linha) {
    if (!linha) return;

    size_t n = strlen(linha);

    // Enquanto o último caractere for quebra de linha, zera e "encurta" a string
    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r')) {
        linha[n - 1] = '\0';
        n--;
    }
}

/*
 * separarCsv:
 * Separa uma linha CSV em campos, gravando em 'campos' ponteiros para dentro da própria 'linha'.
 *
 * IMPORTANTE:
 * - Esta função MODIFICA a string 'linha', substituindo vírgulas separadoras por '\0'
 * - 'campos[i]' passa a apontar para pedaços dentro de 'linha'
 *
 * Recursos suportados:
 * - Campos entre aspas: "texto, com virgula"
 * - Aspas escapadas dentro de campo com aspas: "" vira " (uma aspa literal)
 *
 * Parâmetros:
 * - linha: buffer com a linha CSV (será alterado)
 * - campos: array de char* para receber os ponteiros dos campos
 * - maxCampos: capacidade do array 'campos'
 *
 * Retorno:
 * - total de campos encontrados na linha (mesmo que > maxCampos).
 *   (Ou seja: você pode saber que existem mais campos do que você armazenou.)
 */
int separarCsv(char *linha, char **campos, int maxCampos) {
    if (!linha || !campos || maxCampos <= 0) return 0;

    int total = 0;          // quantos campos foram encontrados
    int dentroAspas = 0;    // flag: estamos dentro de um campo "entre aspas"?
    char *inicio = linha;   // início do campo atual (ponteiro dentro de 'linha')

    /*
     * Percorre caractere a caractere:
     * - Se achar aspas '"', alterna dentroAspas (entra/sai do modo "entre aspas")
     * - Se achar vírgula ',' e NÃO estiver dentro de aspas, finaliza um campo
     */
    for (char *p = linha; *p; p++) {

        if (*p == '"') {
            /*
             * Trata "" como aspas literal dentro de um campo entre aspas:
             * Ex.: "He said ""Hello"""  ->  He said "Hello"
             *
             * Quando estamos dentro de aspas e encontramos um " seguido de outro ",
             * removemos um deles com memmove (puxa a string 1 posição para a esquerda).
             */
            if (dentroAspas && p[1] == '"') {
                memmove(p, p + 1, strlen(p)); // remove uma aspa duplicada
                continue; // fica no mesmo contexto de dentroAspas
            }

            // Alterna estado: entrou ou saiu de aspas externas do campo
            dentroAspas = !dentroAspas;

        } else if (*p == ',' && !dentroAspas) {
            /*
             * Vírgula "válida" como separador (fora de aspas):
             * - Troca a vírgula por '\0' para terminar a string do campo atual
             * - Guarda o ponteiro do início do campo
             * - Move 'inicio' para o próximo caractere após a vírgula
             */
            *p = '\0';

            if (total < maxCampos) campos[total] = inicio; // só armazena se couber
            total++;

            inicio = p + 1; // próximo campo começa após a vírgula
        }
    }

    /*
     * Após o loop, precisamos registrar o último campo (não termina com vírgula).
     */
    if (total < maxCampos) campos[total] = inicio;
    total++;

    /*
     * Pós-processamento dos campos armazenados (até maxCampos):
     * 1) Trim simples de espaços ' ' nas bordas
     * 2) Remove aspas externas: se começa e termina com '"', corta essas aspas
     */
    for (int i = 0; i < total && i < maxCampos; i++) {
        char *c = campos[i];

        // Remove espaços do início (apenas ' ', não tabs)
        while (*c == ' ') c++;

        // Remove espaços do final
        size_t len = strlen(c);
        while (len > 0 && c[len - 1] == ' ') {
            c[len - 1] = '\0';
            len--;
        }

        // Remove aspas externas, se existirem (campo com pelo menos 2 chars)
        if (len >= 2 && c[0] == '"' && c[len - 1] == '"') {
            c[len - 1] = '\0';  // remove a aspa final
            campos[i] = c + 1;  // aponta para depois da aspa inicial
        } else {
            campos[i] = c;      // campo sem aspas externas
        }
    }

    return total;
}

/*
 * encontrarIndiceColuna:
 * Procura o índice (posição) de uma coluna pelo nome, usando o cabeçalho já separado.
 *
 * Retorna:
 * - índice (0..totalCampos-1) se encontrar
 * - -1 se não encontrar
 */
int encontrarIndiceColuna(char **camposCabecalho, int totalCampos, const char *nomeColuna) {
    if (!camposCabecalho || !nomeColuna) return -1;

    for (int i = 0; i < totalCampos; i++) {
        if (camposCabecalho[i] && strcmp(camposCabecalho[i], nomeColuna) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * extrairAnoEEstacaoGames:
 * Extrai:
 * - ano (int): os 4 primeiros dígitos
 * - estacao3 (char*): a palavra após o ano (ex.: "Summer" / "Winter")
 *
 * Exemplo esperado de entrada:
 *   "2012 Summer"
 *
 * Retorna 1 se conseguiu extrair, 0 se falhar.
 */
int extrairAnoEEstacaoGames(const char *games, int *ano, char *estacao3, size_t tamEstacao3) {
    if (!games || !ano || !estacao3 || tamEstacao3 < 2) return 0;

    // Pula espaços iniciais
    while (*games == ' ') games++;

    // Captura exatamente os 4 primeiros dígitos do ano
    char anoTxt[5] = {0};
    int i = 0;
    while (i < 4 && isdigit((unsigned char)games[i])) {
        anoTxt[i] = games[i];
        i++;
    }
    if (i != 4) return 0; // se não tiver 4 dígitos, formato inválido

    *ano = atoi(anoTxt);

    // Anda o ponteiro para depois do ano e pula espaços
    const char *p = games + 4;
    while (*p == ' ') p++;

    // Copia a "estação" (até espaço ou fim), respeitando o tamanho do buffer
    size_t j = 0;
    while (*p && *p != ' ' && j + 1 < tamEstacao3) {
        estacao3[j++] = *p++;
    }
    estacao3[j] = '\0';

    return (j > 0);
}

/*
 * iguaisSemCase:
 * Comparação case-insensitive (não diferencia maiúsculas/minúsculas).
 *
 * static => função privada deste arquivo (não exporta para outros .c)
 */
static int iguaisSemCase(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb) return 0;
        a++;
        b++;
    }
    // Verdadeiro somente se ambas terminam ao mesmo tempo
    return *a == '\0' && *b == '\0';
}

/*
 * obterNocPorNomePais:
 * Abre o arquivo "clean-data/noc_regions.csv", encontra as colunas "NOC" e "region"
 * no cabeçalho, e procura uma linha onde region == nomePais (ignorando case).
 *
 * Se encontrar, copia o NOC para nocSaida.
 *
 * Retorna:
 * - 1 se encontrou e copiou
 * - 0 se não encontrou / erro de arquivo / formato
 */
int obterNocPorNomePais(const char *nomePais, char *nocSaida, int tamNocSaida) {
    // Usa a infra de diretório base + caminho relativo
    FILE *arq = abrirArquivoDados("clean-data/noc_regions.csv", "r");
    if (!arq) return 0;

    char linha[65536]; // buffer grande para linhas longas
    char *campos[32];  // vetor para armazenar ponteiros dos campos (até 32)

    // Lê o cabeçalho
    if (!fgets(linha, sizeof(linha), arq)) {
        fclose(arq);
        return 0;
    }

    removerQuebraLinha(linha);
    int total = separarCsv(linha, campos, 32);

    // Descobre em que posição estão as colunas desejadas
    int idxNoc = encontrarIndiceColuna(campos, total, "NOC");
    int idxRegion = encontrarIndiceColuna(campos, total, "region");

    if (idxNoc < 0 || idxRegion < 0) {
        fclose(arq);
        return 0;
    }

    // Lê linha a linha do CSV e procura a região/pais
    while (fgets(linha, sizeof(linha), arq)) {
        removerQuebraLinha(linha);

        int n = separarCsv(linha, campos, 32);

        // Garante que a linha tem campos suficientes
        if (n <= idxRegion || n <= idxNoc) continue;

        // Compara ignorando case (maiúsculas/minúsculas)
        if (iguaisSemCase(campos[idxRegion], nomePais)) {
            // Copia o NOC para saída com segurança
            strncpy(nocSaida, campos[idxNoc], tamNocSaida - 1);
            nocSaida[tamNocSaida - 1] = '\0';

            fclose(arq);
            return 1;
        }
    }

    fclose(arq);
    return 0;
}

/*
 * diretório base onde ficam os dados (CSV etc).
 * Padrão: "arquivoscsvs"
 *
 * static => variável privada deste arquivo
 */
static char diretorioDados[TAM_CAMINHO] = "arquivoscsvs";

/*
 * removerBarraFinal:
 * Remove barras no final do caminho para evitar "//" ou "\\"
 * quando montar "diretorio/arquivo".
 */
static void removerBarraFinal(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '/' || s[n - 1] == '\\')) {
        s[n - 1] = '\0';
        n--;
    }
}

/*
 * definirDiretorioDados:
 * Define o diretório base (root) onde os CSVs estão.
 * Ex.: "arquivoscsvs" ou "/home/user/arquivoscsvs"
 */
void definirDiretorioDados(const char *diretorioBase) {
    if (!diretorioBase || diretorioBase[0] == '\0') return;

    // Copia com proteção de tamanho
    strncpy(diretorioDados, diretorioBase, sizeof(diretorioDados) - 1);
    diretorioDados[sizeof(diretorioDados) - 1] = '\0';

    // Remove barra final para padronizar
    removerBarraFinal(diretorioDados);
}

/*
 * abrirArquivoDados:
 * Monta um caminho completo baseado no diretório base + caminho relativo:
 *   caminhoCompleto = "<diretorioDados>/<caminhoRelativo>"
 * e abre com fopen no modo indicado.
 */
FILE *abrirArquivoDados(const char *caminhoRelativo, const char *modo) {
    if (!caminhoRelativo || !modo) return NULL;

    char caminhoCompleto[TAM_CAMINHO];

    // Monta "base/relativo" (por isso removemos barra final antes)
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", diretorioDados, caminhoRelativo);

    return fopen(caminhoCompleto, modo);
}
