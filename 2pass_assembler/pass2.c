#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

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

char *padZero(char[], int);
void readInputs();
char *searchSymtab(char[]);
char *getOpcode(char[]);
void writeTextRecord(struct TEXTRECORD *, FILE *);

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
    sprintf(str, "H %s %s %s\n", padZero(pgmName, 6), padZero(startingAddress, 6), padZero(prolen, 6));
    fputs(str, recordFile);

    // initialise first text record
    struct TEXTRECORD *current = (struct TEXTRECORD *)malloc(sizeof(struct TEXTRECORD));
    current->length = 0;
    strcpy(current->startingAddress, lines[i].loc);
    strcpy(current->text, "");

    // loop through all instructions in intermediate file
    for (; i < num; i++)
    {
        if (strcmp(lines[i].op, "END") == 0)
        {
            break;
        }
        char *opcode = getOpcode(lines[i].op);
        char *address = "0";
        bool indexed = false;

        if (opcode != NULL)
        {
            // op found in optab
            if (strcmp(lines[i].operand, "**") != 0)
            {
                // operand present in instruction
                char *operand = lines[i].operand;
                char *comma = strchr(operand, ',');
                indexed = comma != NULL;
                if (indexed)
                {
                    // operand is before ','
                    *comma = '\0';
                }
                address = searchSymtab(operand);
            }
            if (address == NULL)
            {
                printf("Address not found for %s\n", lines[i].operand);
                return 1;
            }
            padZero(address, 4);
            // x = 1 in objcode if indexed
            if (indexed)
            {
                char tmp[MAX];
                strcpy(tmp, address);
                tmp[1] = '\0';
                int bit = atoi(tmp);
                bit += 8;
                sprintf(tmp, "%X", bit);
                address[0] = tmp[0];
            }
            strcpy(str, opcode);
            strcat(str, address);
            strcat(str, " ");
            strcat(current->text, str);
            current->length += 3;
        }
        else if (strcmp(lines[i].op, "BYTE") == 0)
        {
            // get the constant part
            strcpy(str, lines[i].operand + 2);
            str[strlen(str) - 1] = '\0';
            int size = ceil(strlen(str) / 2);
            // if character operand, convert to ASCII value
            if (lines[i].operand[0] == 'C')
            {
                size = strlen(str);
                char temp[MAX] = "";
                int index = 0;
                while (str[index] != '\0')
                {
                    char buffer[MAX];
                    sprintf(buffer, "%X", str[index]);
                    strcat(temp, buffer);
                    index++;
                }
                strcpy(str, temp);
            }

            strcat(str, " ");
            strcat(current->text, str);
            current->length += size;
        }
        if (strcmp(lines[i].op, "WORD") == 0)
        {
            strcpy(str, lines[i].operand);
            int num = atoi(str);
            sprintf(str, "%X", num);
            padZero(str, 6);

            strcat(str, " ");
            strcat(current->text, str);
            current->length += 3;
        }
        // if text record length exceeded, write it and initialise new text record
        if (current->length > 30)
        {
            writeTextRecord(current, recordFile);
            current->length = 0;
            strcpy(current->startingAddress, lines[i + 1].loc);
            strcpy(current->text, "");
        }
    }
    // write last text record
    if (current->length > 0)
        writeTextRecord(current, recordFile);
    // write end record
    sprintf(str, "E %s", startingAddress);
    fputs(str, recordFile);
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

// pad zeroes to left or trim to fit length
char *padZero(char text[], int length)
{
    char zeroes[] = "000000";
    // put required no of zeroes before text
    int c = strlen(text) > length ? 0 : length - strlen(text);
    zeroes[c] = '\0';
    strcat(zeroes, text);
    strcpy(text, zeroes);
    // truncate to fit length
    text[length] = '\0';
    return text;
}

void writeTextRecord(struct TEXTRECORD *record, FILE *file)
{
    char lineStr[MAX];
    char len[MAX];
    sprintf(len, "%X", record->length);
    sprintf(lineStr, "T %s %s %s\n", padZero(record->startingAddress, 6), padZero(len, 2), record->text);
    fputs(lineStr, file);
}