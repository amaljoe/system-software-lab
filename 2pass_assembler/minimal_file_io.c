#include <stdio.h>
#define MAX 50

// a program doing minimal IO operations. Use for reference.
int main()
{
    char str[MAX];
    FILE *inputFile = fopen("sample_input.txt", "r");
    FILE *outputFile = fopen("sample_output.txt", "w");
    while (fgets(str, MAX, inputFile) != NULL)
    {
        fputs(str, outputFile);
        printf("%s", str);
    }
    return 0;
}