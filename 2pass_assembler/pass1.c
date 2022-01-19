#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX 50
#define MAXLINES 150

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

// number of lines
int num = 0;
// length of optab
int optabLength = 0;

void writeLine(struct line *, FILE *);
void readOptab();
char *getOpcode(char[]);

// a program doing minimal IO operations. Use for reference.
int main()
{
    int locctr;
    char str[MAX];
    FILE *inputFile = fopen("sample_input.txt", "r");
    FILE *interFile = fopen("intermediate.txt", "w");

    // read and store optab in memory
    readOptab();

    // check first line
    fgets(str, MAX, inputFile);
    sscanf(str, "%s\t%s\t%s", lines[num].label, lines[num].op, lines[num].operand);
    // initialise location counter to starting address
    sscanf(lines[num].operand, "%X", &locctr);
    strcpy(lines[num].loc, lines[num].operand);
    writeLine(&lines[num], interFile);
    num++;
    while (fgets(str, MAX, inputFile) != NULL)
    {
        sscanf(str, "%s\t%s\t%s", lines[num].label, lines[num].op, lines[num].operand);
        sprintf(lines[num].loc, "%X", locctr);
        if (getOpcode(lines[num].op) != NULL)
        {
            locctr += 3;
        }
        else if (strcmp(lines[num].op, "WORD") == 0)
        {
            locctr += 3;
        }
        else if (strcmp(lines[num].op, "BYTE") == 0)
        {
            int c = strlen(lines[num].operand) - 3;
            if (lines[num].operand[0] == 'X')
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
        else if (strcmp(lines[num].op, "RESW") == 0)
        {
            int size = atoi(lines[num].operand);
            printf("size %d\n", size);
            locctr += size * 3;
        }
        else if (strcmp(lines[num].op, "RESB") == 0)
        {
            int size = atoi(lines[num].operand);
            printf("size %d\n", size);
            locctr += size;
        }
        else if (strcmp(lines[num].op, "END") == 0)
        {
            // stop when END is found
            writeLine(&lines[num], interFile);
            break;
        }
        else
        {
            printf("Invalid instruction: %s\n", str);
            return 1;
        }
        writeLine(&lines[num], interFile);
        num++;
    }

    printf("%x\n", locctr);
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

void writeLine(struct line *myline, FILE *file)
{
    char lineStr[MAX];
    sprintf(lineStr, "%s\t%s\t%s\t%s\n", myline->loc, myline->label, myline->op, myline->operand);
    fputs(lineStr, file);
}