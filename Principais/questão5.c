//Para uma determinada edição dos jogos olímpicos, calcule o peso médio dos atletas medalhistas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 262144

int main(){
    //primeireo passo será guardar o ID de todos os atletas que ganharam algum jogo de determinada edição.

    FILE *arquivo = fopen("arquivoscsvs/results/results.csv","r");//pega os arquivos com todos os jogos.
    int edição_escolhida=9999;
    char linha[MAX_LINE];
    while (1==1){
        printf("Digite qual edição das olímpiadas você deseja calcular o peso médio:");
        scanf("%d", &edição_escolhida);
        if(edição_escolhida%2 != 0){
            printf("Nesse ano não ouve competição.");
            return 1;// realiza o fim do programa.
        }
    }

    //agora precisamos fazer uma lista dinâmica para armazenar todos os ID's dos atletas que ganharma uma olípiada em determinado ano.
    int *lista;
    int tamanho;

    fgets(linha, MAX_LINE, arquivo);//apenas para remover o cabeçalho.

    while(fgets(linha, MAX_LINE, arquivo)){
        char *games, *event, *team, *pos, *medal, *as, *athlete_id, *noc;

        games = strtok(linha, ",");
        event = strtok(NULL, ",");
        team = strtok(NULL, ",");
        pos = strtok(NULL, ",");
        medal = strtok(NULL, ",");
        as = strtok(NULL, ",");
        athlete_id = strtok(NULL, ",");
        noc = strtok(NULL, ",");

        if (medal == NULL || strlen(medal) == 0) {
            continue; // não ganhou medalha
        }

        if (athlete_id != NULL) {
            int id = atoi(athlete_id); // converte string → int

            int *temp = realloc(lista, (tamanho + 1) * sizeof(int));
            if (temp == NULL) {
                free(lista);
                printf("Erro de memória.\n");
                return 1;
            }
            lista = temp;
            lista[tamanho] = id;
            tamanho++;
        }
    }
    free(arquivo);//não é mais necessário ter esse arquivo aberto.

    FILE *bio = fopen("arquivoscsvs/athletes/bios.csv","r");

    fgets(linha, MAX_LINE, arquivo);//apenas para remover o cabeçalho.    

    while(fgets(linha, MAX_LINE, bio)){
        char *games, *event, *team, *pos, *medal, *as, *athlete_id, *noc;

        games = strtok(linha, ",");
        event = strtok(NULL, ",");
        team = strtok(NULL, ",");
        pos = strtok(NULL, ",");
        medal = strtok(NULL, ",");
        as = strtok(NULL, ",");
        athlete_id = strtok(NULL, ",");
        noc = strtok(NULL, ",");
        
        if (medal == NULL || strlen(medal) == 0) {
            continue; // não ganhou medalha
        }

        if (athlete_id != NULL) {
            int id = atoi(athlete_id); // converte de string para int.

            int *temp = realloc(lista, (tamanho + 1) * sizeof(int));//essa parte aqui faz a lista dinâmica. que adiciona ( ou deveria adicionar ), todos os ID's dos ganhadores de determinado ano.
            if (temp == NULL) {
                free(lista);
                printf("Erro de memória.\n");
                return 1;
            }
            lista = temp;
            lista[tamanho] = id;
            tamanho++;
        }
    }

    return 0;
}