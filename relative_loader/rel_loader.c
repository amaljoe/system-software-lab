#include <stdio.h>
#include <string.h>

#define MAX 100

int main()
{
    char str[MAX];
    FILE *inputFile = fopen("objcode.txt", "r");
    int location = 0, reloc = 0;
    printf("Enter relocation address:\n");
    scanf("%x", &reloc);
    printf("Program loaded into the memory with following addresses:\n");
    while (fgets(str, MAX, inputFile) != NULL)
    {
        // extract starting address from header record
        if (str[0] == 'H')
        {
            sscanf(str + 9, "%x", &location);
            reloc -= location;
            continue;
        }
        // stop if end record reached
        if (str[0] == 'E')
            break;

        int startingAddress;
        //  extract starting address from text record
        sscanf(str, "T%x", &startingAddress);
        location = startingAddress + reloc;
        int mask;
        sscanf(str + 12, "%x", &mask);
        // point ptr to start of object code in text record
        char *ptr = str + 16;
        int len = 0;
        for (int i = 0, c = 0; ptr[i] != '\0' && ptr[i] != '\n'; i += len)
        {
            int bit = mask >> (11 - c++) & 0x1;
            char temp[MAX];
            sscanf(ptr + i, "%s", temp);
            len = strlen(temp);
            int code;
            sscanf(temp, "%x", &code);
            code = bit ? code + reloc : code;
            printf("%x %x\n", location, code);
            location += len++ / 2;
        }
    }
    return 0;
}