#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 200
#define MAXLINES 250

struct LINE
{
    char loc[MAX];
    char label[MAX];
    char op[MAX];
    char operand[MAX];
};

struct
{
    char op[MAX];
    char opcode[MAX];
} optab[MAXLINES];

struct NODE
{
    char address[MAX];
    struct NODE *next;
};

struct SYMBOL
{
    char sym[MAX];
    char loc[MAX];
    struct NODE *nodes;
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
// input line currently working on
struct LINE *currentLine;
// text record currently working on
struct TEXTRECORD *currentRecord;
// location counter
int locctr = 0;

FILE *inputFile;
FILE *recordFile;
FILE *symtabFile;

char *padZero(char[], int);
void readOptab();
struct SYMBOL *searchSymtab(char[]);
void putSym(char[], char[]);
void putSymNode(char[], char[]);
char *getOpcode(char[]);
void writeCurrentRecord();

int main()
{
    char startingAddress[MAX] = "0";
    char pgmName[MAX] = "";
    char str[MAX];

    currentLine = (struct LINE *)malloc(sizeof(struct LINE));

    recordFile = fopen("record.txt", "w");
    symtabFile = fopen("symtab.txt", "w");
    inputFile = fopen("input.txt", "r");

    // store optab in memory
    readOptab();
    // check first line
    fgets(str, MAX, inputFile);
    sscanf(str, "%s\t%s\t%s", currentLine->label, currentLine->op, currentLine->operand);
    if (strcmp(currentLine->op, "START") == 0)
    {
        // initialise location counter to starting address
        strcpy(startingAddress, currentLine->operand);
        strcpy(pgmName, currentLine->label);
        if (strcmp(currentLine->label, "**") != 0)
        {
            putSym(currentLine->label, currentLine->operand);
        }
    }
    else
    {
        strcpy(startingAddress, "0");
        // reset file pointer to start
        rewind(inputFile);
    }
    sscanf(startingAddress, "%X", &locctr);

    // write header record
    sprintf(str, "H %s %s\n", padZero(pgmName, 6), padZero(startingAddress, 6));
    fputs(str, recordFile);

    // initialise first text record
    currentRecord = (struct TEXTRECORD *)malloc(sizeof(struct TEXTRECORD));
    currentRecord->length = 0;
    strcpy(currentRecord->startingAddress, startingAddress);
    strcpy(currentRecord->text, "");

    // loop through all instructions in intermediate file
    while (fgets(str, MAX, inputFile) != NULL)
    {
        sscanf(str, "%s\t%s\t%s", currentLine->label, currentLine->op, currentLine->operand);
        sprintf(currentLine->loc, "%X", locctr);

        // if symbol is present, put in symtab
        if (strcmp(currentLine->label, "**") != 0)
        {
            putSym(currentLine->label, currentLine->loc);
        }
        if (getOpcode(currentLine->op) != NULL)
        {
            // opcode found in optab
            locctr += 3;
        }
        else if (strcmp(currentLine->op, "WORD") == 0)
        {
            locctr += 3;
        }
        else if (strcmp(currentLine->op, "BYTE") == 0)
        {
            int c = strlen(currentLine->operand) - 3;
            if (currentLine->operand[0] == 'X')
            {
                // hexadecimal operand
                locctr += ceil(c / 2);
            }
            else
            {
                // character operand
                locctr += c;
            }
        }
        else if (strcmp(currentLine->op, "RESW") == 0)
        {
            int size = atoi(currentLine->operand);
            locctr += size * 3;
        }
        else if (strcmp(currentLine->op, "RESB") == 0)
        {
            int size = atoi(currentLine->operand);
            locctr += size;
        }
        else if (strcmp(currentLine->op, "END") == 0)
        {
            // stop when END is found
            break;
        }
        else
        {
            printf("Invalid instruction: %s\n", str);
            return 1;
        }

        // if text record will exceed max length, write it and initialise new text record
        int sAddress;
        sscanf(currentRecord->startingAddress, "%X", &sAddress);
        if (locctr - sAddress > 30)
        {
            writeCurrentRecord();
        }

        char *opcode = getOpcode(currentLine->op);
        char address[MAX] = "0";
        bool indexed = false;
        if (opcode != NULL)
        {
            // op found in optab
            if (strcmp(currentLine->operand, "**") != 0)
            {
                // operand present in instruction
                char *operand = currentLine->operand;
                char *comma = strchr(operand, ',');
                indexed = comma != NULL;
                if (indexed)
                {
                    // operand is before ','
                    *comma = '\0';
                }
                struct SYMBOL *ref = searchSymtab(operand);
                if (ref == NULL)
                {
                    // symbol not found in symtab
                    char symLoc[MAX];
                    sprintf(symLoc, "%X", locctr - 2);
                    putSymNode(currentLine->operand, symLoc);
                }
                else
                {
                    if (strcmp(ref->loc, "**") == 0)
                    {
                        // symbol address not found in symtab
                        char symLoc[MAX];
                        sprintf(symLoc, "%X", locctr - 2);
                        putSymNode(currentLine->operand, symLoc);
                    }
                    else
                    {
                        strcpy(address, ref->loc);
                    }
                }
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
            strcat(currentRecord->text, str);
            currentRecord->length += 3;
        }
        else if (strcmp(currentLine->op, "BYTE") == 0)
        {
            // get the constant part
            strcpy(str, currentLine->operand + 2);
            str[strlen(str) - 1] = '\0';
            int size = ceil(strlen(str) / 2);
            // if character operand, convert to ASCII value
            if (currentLine->operand[0] == 'C')
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
            strcat(currentRecord->text, str);
            currentRecord->length += size;
        }
        if (strcmp(currentLine->op, "WORD") == 0)
        {
            strcpy(str, currentLine->operand);
            int num = atoi(str);
            sprintf(str, "%X", num);
            padZero(str, 6);

            strcat(str, " ");
            strcat(currentRecord->text, str);
            currentRecord->length += 3;
        }

        if (strcmp(currentLine->op, "RESW") == 0 || strcmp(currentLine->op, "RESB") == 0)
        {
            writeCurrentRecord();
        }

        printf("%s\t%s\t%s\t%s\n", currentLine->loc, currentLine->label, currentLine->op, currentLine->operand);
    }
    // write last text record
    writeCurrentRecord();
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

// returns SYMBOL entry for given symbol or null if not found
struct SYMBOL *searchSymtab(char sym[])
{
    for (int i = 0; i < symtabLength; i++)
    {
        if (strcmp(sym, symtab[i].sym) == 0)
        {
            return &symtab[i];
        }
    }
    return NULL;
}

void putSymNode(char label[], char loc[])
{
    struct SYMBOL *ref = searchSymtab(label);
    // add new entry in symtab if symbol not found
    if (ref == NULL)
    {
        strcpy(symtab[symtabLength].loc, "**");
        strcpy(symtab[symtabLength].sym, label);
        symtab[symtabLength].nodes = (struct NODE *)malloc(sizeof(struct NODE));
        strcpy(symtab[symtabLength].nodes->address, "");
        symtab[symtabLength].nodes->next = NULL;
        ref = &symtab[symtabLength];
        symtabLength++;
    }
    // add address as last node
    struct NODE *last = ref->nodes;
    while (last->next != NULL)
        last = last->next;

    last->next = (struct NODE *)malloc(sizeof(struct NODE));
    last = last->next;
    strcpy(last->address, loc);
    last->next = NULL;
}

// read optab
void readOptab()
{
    FILE *optabFile = fopen("optab.txt", "r");
    char str[MAX];

    while (fgets(str, MAX, optabFile) != NULL)
    {
        sscanf(str, "%s\t%s", optab[optabLength].op, optab[optabLength].opcode);
        optabLength++;
    }
}

void putSym(char label[], char loc[])
{
    char symStr[MAX];
    struct SYMBOL *ref = searchSymtab(label);
    if (ref != NULL)
    {
        // symbol already present in symtab
        strcpy(ref->loc, loc);
        struct NODE *next = ref->nodes->next;
        while (next != NULL)
        {
            writeCurrentRecord();
            strcpy(currentRecord->startingAddress, next->address);
            currentRecord->length += 2;
            strcpy(currentRecord->text, ref->loc);
            writeCurrentRecord();
            next = next->next;
        }
    }
    else
    {
        // symbol not found in symtab
        strcpy(symtab[symtabLength].loc, loc);
        strcpy(symtab[symtabLength].sym, label);
        symtabLength++;
    }
    sprintf(symStr, "%s\t%s\n", label, loc);
    fputs(symStr, symtabFile);
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

void writeCurrentRecord()
{

    if (currentRecord->length == 0)
    {
        return;
    }
    char lineStr[MAX];
    char len[MAX];
    sprintf(len, "%X", currentRecord->length);
    sprintf(lineStr, "T %s %s %s\n", padZero(currentRecord->startingAddress, 6), padZero(len, 2), currentRecord->text);
    fputs(lineStr, recordFile);

    // initialise new text record
    currentRecord->length = 0;
    sprintf(currentRecord->startingAddress, "%X", locctr);
    strcpy(currentRecord->text, "");
}