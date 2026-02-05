//Para uma determinada edição dos jogos olímpicos, calcule o peso médio dos atletas medalhistas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "questao5.h"

#define MAX_LINE 262144

int contemElemento(int *v, int tamanho, int valor) {
    for (int i = 0; i < tamanho; i++) {
        if (v[i] == valor)
            return 1;
    }
    return 0;
}

int executarQuestao5(void){
    //primeireo passo será guardar o ID de todos os atletas que ganharam algum jogo de determinada edição.

    double imc_total,qtdd_atletas=0;// esse será o que a questão pede.

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

        if (strlen(medal) == 0) {
            continue; // não ganhou medalha
        }

        if (strlen(athlete_id) != 0) {//verifica se há String do id do atleta
            int id = atoi(athlete_id); // converte de string para int

            int *temp = realloc(lista, (tamanho + 1) * sizeof(int));//refaz a lista adicionando esse id.
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
    if (tamanho==0){
        return 0;
    }
    FILE *bio = fopen("arquivoscsvs/athletes/bios_locs.csv","r");

    fgets(linha, MAX_LINE, arquivo);//apenas para remover o cabeçalho.    

    while(fgets(linha, MAX_LINE, bio)){
        //o arquivo original CSV é:
        //athlete_id,name,born_date,born_city,born_region,born_country,NOC,height_cm,weight_kg,
        char *id, *name, *born, *born_city, *born_region, *born_country, *NOC, *height, *weight;
        double peso,altura,imc;

        //aqui abaixo está o reconhecimendo dos dados:

        id = strtok(linha, ",");//parte que também nos interessa.
        name = strtok(NULL, ",");
        born = strtok(NULL, ",");
        born_city = strtok(NULL, ",");
        born_region = strtok(NULL, ",");
        born_country = strtok(NULL, ",");
        NOC = strtok(NULL, ",");
        height = strtok(NULL, ",");//parte que também nos interessa.
        weight = strtok(NULL, ",");//parte que nos interessa.

        if(contemElemento(lista,tamanho,atoi(id)) == 1){//funçãpo para percorrer a lista e verificar se o id está presente na lista dos ganhadores.
            peso = atoi(weight);
            altura = atoi(height);
            imc = peso/(altura*altura);//fazer o calculo do imc desse atleta.
            imc_total += imc;//adicionar o imc no imc total.
            qtdd_atletas += 1;//adiciona um atleta na quantidade de atletas total daquele ano.
        }else{
            continue;
        }
    }
    free(bio);//não é mais necessário ter esse arquivo aberto.
    return 0;
}
 