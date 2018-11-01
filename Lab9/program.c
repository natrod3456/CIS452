#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 16

int main()
{
    char *data1;

    data1 = malloc (SIZE);
    printf ("Please input username: ");
    fgets(data1, SIZE, stdin);
    int len = strlen(data1);
    if(data1[len-1]=='\n')
   	data1[len-1]='\0';
    printf ("you entered: %s\n", data1);
    free (data1);
    return 0;
}
