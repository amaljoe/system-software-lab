#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX 200
#define MAXLINES 250

struct line
{
    char loc[MAX];
    char label[MAX];
    char op[MAX];
    char operand[MAX];
} lines[MAXLINES];

struct
{
    char op[MAX];
    char opcode[MAX];
} optab[MAXLINES];

struct
{
    char sym[MAX];
    char loc[MAX];
} symtab[MAXLINES];

struct TEXTRECORD
{
    char startingAddress[MAX];
    int length;
    char text[MAX];
};

// number of lines
int num = 0;
// length of optab
int optabLength = 0;
// length of symtab
int symtabLength = 0;
// length of program in hexadecimal
char prolen[MAX];

char *padZero(char[]);
void readInputs();
char *searchSymtab(char[]);
char *getOpcode(char[]);

int main()
{
    int locctr;
    char startingAddress[MAX] = "0";
    char pgmName[MAX] = "";
    char str[MAX];

    FILE *recordFile = fopen("record.txt", "w");
    // store symtab, optab, intermediate file and program length in memory
    readInputs();
    int i = 0;
    if (strcmp(lines[i].op, "START") == 0)
    {
        strcpy(startingAddress, lines[i].operand);
        strcpy(pgmName, lines[i].label);
        i++;
    }
    // write header record
    sprintf(str, "H %s %s %s", padZero(pgmName), padZero(startingAddress), padZero(prolen));
    fputs(str, recordFile);
    // loop through all instructions in intermediate file
    struct TEXTRECORD *current;
    for (; i < num; i++)
    {
        char *opcode = getOpcode(lines[i].op);
        char *address = searchSymtab(lines[i].operand);
        strcpy(str, opcode);
        strcat(str, address);
        printf("%s\n", str);
    }
    printf("Pass 2 of 2 completed");
    return 0;
}

// get opcode for given op or returns null if not found
char *getOpcode(char op[])
{
    for (int i = 0; i < optabLength; i++)
    {
        if (strcmp(op, optab[i].op) == 0)
        {
            return optab[i].opcode;
        }
    }
    printf("Opcode not found for %s\n", op);
    return NULL;
}

// returns address for given symbol or null if not found
char *searchSymtab(char sym[])
{
    for (int i = 0; i < symtabLength; i++)
    {
        if (strcmp(sym, symtab[i].sym) == 0)
        {
            return symtab[i].loc;
        }
    }
    printf("Address not found for %s\n", sym);
    return NULL;
}

// read symtab, optab, intermediate file and program length
void readInputs()
{
    FILE *optabFile = fopen("optab.txt", "r");
    char str[MAX];

    while (fgets(str, MAX, optabFile) != NULL)
    {
        sscanf(str, "%s\t%s", optab[optabLength].op, optab[optabLength].opcode);
        optabLength++;
    }

    FILE *interFile = fopen("intermediate.txt", "r");
    while (fgets(str, MAX, interFile) != NULL)
    {
        sscanf(str, "%s\t%s\t%s\t%s", lines[num].loc, lines[num].label, lines[num].op, lines[num].operand);
        num++;
    }

    FILE *symtabFile = fopen("symtab.txt", "r");
    while (fgets(str, MAX, symtabFile) != NULL)
    {
        sscanf(str, "%s\t%s", symtab[symtabLength].sym, symtab[symtabLength].loc);
        symtabLength++;
    }

    FILE *lenFile = fopen("prolen.txt", "r");
    fgets(prolen, MAX, lenFile);
}

// pad zeroes to left or trim to make string length 6
char *padZero(char text[])
{
    char zeroes[] = "000000";
    // put required no of zeroes before text
    int c = strlen(text) > 6 ? 0 : strlen(text) - 6;
    zeroes[c] = '\0';
    strcat(zeroes, text);
    strcpy(text, zeroes);
    // truncate to 6 characters
    text[6] = '\0';
    return text;
}

void writeTextRecord(struct line *myline, FILE *file)
{
    char lineStr[MAX];
    sprintf(lineStr, "%s\t%s\t%s\t%s\n", myline->loc, myline->label, myline->op, myline->operand);
    fputs(lineStr, file);
}