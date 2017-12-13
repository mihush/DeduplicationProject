#include <stdio.h>

int main() {
    printf("Opening File ...\n");
    FILE *fp;
    char buff[255];

    fp = fopen("C:\\Users\\mihush\\Documents\\Technion\\Sem_7\\Gala - project\\input_example.txt" , "r");
    if(fp ==NULL){
        printf("Can't open input file\n");
        return 0;
    }

    printf("The contents of the file are:");
    for (int i = 0; i < 10 ; ++i) {
        fgets(buff, 255 , fp);
        printf("%s\n" , buff);
    }

    printf("Finished\n");
    fclose(fp);
    return 0;
}