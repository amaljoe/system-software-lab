#include <stdio.h>
#define MAX 50
#define MAXLINES 150

struct
{
    char label[MAX];
    char op[MAX];
    char operand[MAX];
} input[MAXLINES];

// a program doing minimal IO operations. Use for reference.
int main()
{
    int locctr;
    char str[MAX];
    FILE *inputFile = fopen("sample_input.txt", "r");
    FILE *interFile = fopen("intermediate.txt", "w");
    int i = 0;
    // check first line
    fgets(str, MAX, inputFile);
    sscanf(str, "%s\t%s\t%s", input[i].label, input[i].op, input[i].operand);
    // initialise location counter to starting address
    locctr = input[i].operand;
    fputs(str, interFile);
    while (fgets(str, MAX, inputFile) != NULL)
    {
        i++;
        sscanf(str, "%s\t%s\t%s", input[i].label, input[i].op, input[i].operand);
        i++;
        //printf("%s", str);
    }
    int n = 4096;
    printf("%x\n", n);
    return 0;
}