#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 100

struct file
{
    char name[10];
    int location[20];
    int size;
    int starting_address;
    
} files[50];

struct file* memory[MEMORY_SIZE];
int n;

void sequential();
int search(char[]);
void input(int);
void display(int);

int main()
{
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i] = NULL;
    }
    sequential();
    return 0;
}

void sequential()
{
    printf("Enter the number of files:\n");
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        input(i);
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
    display(index);
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

void input(int i)
{
    printf("Enter the name of file %d:\n", i + 1);
    scanf("%s", files[i].name);
    printf("Enter the size of file %d:\n", i + 1);
    scanf("%d", &files[i].size);
    printf("Enter the starting index of file %d:\n", i + 1);
    scanf("%d", &files[i].starting_address);
}

void display(int i)
{
    struct file myfile = files[i];
    printf("Blocks occupied by file %s:\n", myfile.name);
    for (int i = 0; i < myfile.size; i++)
    {
        printf("%d\t", myfile.location[i]);
    }
}