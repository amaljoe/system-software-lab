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
        char str1[MAX], str2[MAX], str3[MAX];

        //HEX and number operations
        //itoa(n,str,10); for integer to ascii (string) with base 10
        //n = atoi(str) for string to int <stdlib.h>
        //sprintf(str, "%x", n); int to hex string (alt method)

        fputs(str, outputFile);
        // sscanf splits the string as tokens
        // or use fscanf directly
        sscanf(str, "%s\t%s\t%s", str1, str2, str3);
        printf("%s | %s | %s\n", str1, str2, str3);
    }
    return 0;
}