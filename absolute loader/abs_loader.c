#include <stdio.h>

#define MAX 100

int main()
{
    char str[MAX];
    FILE *inputFile = fopen("objcode.txt", "r");
    int location = 0;
    printf("Program loaded into the memory with following addresses:\n");
    while (fgets(str, MAX, inputFile) != NULL)
    {
        // extract starting address from header record
        if (str[0] == 'H')
        {
            sscanf(str + 9, "%x", &location);
            continue;
        }
        // stop if end record reached
        if (str[0] == 'E')
            break;

        int startingAddress;
        // extract starting address from text record
        sscanf(str, "T%x", &startingAddress);
        // put xx for empty locations
        while (location < startingAddress)
        {
            printf("%x xx\n", location++);
        }
        // point ptr to start of object code in text record
        char *ptr = str + 12;
        for (int i = 0; ptr[i] != '\0' && ptr[i] != '\n'; i += 2)
        {
            // skip white spaces
            if (ptr[i] == ' ')
                i++;
            printf("%x %c%c\n", location++, ptr[i], ptr[i + 1]);
        }
    }
    return 0;
}