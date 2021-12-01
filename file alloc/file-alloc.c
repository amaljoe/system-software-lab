#include <stdio.h>

struct file
{
    char name[10];
    int location[50];
    int size;
} files[50];

void sequential();

int main()
{
    sequential();
    return 0;
}

void sequential()
{
    int n;
    printf("Enter the number of files:\n");
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        printf("Enter the name of file %d:\n", i + 1);
        scanf("%s", files[i].name);
    }
    for (int i = 0; i < n; i++)
    {
        printf("%s\n", files[i].name);
    }
}