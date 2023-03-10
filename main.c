#include <stdio.h>
#include "library.h"

int menuOption = 0;
char LOADING_OPTION[] = {"\nOpcao em construcao . . .\n"};
char INVALID_OPTION[] = {"\nOpcao invalida, digite um numero de 1 a 6\n"};
char FINISH_PROGRAM[] = {"\n-------------- FINALIZANDO PROGRAMA -------------\n"};
char INIT_APP[] = {"\n-------------------------- ATENCAO -----------------------\nCaro usuario, para funcionamento correto do programa:\n - Insira textos e caracteres preferencialmente em letras MAIUSCULAS\n - O CPF digitado para consulta, cadastro ou outros, deve ser composto apenas de numeros\n - Os nomes das cidades e UFS devem ser escritos sem acento\n - A UF/ESTADO digitada deve ser composta apenas de DOIS caracteres\n\n PRESSIONE ENTER PARA PROSSEGUIR:"};

int main()
{
    printf("%s", INIT_APP);
    char whiteSpace[2];
    scanf("%[^\n]s", whiteSpace);
    printf("\n\n");
    int menuOption;
    while (1)
    {
        menuOption = executeMenu();
        if (menuOption == 6)
        {
            printf("%s", FINISH_PROGRAM);
            break;
        }
        switch (menuOption)
        {
        case 1:
            registerPerson();
            break;
        case 2:
            consultPerson();
            break;
        case 3:
            listPeopleByCity();
            break;
        case 4:
            generateReport();
            break;
        case 5:
            removeRecord();
            break;
        default:
            printf("%s", INVALID_OPTION);
            break;
        }
    }

    fflush(stdin);
    return 0;
}