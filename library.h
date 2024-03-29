#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// CRIA E DEFINE VALORES PARA AS VARIAVEIS
#define LOCATIONS_LINES 5572
#define N_CPF_DIGITS 11
#define MAX_DAYS_IN_MONTH 31

typedef struct
{
    char Cpf[16];
    char Name[120];
    char Sex[2];
    int MonthBorn;
    int DayBorn;
    int yearBorn;
    char City[40];
    char Uf[3];
} Person;

typedef struct
{
    char Uf[3];
    char Name[120];

} Locations;

int i = 0;
int isCpfValid,
    isInputValid,
    isDateValid,
    ufExists,
    isElementPresentInGroup = 0;

// MENSAGENS
char RESET[] = {"\n------------- REINICIANDO O SISTEMA ------------"};
char ASK_INFO_AGAIN[] = {"\nPor favor, digite novamente:\n\n"};
char ASK_FOR_ANOTHER[] = {"\nDigite outro, se desejar:\n\n"};
char SUCCESS_REGISTERED[] = {"Pessoa cadastrada com sucesso!\n"};
char REMOVE_SUCCESS[] = {"PESSOA REMOVIDA COM SUCESSO!\n"};
char REMOVE_ERROR[] = {"Erro ao excluir a pessoa! Por favor, tente novamente."};
char ERROR_NOBODY_IN_CITY[] = {"Nenhum registro correspondente a cidade digitada!\n"};
char CONFIRM_EXCLUDE[] = {"Tem certeza de que quer excluir essa pessoa do sistema?(S/N)\n"};

// ERROS - VALIDACAO CPF
char ERROR_CPF_EXISTS[] = {"CPF ja cadastrado no sistema!"};
char ERROR_CPF_NOT_EXISTS[] = {"CPF nao encontrado na base de dados"};
char ERROR_CPF_INVALID[] = {"!CPF INVALIDO: Esse cpf nao e valido de acordo com as especificacoes da Receita Federal do Brasil."};
char ERROR_CPF_FORMAT[] = {"!FORMATO ESPERADO INVALIDO: O CPF precisa ter 11 digitos, apenas numeros, escritos sem tracos ou pontos."};
char ERROR_CPF_NOT_NUMBERS[] = {"!OPS, PARECE QUE VOCE DIGITOU ALGO DIFENTE DE NUMEROS: O CPF deve ser composto apenas de numeros, e deve ter 11 digitos escritos sem tracos ou pontos."};
char NO_RECORDS[] = {"\nAinda nao ha nenhum beneficiado registrado na base de dados!\n"};

// ERROS - VALIDACAO DE OUTROS INPUTS
char ERROR_INPUT_NOT_STRING[] = {"Voce deve digitar apenas letras, SEM simbolos ou acentos para esse campo!"};
char ERROR_INPUT_LIMIT[] = {"Ou voce ultrapassou o limite de caracteres para esse campo ou digitou apenas um espaco, nao e valido!"};
char ERRORDATE[] = {"Data de nascimento invalida\n"};
char ERRORFILE[] = {"Erro ao abrir a base de dados. Tente novamente mais tarde\n"};
char ERRORSEX[] = {"\nO sexo digitado deve ser F ou M"};
char ERROR_UF[] = {"UF inexistente\n"};
char ERROR_CITY[] = {"CIDADE inexistente"};

// MENSAGENS DE INICIALIZACAO DE OPCOES
char REPORT_INIT[] = {"------------------ RELATORIO DE PESSOAS BENEFICIADAS ----------------\n"};
char REMOVE_INIT[] = {"----------- REMOCAO DE REGISTRO ---------"};
char MENU[] = {"-------------- MENU DE OPCOES -------------\n"};
char PERCENT_BY_AGE[] = {"***** PORCENTAGEM POR IDADE\n"};
char PERCENT_BY_SEX[] = {"***** PORCENTAGEM POR SEXO\n"};

// FUNCAO DE LIMPAR TELA
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 1. FUNCAO INICIAL DO CODIGO
int executeMenu()
{
    int option;
    printf("\n%s", MENU);
    printf("Escolha uma das opcoes a seguir: \n");
    printf("1) Cadastrar Pessoa \n");
    printf("2) Consultar Pessoa \n");
    printf("3) Listar pessoas por cidade \n");
    printf("4) Gerar relatorio \n");
    printf("5) Excluir Pessoa \n");
    printf("6) Encerrar programa\n");

    scanf("%d", &option);
    clearScreen();
    return option;
}

// 2. VALIDACOES E FUNCOES AUXILIARES
// 2.1 VALIDADOR DE CPF
int validateCpf(char cpf[])
{
    int i, d1, d2, r;
    int cpfDigits[N_CPF_DIGITS];

    // CONFERE SE CPF ...
    if (strlen(cpf) == N_CPF_DIGITS) // TEM APENAS 11 DIGITOS?
    {
        // SE SIM - É COMPOSTO APENAS DE NÚMEROS?
        for (i = 0; i < 11; i++)
        {
            if (!isdigit(cpf[i]))
            {
                printf("%s", ERROR_CPF_NOT_NUMBERS);
                return 0;
            }
            cpfDigits[i] = cpf[i] - '0';
        }
    }
    else
    {
        printf("%s", ERROR_CPF_FORMAT);
        return 0;
    }

    // É COMPOSTO DE APENAS DE UM MESMO NUMERO?. EX: 88888888888
    for (i = 1; i < 11; i++)
    {
        if (cpfDigits[i] != cpfDigits[0])
        {
            break;
        }
    }
    if (i == N_CPF_DIGITS)
    {
        printf("%s", ERROR_CPF_INVALID);
        return 0;
    }

    // TEM SEU PRIMEIRO DIGITO VALIDO?
    d1 = 0;
    for (i = 0; i < 9; i++)
    {
        d1 += cpfDigits[i] * (10 - i);
    }

    r = 11 - (d1 % 11);
    if (r == 10 || r == 11)
    {
        r = 0;
    }

    if (r != cpfDigits[9])
    {
        printf("%s", ERROR_CPF_INVALID);
        return 0;
    }

    // TEM SEU SEGUNDO DIGITO VALIDO?
    d2 = 0;
    for (i = 0; i < 10; i++)
    {
        d2 += cpfDigits[i] * (11 - i);
    }
    r = 11 - (d2 % 11);
    if (r == 10 || r == 11)
    {
        r = 0;
    }
    if (r != cpfDigits[10])
    {
        printf("%s", ERROR_CPF_INVALID);
        return 0;
    }

    return 1;
}

int isFileOpen(FILE *file)
{
    // CONFERE O ARQUIVO FOI ABERTO, SE NAO - ERRO
    if (!file)
    {
        printf("%s%s", ERRORFILE, RESET);
        return 1;
    }
    return 0;
}

int checkIfFileHasRecords(FILE *file)
{
    // MOVE O PONTEIRO DE POSICAO PARA O INICIO DO ARQUIVO
    fseek(file, 0, SEEK_SET);
    int regCount = 0;
    // VARIAVEL PARA ARMAZENAR CADA LINHA DO ARQUIVO
    char line[5000];

    // ENQUANTO HOUVER LINHAS PARA SEREM LIDAS
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // INCREMENTA A QUANTIDADE DE REGISTROS
        regCount++;
    }
    // MOVE O PONTEIRO DE POSICAO PARA O INICIO DO ARQUIVO DE NOVO
    fseek(file, 0, SEEK_SET);
    return regCount;
}

int checkIfDateValid(int d, int m, int y)
{
    int leapYear = 0;
    int monthMaxDays = MAX_DAYS_IN_MONTH;

    // CONFERE SE O ANO INPUTADO TEM 4 DIGITOS
    if (y < 1000 || y > 9999)
    {
        printf("O ano precisa ser digitado com 4 digitos!\n");
        return 0;
    }

    // CONFERE SE MES ESTÁ ENTRE 1 E 12
    if (m < 1 || m > 12)
    {
        return 0;
    };

    // DIA É MAIOR QUE 1?
    if (d < 1)
    {
        return 0;
    };

    // SE O MES FOR FEVEREIRO, CONFERE SE É BISSEXTO O ANO
    if (m == 2)
    {
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
        {
            leapYear = 1;
        };

        if (leapYear == 1)
        {
            monthMaxDays = 29;
        }
        else
        {
            monthMaxDays = 28;
        };
    }
    else if (m == 4 || m == 6 || m == 9 || m == 11)
    {
        monthMaxDays = 30;
    };

    // CONFERE SE DIAS ESTAO DENTRO DO NUMERO MAXIMO DEFINIDO
    if (d > monthMaxDays)
    {
        return 0;
    };

    return 1;
}

int checkIfInputIsValid(char input[], int maxChar)
{
    int inputLen = strlen(input);

    // CONFERE SE O LIMITE MAXIMO DE CARACTERES NAO É EXCEDIDO
    if (inputLen > maxChar || (inputLen == 0))
    {
        printf("%s", ERROR_INPUT_LIMIT);
        return 1;
    }

    // CONFERE SE É COMPOSTA APENAS DE LETRAS
    for (int i = 0; i < inputLen; i++)
    {
        if (!(input[i] >= 'A' && input[i] <= 'Z' || input[i] == ' '))
        {
            printf("%s%s", ERROR_INPUT_NOT_STRING, ASK_INFO_AGAIN);
            return 1;
        }
    }

    return 0;
}

int checkIfCpfIsRegistered(char cpf[])
{
    FILE *readFile = fopen("person.txt", "r");

    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(readFile) == 1)
    {
        executeMenu();
    }

    Person personData;

    // LE DADOS DO ARQUIVO
    while (fscanf(readFile,
                  "%s\n%[^\n]\n%s\n%d/%d/%d\n%[^\n]\n%[^\n]",
                  personData.Cpf,
                  personData.Name,
                  personData.Sex,
                  &personData.DayBorn,
                  &personData.MonthBorn,
                  &personData.yearBorn,
                  personData.City,
                  personData.Uf) != EOF)
    {

        // SE O CPF FOR IGUAL, JA ESTÁ REGISRTADO NO SISTEMA
        if (strcmp(cpf, personData.Cpf) == 0)
        {
            fclose(readFile);
            return 1;
        }
    }
    return 0;
}

int ufCorrespondsToCity(char cityName[], char uf[])
{

    int size = 0;
    char locationLine[LOCATIONS_LINES];
    char *ufToken;
    char *cityTokenNoAccent;
    static char Uf[3];

    FILE *fp2 = fopen("locations.csv", "r");

    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(fp2) == 1)
    {
        executeMenu();
    }

    size = 0;
    // LER TODAS AS UFS DO ARQUIVO
    while (fgets(locationLine, sizeof(locationLine), fp2))
    {
        // PEGA A UF E CIDADE DO ARQUIVO
        ufToken = strtok(locationLine, ","); // Ignora a primeira coluna do arquivo
        ufToken = strtok(NULL, ",");         // Recebe a segunda coluna do arquivo
        cityTokenNoAccent = strtok(NULL, ",");
        cityTokenNoAccent = strtok(NULL, ",");

        // VERIFICA SE O NOME DAS CIDADES E UFs BATE
        if (strcmp(cityName, cityTokenNoAccent) == 0 && strcmp(uf, ufToken) == 0)
        {
            fclose(fp2);
            return 0;
        }
        size++;
    }

    fclose(fp2);
    return 1;
}

int checkIfLocationExists(char userInput[], int locationType)
{
    int size = 0;
    char locationLine[LOCATIONS_LINES];
    char *ufToken;
    char *cityTokenNoAccent;

    FILE *fp2 = fopen("locations.csv", "r");

    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(fp2) == 1)
    {
        executeMenu();
    }

    if (locationType == 1)
    {
        size = 0;
        // VERIFICA A UF
        while (fgets(locationLine, sizeof(locationLine), fp2))
        {
            ufToken = strtok(locationLine, ",");
            ufToken = strtok(NULL, ",");

            // VERIFICA SE TOKEN == INPUT - SE SIM, UF EXISTE
            if (strcmp(userInput, ufToken) == 0)
            {
                fclose(fp2);
                return 1;
            }
            size++;
        }
    }

    if (locationType == 0)
    {
        // VERIFICA A CIDADE
        while (fgets(locationLine, sizeof(locationLine), fp2))
        {
            cityTokenNoAccent = strtok(locationLine, ",");
            cityTokenNoAccent = strtok(NULL, ",");
            cityTokenNoAccent = strtok(NULL, ",");
            cityTokenNoAccent = strtok(NULL, ",");

            // VERIFICA SE TOKEN == INPUT - SE SIM, CIDADE EXISTE
            if (strcmp(userInput, cityTokenNoAccent) == 0)
            {
                fclose(fp2);
                return 1;
            }
            size++;
        }
    }

    fclose(fp2);
    return 0;
}

int orderPeopleInAlphabet(const void *p1, const void *p2)
{
    // CRIA PONTEIROS COM O STRUCT DE PESSOA
    Person *pa = (Person *)p1;
    Person *pb = (Person *)p2;

    // COMPARA O NOME DOS DOIS PONTEIROS E RETORNA 1 OU 0
    return strcmp(pa->Name, pb->Name);
}

void tranformStringToUpper(char *string)
{
    char caracter;
    int i;

    // USA FUNCAO TO UPPER PARA CADA CARACTER E ARMAZENA NOVA STRING EM ARRAY
    for (i = 0; i < strlen(string); i++)
    {
        caracter = string[i];
        string[i] = toupper(caracter);
    }
}

int calcAge(int yearBorn, int monthBorn, int dayBorn)
{
    // PEGA A DATA ATUAL
    time_t actualTime = time(NULL);
    struct tm date = *localtime(&actualTime);

    // CALCULA A IDADE DE ACORDO COM O ANO
    int age = (date.tm_year) - yearBorn;

    // Confere se o mes de nacimento é maior que o mes atual
    // Se for, é retirado 1 ano da idade
    if (monthBorn > date.tm_mon + 1)
    {
        age--;
    }
    // Se o mes de nascimento nao tiver chegado ainda, ou for esse mes  E
    // Se o dia de nascimento nao tiver chegado ainda, é retirado 1 ano da idade
    if (monthBorn <= date.tm_mon + 1 && dayBorn > date.tm_mday)
    {
        age--;
    }

    return age;
}

// 3.FUNCOES DO MENU DE OPCOES
// 3.1 REGISTRO DE PESSOA BENEFICIADA
void registerPerson()
{
    Person pw = {0};
    int validCpf = 0, cpfExists = 0;
    int ufCorresponds = 0;

    // ABRE O ARQUIVO
    FILE *writeFile = fopen("person.txt", "a");

    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(writeFile) == 1)
    {
        return;
    }

    fflush(stdin);

    // PEDE OS DADOS
    printf("-------------------- REGISTRO DE PESSOA --------------------\n");
    do
    {
        printf("CPF(Ex: 04285172186): ");
        scanf("%[^\n]", pw.Cpf);
        getchar(); // Remove \n e troca por um caracter nulo
        isCpfValid = validateCpf(pw.Cpf);
        if (isCpfValid == 0)
        {
            printf("%s", ASK_INFO_AGAIN);
            continue;
        }
        else
        {
            cpfExists = checkIfCpfIsRegistered(pw.Cpf);
            if (cpfExists == 1)
            {
                printf("%s%s", ERROR_CPF_EXISTS, ASK_FOR_ANOTHER);
            }
        }
    } while (isCpfValid == 0 || checkIfCpfIsRegistered(pw.Cpf) == 1);

    do
    {
        printf("Nome: ");
        fflush(stdin);
        scanf("%[^\n]s", pw.Name);
        tranformStringToUpper(pw.Name);
        isInputValid = checkIfInputIsValid(pw.Name, sizeof(pw.Name));
    } while (isInputValid != 0);

    do
    {
        printf("Sexo ('F' para Feminino OU 'M' para Masculino): ");
        fflush(stdin);
        scanf("%s", pw.Sex);
        tranformStringToUpper(pw.Sex);
        if (strcmp(pw.Sex, "F") != 0 && strcmp(pw.Sex, "M") != 0)
        {
            printf("%s%s", ERRORSEX, ASK_INFO_AGAIN);
        }
    } while (strcmp(pw.Sex, "F") != 0 && strcmp(pw.Sex, "M") != 0);

    do
    {
        printf("Data de Nascimento (dia/mes/ano - Ex: 01/02/2003):");
        scanf("%d/%d/%d", &pw.DayBorn, &pw.MonthBorn, &pw.yearBorn);

        isDateValid = checkIfDateValid(pw.DayBorn, pw.MonthBorn, pw.yearBorn);
        if (isDateValid != 1)
        {
            printf("\n%s%s", ERRORDATE, ASK_INFO_AGAIN);
        }
    } while (isDateValid != 1);

    do
    {
        fflush(stdin);
        printf("Cidade: ");
        scanf("%[^\n]", pw.City);
        getchar();
        tranformStringToUpper(pw.City);
        isInputValid = checkIfInputIsValid(pw.City, sizeof(pw.City));

        if (isInputValid == 0)
        {
            if (checkIfLocationExists(pw.City, 0) == 0)
            {
                printf("%s%s", ERROR_CITY, ASK_INFO_AGAIN);
            }
        }

    } while (isInputValid != 0 || checkIfLocationExists(pw.City, 0) == 0);

    do
    {
        printf("UF/Estado: ");
        // VERIFICA SE A UF DIGITADA TEM APENAS 2 DIGITOS
        fflush(stdin);
        scanf("%s", pw.Uf);
        tranformStringToUpper(pw.Uf);
        isInputValid = checkIfInputIsValid(pw.Uf, sizeof(pw.Uf) - 1);

        // CONFERE SE UF CORRESPONDE A CIDADE DIGITADA
        ufCorresponds = ufCorrespondsToCity(pw.City, pw.Uf);
        if (ufCorresponds != 0)
        {
            printf("%s%s", "A UF/Estado nao corresponde a cidade digitada!", ASK_INFO_AGAIN);
        }

        if (isInputValid == 0)
        {
            if (checkIfLocationExists(pw.Uf, 1) == 0)
            {
                printf("%s", ERROR_UF);
            }
        }
    } while (isInputValid != 0 || checkIfLocationExists(pw.Uf, 1) == 0 || ufCorresponds != 0);

    fflush(stdin);

    // SE PASSOU POR TODAS AS VALIDAÇÕES DOS INPUTS ACIMA, ESCREVE NO ARQUIVO
    fprintf(writeFile, "%s\n", pw.Cpf);
    fprintf(writeFile, "%s\n", pw.Name);
    fprintf(writeFile, "%s\n", pw.Sex);
    fprintf(writeFile, "%02d/%02d/%02d\n", pw.DayBorn, pw.MonthBorn, pw.yearBorn);
    fprintf(writeFile, "%s\n", pw.City);
    fprintf(writeFile, "%s\n", pw.Uf);
    clearScreen();
    printf("\n\n%s\n", SUCCESS_REGISTERED);

    // FECHA O ARQUIVO
    fclose(writeFile);
}
// 3.2 CONSULTA DE PESSOA BENEFICIADA
void consultPerson()
{
    Person pr = {0};
    char cpf[16];
    int cpfExists = 0;

    printf("-------------------- CONSULTA DE PESSOA --------------------\n");
    FILE *readFile = fopen("person.txt", "r");

    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(readFile) == 1)
    {
        return;
    }

    // CONFERE SE O ARQUIVO POSSUI DADOS
    if (checkIfFileHasRecords(readFile) == 0)
    {
        printf("%s", NO_RECORDS);
        fclose(readFile);
        return;
    }

    // PEDE O CPF E FAZ SUA VALIDACAO
    do
    {
        printf("Digite o CPF para consulta (Ex: 04285172186):");
        fflush(stdin);
        scanf("%[^\n]", cpf);
        getchar();
        isCpfValid = validateCpf(cpf);
        if (isCpfValid == 0)
        {
            printf("%s", ASK_INFO_AGAIN);
            continue;
        }
        else
        {
            // SE O CPF É VALIDO, CONFERE SE ELE EXISTE NA BASE DE DADOS
            cpfExists = checkIfCpfIsRegistered(cpf);
            // SE NAO EXISTE, PEDE OUTRO NOVAMENTE
            if (cpfExists == 0)
            {
                printf("%s%s", ERROR_CPF_NOT_EXISTS, ASK_FOR_ANOTHER);
            }
        }
    } while (isCpfValid == 0 || checkIfCpfIsRegistered(cpf) != 1);

    // FAZ A LEITURA DE TODOS OS DADOS DO ARQUIVO ENQUANTO NAO FOR O FIM (EOF - END OF FILE)
    while (fscanf(readFile,
                  "%s\n%[^\n]\n%s\n%d/%d/%d\n%[^\n]\n%[^\n]",
                  pr.Cpf, pr.Name, pr.Sex, &pr.DayBorn, &pr.MonthBorn, &pr.yearBorn, pr.City, pr.Uf) != EOF)
    {
        // QUANDO ECONTRA O CPF PROCURADO, MOSTRA SEUS DADOS E VOLTA AO MENU
        if (strcmp(pr.Cpf, cpf) == 0)
        {
            printf("\n");
            printf("---- DADOS DE: %s ----\n", pr.Name);
            printf("---- CPF:               %s \n", pr.Cpf);
            printf("---- SEXO:              %s \n", pr.Sex);
            printf("---- DT. NASCIMENTO:    %d/%d/%d \n", pr.DayBorn, pr.MonthBorn, pr.yearBorn);
            printf("---- CIDADE:            %s \n", pr.City);
            printf("---- ESTADO/UF          %s \n", pr.Uf);
            printf("\n\n");
        }
    };

    fclose(readFile);
}
// 3.3 LISTAR PESSOAS POR CIDADE
void listPeopleByCity()
{
    int j = 0;
    int numberOfPeople = 0;
    int hasSomeoneInCity = 0;
    int ufCorresponds = 0;
    Person personByCityStruct;
    Person listOfPeople[200];

    printf("----------------- LISTAGEM POR CIDADE -----------------\n");

    FILE *readFile = fopen("person.txt", "r");
    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(readFile) == 1)
    {
        return;
    }

    // CONFERE SE O ARQUIVO POSSUI DADOS
    if (checkIfFileHasRecords(readFile) == 0)
    {
        printf("%s", NO_RECORDS);
        fclose(readFile);
        return;
    }

    Locations city;

    // PEDE NOME DA CIDADE e UF E VERIFICA SE ELA EXISTE ATE QUE SEJA DIGITADA UMA CIDADE EXISTENTE
    do
    {
        fflush(stdin);
        printf("Cidade: ");
        scanf("%[^\n]", city.Name);
        getchar();
        tranformStringToUpper(city.Name);
        isInputValid = checkIfInputIsValid(city.Name, sizeof(city.Name));

        if (isInputValid == 0)
        {
            if (checkIfLocationExists(city.Name, 0) == 0)
            {
                printf("%s%s", ERROR_CITY, ASK_INFO_AGAIN);
            }
        }

    } while (isInputValid != 0 || checkIfLocationExists(city.Name, 0) == 0);

    do
    {
        printf("UF/Estado: ");
        // VERIFICA SE A UF DIGITADA TEM APENAS 2 DIGITOS
        fflush(stdin);
        scanf("%s", city.Uf);
        tranformStringToUpper(city.Uf);
        isInputValid = checkIfInputIsValid(city.Uf, sizeof(city.Uf) - 1);

        // CONFERE SE UF CORRESPONDE A CIDADE DIGITADA
        ufCorresponds = ufCorrespondsToCity(city.Name, city.Uf);
        if (ufCorresponds != 0)
        {
            printf("%s%s", "A UF/Estado nao corresponde a cidade digitada!", ASK_INFO_AGAIN);
        }

        if (isInputValid == 0)
        {
            if (checkIfLocationExists(city.Uf, 1) == 0)
            {
                printf("%s", ERROR_UF);
            }
        }
    } while (isInputValid != 0 || checkIfLocationExists(city.Uf, 1) == 0 || ufCorresponds != 0);

    printf("\n-------------- LISTA DE PESSOAS EM %s -------------", city.Name);

    // FAZ A LEITURA DOS DADOS NA BASE
    // COMPARA A CIDADE DE CADA PESSOA COM CADA LINHA DO ARQUIVO DE CIDADES
    while (fscanf(readFile,
                  "%s\n%[^\n]\n%s\n%d/%d/%d\n%[^\n]\n%[^\n]",
                  personByCityStruct.Cpf,
                  personByCityStruct.Name,
                  personByCityStruct.Sex,
                  &personByCityStruct.DayBorn,
                  &personByCityStruct.MonthBorn,
                  &personByCityStruct.yearBorn,
                  personByCityStruct.City,
                  personByCityStruct.Uf) != EOF)
    {

        // SE O NOME DA CIDADE É IGUAL, A PESSOA CORRESPONDENTE É SALVA NO ARRAY DE STRUCTS - personByCityStruct
        if (strcmp(city.Name, personByCityStruct.City) == 0 && strcmp(city.Uf, personByCityStruct.Uf) == 0)
        {
            hasSomeoneInCity = 1;
            listOfPeople[numberOfPeople] = personByCityStruct;
            numberOfPeople++;
        }
    }

    // SE NAO TEM NENHUM REGISTRO CORRESPONDENTE À CIDADE, ERRO E VOLTA AO MENU
    if (hasSomeoneInCity == 0)
    {
        printf("\n%s\n", ERROR_NOBODY_IN_CITY);
        return;
    }

    // ORDENA OS REGISTRO EM ORDEM ALFABETICA DE NOME
    qsort(listOfPeople, numberOfPeople, sizeof(Person), orderPeopleInAlphabet);

    // MOSTRA A LISTA DE REGISTROS CORRESPONDENTES À CIDADE
    for (j = 0; j < numberOfPeople; j++)
    {
        printf("\n%s   |   %s", listOfPeople[j].Cpf, listOfPeople[j].Name);
    }

    printf("\n");
    fclose(readFile);
}

// 3.4 GERAR RELATORIO DE PESSOAS BENEFICIADAS
void generateReport()
{
    int registeredPeople = 0;
    int personAge = 0;
    int et15 = 0, et16 = 0, et30 = 0, et50 = 0, et60 = 0;
    int fem = 0, masc = 0;
    float p15 = 0, p16 = 0, p30 = 0, p50 = 0, p60 = 0;
    float pM = 0, pF = 0;
    Person personData;

    printf("%s", REPORT_INIT);

    FILE *readFile = fopen("person.txt", "r");
    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(readFile) == 1)
    {
        return;
    }

    // CONFERE SE O ARQUIVO POSSUI DADOS
    if (checkIfFileHasRecords(readFile) == 0)
    {
        printf("%s", NO_RECORDS);
        fclose(readFile);
        return;
    }

    // FAZ A LEITURA DE TODOS OS DADOS DO ARQUIVO DE PESSOA
    while (fscanf(readFile,
                  "%s\n%[^\n]\n%s\n%d/%d/%d\n%[^\n]\n%[^\n]",
                  personData.Cpf,
                  personData.Name,
                  personData.Sex,
                  &personData.DayBorn,
                  &personData.MonthBorn,
                  &personData.yearBorn,
                  personData.City,
                  personData.Uf) != EOF)
    {
        // CLASSIFICA A PESSOA POR IDADE DE ACORDO COM DATA DE NASCIMENTO
        personAge = calcAge(personData.yearBorn - 1900, personData.MonthBorn, personData.DayBorn);
        if (personAge <= 15)
        {
            et15 += 1;
        }

        if (personAge >= 16 && personAge <= 29)
        {
            et16 += 1;
        }

        if (personAge >= 30 && personAge <= 49)
        {
            et30 += 1;
        }

        if (personAge >= 50 && personAge <= 60)
        {
            et50 += 1;
        }

        if (personAge > 60)
        {
            et60 += 1;
        }

        // CLASSIFICA PESSOA DE ACORDO COM O SEXO
        if (strcmp(personData.Sex, "F") == 0)
        {
            fem++;
        }
        if (strcmp(personData.Sex, "M") == 0)
        {
            masc++;
        }

        // CONTA QUANTAS PESSOAS HA NO ARQUIVO
        registeredPeople++;
    }

    printf("TOTAL PESSOAS BENEFICIADAS: %d\n\n", registeredPeople);

    // CALCULA A PORCENTAGEM POR FAIZA ETARIA
    p15 = ((float)et15 / registeredPeople) * 100;
    p16 = ((float)et16 / registeredPeople) * 100;
    p30 = ((float)et30 / registeredPeople) * 100;
    p50 = ((float)et50 / registeredPeople) * 100;
    p60 = ((float)et60 / registeredPeople) * 100;

    printf("%s", PERCENT_BY_AGE);

    printf("0 a 15 : %.2f%%\n", p15);
    printf("16 a 29 : %.2f%%\n", p16);
    printf("30 a 49 : %.2f%%\n", p30);
    printf("50 a 60 : %.2f%%\n", p50);
    printf("60+ : %.2f%%\n", p60);

    // CALCULA A PORCENTAGEM POR SEXO
    printf("\n%s", PERCENT_BY_SEX);
    pM = ((float)masc / registeredPeople) * 100;
    pF = ((float)fem / registeredPeople) * 100;

    printf("FEMININO: %.2f%%\n", pF);
    printf("MASCULINO: %.2f%%\n", pM);

    fclose(readFile);
}

// 3.5 REMOVER REGISTRO
void removeRecord()
{
    Person pRm;
    char cpf[16];
    char confirm[2];
    int cpfExists = 0;

    printf("%s\n", REMOVE_INIT);
    FILE *primaryFile = fopen("person.txt", "r");
    FILE *tempFile = fopen("personTemp.txt", "ab");

    // CHECAR SE O ARQUIVO ABRIU, SE NAO - VOLTA AO MENU
    if (isFileOpen(primaryFile) == 1 || isFileOpen(tempFile) == 1)
    {
        return;
    }

    // CONFERE SE O ARQUIVO POSSUI DADOS
    if (checkIfFileHasRecords(primaryFile) == 0)
    {
        printf("%s", NO_RECORDS);
        fclose(primaryFile);
        fclose(tempFile);
        remove("personTemp.txt");
        return;
    }

    // PEDE O CPF E FAZ SUA VALIDACAO
    do
    {
        printf("Digite o CPF da pessoa para exclusao: ");
        fflush(stdin);
        scanf("%[^\n]", cpf);
        getchar();
        isCpfValid = validateCpf(cpf);
        if (isCpfValid == 0)
        {
            printf("\n%s", ASK_INFO_AGAIN);
            continue;
        }
        else
        {
            cpfExists = checkIfCpfIsRegistered(cpf);
            if (cpfExists != 1)
            {
                printf("%s%s", ERROR_CPF_NOT_EXISTS, ASK_FOR_ANOTHER);
            }
        }
    } while (isCpfValid == 0 || cpfExists != 1);

    // FAZ A LEITURA DE TODOS OS DADOS DO ARQUIVO INICIAL
    while (fscanf(primaryFile,
                  "%s\n%[^\n]\n%s\n%d/%d/%d\n%[^\n]\n%[^\n]",
                  pRm.Cpf, pRm.Name, pRm.Sex, &pRm.DayBorn, &pRm.MonthBorn, &pRm.yearBorn, pRm.City, pRm.Uf) != EOF)
    {
        // SE O CPF É DIFERENTE DO QUE DEVE EXCLUIR, OS DADOS SAO ESCRITOS EM UM ARQUIVO TEMPORARIO
        if (strcmp(pRm.Cpf, cpf) != 0)
        {
            fprintf(tempFile, "%s\n", pRm.Cpf);
            fprintf(tempFile, "%s\n", pRm.Name);
            fprintf(tempFile, "%s\n", pRm.Sex);
            fprintf(tempFile, "%02d/%02d/%02d\n", pRm.DayBorn, pRm.MonthBorn, pRm.yearBorn);
            fprintf(tempFile, "%s\n", pRm.City);
            fprintf(tempFile, "%s\n", pRm.Uf);
        }
        else
        {
            // SE O CPF É O QUE DEVE SER EXCLUIDO, PEDE CONFIRMACAO PARA EXCLUSAO
            do
            {
                printf("\n%s   |   %s\n", pRm.Cpf, pRm.Name);
                printf("%s", CONFIRM_EXCLUDE);
                fflush(stdin);
                scanf("%[^\n]", confirm);
                tranformStringToUpper(confirm);

                // VERIFICA SE O INPUT É N OU S, ATE QUE SEJA DIGITADA UMA DAS DUAS LETRAS
                if (strcmp(confirm, "N") != 0 && strcmp(confirm, "S") != 0)
                {
                    printf("Opcao invalida, deve ser digitado S ou N\n");
                }
            } while (strcmp(confirm, "N") != 0 && strcmp(confirm, "S") != 0);

            // SE NA CONFIRMACAO FOR DIGITADO "N", VOLTA AO MENU E EXLUI O TEMP
            if (strcmp(confirm, "N") == 0)
            {
                fclose(primaryFile);
                fclose(tempFile);
                remove("personTemp.txt");
                return;
            }
        }
    };

    fclose(primaryFile);
    fclose(tempFile);

    // REMOVE O ARQUIVO INICIAL
    int removeResult = remove("person.txt");
    // RENOMEIA O TEMPORARIO COM O NOME DO INICIAL
    int renameResult = rename("personTemp.txt", "person.txt");

    // SE A REMOCAO OU A RENOMEACAO DEREM ERRADO OU CERTO, É MOSTRADO NA TELA
    if (removeResult != 0 || renameResult != 0)
    {
        printf("\n%s\n", REMOVE_ERROR);
    }
    else
    {
        printf("\n%s\n", REMOVE_SUCCESS);
    }
}
