//para uma determinada competição(um esporte em específico e um determinado ano) determine o peso médio dos atletas masculinos e atletas femininos(naquele ano e naquele esporte)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "pesoMedioEsporte.h"
#include "../Bibliotecas/csvUtil.h"
#define MAX_LINE 262144

int contemnumero(int *lista, int tamanho, int valor) {
    for (int i = 0; i < tamanho; i++) {
        if (lista[i] == valor)
            return 1;//percorre toda a lista, se tiver na lista, retorna um, se não, retorna 0.
    }
    return 0;
}

int proximocampo(char **ptr, char **campo) {
    char *p = *ptr;
    int dentroAspas = 0; // Flag para controlar se estamos dentro de aspas(o CSV tem muitas aspas).

    *campo = p; 

    while (*p) { // Enquanto não chegar ao final da string
        if (*p == '"') { // se p for aspas .
            dentroAspas = !dentroAspas; //alterar o valor da variável para informar que está entre aspas.
        } else if (*p == ',' && !dentroAspas) {// se p for igual a , e estiver fora das aspas.
            *p = '\0';
            *ptr = p + 1; // Atualiza ptr para apontar para o próximo campo
            return 1; // aqui ele já achou 0 campo inteiro.
        }
        p++; // Avança para o próximo caractere
    }

    *ptr = p;  // Atualiza ptr para o final da string, ou seja, a parte que foi pega no campo foi "removida".
    return 1;
}

int executarPesoMedioEsporte(){
    setlocale(LC_ALL, "portuguese");
    //primeireo passo será guardar o ID de todos os atletas que ganharam algum jogo de determinada edição.
    //todas as variáveis necessárias para se fazer a ddx da questão 5.

    double peso_fem=0,peso_mas=0;//peso masculino e feminino.
    int qtdd_fem=0, qtdd_mas=0;//quantidade de atletas em determinado ano.
    char esporte_escolhido[50];

    printf("Digite qual eaporte você deseja calcular o peso médio feminino e masculino em um determinado ano.");
    fgets(esporte_escolhido, sizeof(esporte_escolhido), stdin);//preenche corretamente o esporte escolhido com o tamanho adequado.
    esporte_escolhido[strcspn(esporte_escolhido, "\n")] = 0; // Remove o \n
    
    int edicao_escolhida=9999;
    char linha[MAX_LINE];

    while (1==1){
        printf("Digite qual ano das olímpiadas você deseja calcular o peso médio:");
        scanf("%d", &edicao_escolhida);
        if(edicao_escolhida%2 != 0 || edicao_escolhida <1880){//se for ímpar, não houve competição.
            printf("Nesse ano não ouve competição.");
            return 0;// realiza o fim do programa.
        }else{
            break;
        }
    }

    //está na paste "clean-data"
    FILE *resultados = abrirArquivoDados("clean-data/results.csv","r");//pega os arquivos com todos os jogos.

    //agora precisamos fazer uma lista dinâmica para armazenar todos os ID's dos atletas que ganharma uma olípiada em determinado ano.
    int *lista = NULL; // aqui estarão os ids dos ganhadores de medalhas em determinado ano.
    int tamanho = 0; // aqui é para saber o tamanho da lista. Começa em 0. 

    fgets(linha, MAX_LINE, resultados);//apenas para remover o cabeçalho.

    //é agora que de fato começa o código.

    while(fgets(linha, MAX_LINE, resultados)){//vai pegar linha por linha.

        //formato do arquivo: year,type,discipline,event,as,athlete_id
        char *year, *type, *discipline, *event, *as, *athlete_id;

        //eu tenho que remover a parte que está entre aspas.
        //é aqui que a porca torce o rabo.
        //aparentemente, agora está funcionando corretamente.

        char *ptr = linha; //ponteiro que aponta para o a linha.
        char *campo;// ponteiro para armazenar o início do campo.

        //toda essa parte aqui é para separar a linha pelas informações corretas.
        //funcionamento dessas funções explicada na função acima.

        proximocampo(&ptr, &year);
        proximocampo(&ptr, &type);
        proximocampo(&ptr, &discipline);
        proximocampo(&ptr, &event);
        proximocampo(&ptr, &as);
        proximocampo(&ptr, &athlete_id);


        // se a edição não for informada, pule para a próxima linha.
        if(year==NULL || strlen(year)==0){
            continue;
        }

        //se a edição que aconteceu o jogo for diferente, ele deve pular para a próxima linha, esse ano não interessa.
        if(edicao_escolhida!=(atoi(year))){
            continue;// Se o ano da competição for direfernte, ele não deve contabilizar.
        }

        if(discipline==NULL || strlen(discipline)==0){//se o tipo for vazio.
            continue;
        }

        if(strcmp(discipline, esporte_escolhido) != 0){//se os esportes forem diferentes deve ler a próixima linha.
            continue;
        }

        //se o id estiver algo e o id for maior que zero (garantir quer não é uma string vazia).
        if (athlete_id!=NULL && strlen(athlete_id) > 0 ) {//verifica se há String do id do atleta
            //printf("ANO: %d, Medal: '%s', ID: '%s'\n", atoi(games), Medal, athlete_id);
            if(contemnumero(lista,tamanho,atoi(athlete_id)) == 1){//verifica se esse atleta ja está na lista.
                continue;//se já estiver incluso, ele vai para a próxima 
            }
            // NO LOOP DO results.csv
            int id = atoi(athlete_id); // converte de string para int
            int *temp = realloc(lista, (tamanho + 1) * sizeof(int));//refaz a lista adicionando esse id.
            if (temp == NULL) {
                free(lista);
                printf("Erro de memória.\n");
                return 1;//precisa ter o return, pq se não dará erro.
            }
            lista = temp;//transformará a lista, na lista temporiaria feita.
            lista[tamanho] = id;// na nova posição, será adicionado o id do medalhista.
            tamanho++;//irá adicionar um ao tamanho.
        }
    }

    //está na pasta "athletes"
    FILE *bio_genero = abrirArquivoDados("athletes/bios.csv","r");

    fgets(linha, MAX_LINE, bio_genero);//apenas para remover o cabeçalho.

    //aqui eu irei fazer duas listas para os atletas masculinos e femininos.

    int *lista_fem = NULL; // aqui estarão os ids dos ganhadores de medalhas em determinado ano.
    int tamanho_fem = 0;

    int *lista_mas = NULL; // aqui estarão os ids dos ganhadores de medalhas em determinado ano.
    int tamanho_mas = 0;

    //criada as listas, agora preencer as listas:
    while(fgets(linha, MAX_LINE, bio_genero)){//vai pegar linha por linha.

        //essa parte eu acho que está certo.
        //o arquivo original CSV é:
        //formato do arquivo: Roles,Sex,Full name,Used name,Born,Died,NOC,athlete_id

        char *Roles, *Sex, *Full_name, *Used_name, *born, *Died, *NOC, *athlete_id;//adicionando todos os campos que são necessários para aquestão.

        //aqui abaixo está o reconhecimendo dos dados:
        //usando o mesmo esqueleto da parte anterior:

        char *ptr = linha;
        char *campo;

        //toda essa parte aqui é para separar a linha pelas informações corretas.
        //funcionamento dessas funções explicada na função acima.
        
        proximocampo(&ptr, &Roles);
        proximocampo(&ptr, &Sex);
        proximocampo(&ptr, &Full_name);
        proximocampo(&ptr, &Used_name);
        proximocampo(&ptr, &born);
        proximocampo(&ptr, &Died);
        proximocampo(&ptr, &NOC);
        proximocampo(&ptr, &athlete_id);

        // se o id não existir, ele deve ir para a próxima linha.
        if(athlete_id == NULL || strlen(athlete_id) == 0){//se o id for vazio, pule para a próxima linha.
            continue;
        }
        if(strcmp(Sex,"Female")==0 || strcmp(Sex,"Male")==0){}else{//se não for MALE ou FEMALE ele irá para a ouyra linha.
            continue;
        }
        if(contemnumero(lista,tamanho,atoi(athlete_id)) == 1){//funçãpo para percorrer a lista e verificar se o id está presente na lista dos ganhadores.
            int id = atoi(athlete_id);
            if(strcmp(Sex,"Female")==0){//se for mulher.
                int *temp = realloc(lista_fem, (tamanho_fem + 1) * sizeof(int));//refaz a lista adicionando esse id.
                if (temp == NULL) {
                    free(lista_fem);
                    printf("Erro de memória.\n");
                    return 1;//precisa ter o return, pq se não dará erro.
                }
                lista_fem = temp;//transformará a lista, na lista temporiaria feita, criandpo uma lista com todos os id's dos atletas femininos.
                lista_fem[tamanho_fem] = id;// na nova posição, será adicionado o id do medalhista.
                tamanho_fem++;//irá adicionar um ao tamanho.
            }else{
                int *temp = realloc(lista_mas, (tamanho_mas + 1) * sizeof(int));//refaz a lista adicionando esse id.
                if (temp == NULL) {
                    free(lista_mas);
                    printf("Erro de memória.\n");
                    return 1;//precisa ter o return, pq se não dará erro.
                }
                lista_mas = temp;//transformará a lista, na lista temporiaria feita, criandpo uma lista com todos os id's dos atletas masculinos.
                lista_mas[tamanho_mas] = id;// na nova posição, será adicionado o id do medalhista.
                tamanho_mas++;//irá adicionar um ao tamanho.
            }
        }
    }

    //agora que já temos a lista com todos os atletas masculinos e femininos em um determinado esporte em um determoinado ano. 
    //agora iremos calcular o peso médio de cada gênero do esporte em determinado ano.

    //esse arquivo contme os pesos de todos os atletas por id.
    //está na paste "clean-data"
    FILE *pesos = abrirArquivoDados("clean-data/bios.csv", "r");

    fgets(linha, MAX_LINE, pesos);//apenas para remover o cabeçalho.

    while(fgets(linha, MAX_LINE, pesos)){//vai pegar linha por linha.

        //essa parte eu acho que está certo.
        //o arquivo original CSV é:
        //athlete_id,name,born_date,born_city,born_region,born_country,NOC,height_cm,weight_kg,died_date

        char *id, *name, *born, *born_city, *born_region, *born_country, *NOC, *height, *weight;//adicionando todos os campos que são necessários para aquestão.
        double peso;

        //aqui abaixo está o reconhecimendo dos dados:
        //usando o mesmo esqueleto da parte anterior:

        char *ptr = linha;
        char *campo;

        //toda essa parte aqui é para separar a linha pelas informações corretas.
        //funcionamento dessas funções explicada na função acima.
        
        proximocampo(&ptr, &id);
        proximocampo(&ptr, &name);
        proximocampo(&ptr, &born);
        proximocampo(&ptr, &born_city);
        proximocampo(&ptr, &born_region);
        proximocampo(&ptr, &born_country);
        proximocampo(&ptr, &NOC);
        proximocampo(&ptr, &height);
        proximocampo(&ptr, &weight);

        // se o peso ou id não existir, ele deve ir para a próxima linha.

        if(weight == NULL || strlen(weight) == 0 || id==NULL || strlen(id)==0){
            continue;
        }

        // NO LOOP DO bios.csv  
        if(contemnumero(lista,tamanho,atoi(id)) == 1){//funçãpo para percorrer a lista e verificar se o id está presente na lista dos ganhadores.
            peso = atof(weight);//pega a string com o valor do peso e transforma em float.
            if(contemnumero(lista_fem,tamanho_fem,atoi(id)) == 1){
                peso_fem+=peso;//adiciona o peso da atleta ao peso faminino.
                qtdd_fem++;//adiciona um na quantidade de etletas femininas.
            }else{
                peso_mas+=peso;//adiciona o peso da atleta ao peso masculino.
                qtdd_mas++;//adiciona um na quantidade de etletas masculinos.
            }
        }
    }

    fclose(pesos);
    fclose(resultados);//não é mais necessário ter esse arquivo aberto.
    fclose(bio_genero);//não é mais necessário ter esse arquivo aberto.
    free(lista);
    free(lista_fem);
    free(lista_mas);


    //resposta para o público feminino:
    if(qtdd_fem>0){
        printf("Na olímpiada de %d houve %d atletas femininas que jogaram %s.\n", edicao_escolhida, qtdd_fem, esporte_escolhido);
        printf("O peso médio feminino foi de: %.2lf\n", peso_fem/qtdd_fem);
    }else{
        printf("Na olímpiada de %d houve 0 atletas femininas que jogaram %s.\n", edicao_escolhida, esporte_escolhido);
    }

    //resposta para o público masculino:
    if(qtdd_mas>0){
        printf("Na olímpiada de %d houve %d atletas masculinos que jogaram %s\n.", edicao_escolhida, qtdd_mas, esporte_escolhido);
        printf("O peso médio masculino foi de: %.2lf\n", peso_mas/qtdd_mas);
    }else{
        printf("Na olímpiada de %d houve 0 atletas masculinos que jogaram %s.\n", edicao_escolhida, esporte_escolhido);
    }

    return 0;
}