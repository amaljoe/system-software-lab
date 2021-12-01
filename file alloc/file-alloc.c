#include <stdio.h>
#include <string.h>

struct file
{
    char name[10];
    int location[50];
    int size;
    int starting_address;
} files[50];

int n;

void sequential();
int search(char[]);

int main()
{
    sequential();
    return 0;
}

void sequential()
{
    printf("Enter the number of files:\n");
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        printf("Enter the name of file %d:\n", i + 1);
        scanf("%s", files[i].name);
        printf("Enter the size of file %d:\n", i + 1);
        scanf("%s", files[i].size);
        printf("Enter the starting index of file %d:\n", i + 1);
        scanf("%s", files[i].starting_address);
    }
    char key[10];
    printf("Enter the name of file to be searched:\n");
    scanf("%s", key);
    int index = search(key);
    if (index == -1)
    {
        printf("File not found\n");
        return;
    }
}

// search for a filename in a list of files, return file index or -1 if not found
int search(char key[])
{
    for (int i = 0; i < n; i++)
    {
        if (strcmp(key, files[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}