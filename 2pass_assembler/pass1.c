#include <stdio.h>
#include <string.h>

#define MAX 50
#define MAXLINES 150

struct line
{
    char loc[MAX];
    char label[MAX];
    char op[MAX];
    char operand[MAX];
} lines[MAXLINES];

// number of lines
int num = 0;

void writeLine(struct line *, FILE *);

// a program doing minimal IO operations. Use for reference.
int main()
{
    int locctr;
    char str[MAX];
    FILE *inputFile = fopen("sample_input.txt", "r");
    FILE *interFile = fopen("intermediate.txt", "w");
    // check first line
    fgets(str, MAX, inputFile);
    sscanf(str, "%s\t%s\t%s", lines[num].label, lines[num].op, lines[num].operand);
    // initialise location counter to starting address
    sscanf(lines[num].operand, "%x", &locctr);
    strcpy(lines[num].loc, lines[num].operand);
    writeLine(&lines[num], interFile);
    while (fgets(str, MAX, inputFile) != NULL)
    {
        num++;
        sscanf(str, "%s\t%s\t%s", lines[num].label, lines[num].op, lines[num].operand);
        sprintf(lines[num].loc, "%x", locctr);
        locctr += 3;
        writeLine(&lines[num], interFile);
    }
    printf("%d, %x\n", locctr, locctr);
    return 0;
}

void writeLine(struct line *myline, FILE *file)
{
    char lineStr[MAX];
    sprintf(lineStr, "%s\t%s\t%s\t%s\n", myline->loc, myline->label, myline->op, myline->operand);
    fputs(lineStr, file);
}