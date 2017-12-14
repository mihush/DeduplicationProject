#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main() {
    int num_of_input_files = 0;
    const char* input_dir_start;
    char file_names [num_of_input_files][255];
    FILE *input_file , *res_file;
    char buff[255];
    bool fileEnd = false;

    /* ------------------------------------------ Get File Names To Process ----------------------------------------- */
    /*printf("How many files would you like to process?\n");
    scanf("%d" , &num_of_input_files);
    if(num_of_input_files < 1){ //check that at least one file will be processed
        printf("You have requested an illegal number of files to process - The program will end now.");
        return 0;
    }
    printf("Please insert the path to the directory containing the imput files: \n");
    scanf("%s" , input_dir_start);

    printf("Now we will insert the file names to be processed:\n");
    for (int i = 0; i <num_of_input_files ; ++i) {
        printf("Please insert the name of file %d : \n",(i+1));
        scanf("%s" , file_names[i]);
    }*/
    printf("---------------------------------------------------------------------------------------------------------");
    /* ------------------------------------------ Get File Names To Process ----------------------------------------- */
    /* -------------------------------------------------------------------------------------------------------------- */
    /* ------------------------------------------------ File Opening ------------------------------------------------ */
    printf("Opening File ...\n");
    //input_file = fopen("//home//mihuahams//project_files//input_example.txt" , "r");
    input_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\input_example.txt" , "r");
    res_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\res_file.txt" , "w");
    if(input_file == NULL){ //check the file was opened successfully - if not terminate
        printf("Can't open input file\n");
        return 0;
    }
    printf("---------------------------------------------------------------------------------------------------------");
    /* ------------------------------------------------ File Opening ------------------------------------------------ */
    /* -------------------------------------------------------------------------------------------------------------- */
    /* ------------------------------------------------ File Reading ------------------------------------------------ */
    printf("Start Reading the file ... \n");
    do{ //skip till the first empty line - over the file system description
        fgets(buff, 255 , input_file);
    } while(strlen(buff) > 1);
    printf("Skipped over the file-system data block");


    printf("---> now lets start Reading the real data ...\n");
    while(!feof(input_file)){//Read File till the end
        fgets(buff, 255 , input_file);
        if(strcmp(buff , "LOGCOMPLETE\n") == 0){ //You have reached the end of the file, nothing more to read
            printf("%d - We have reached the end of the file\n", strcmp(buff , "LOGCOMPLETE"));
            fileEnd = true;
            fgets(buff, 255 , input_file); // read the last line before EOF
        }
        if(fileEnd == false){
            while (strlen(buff) > 1 && !feof(input_file)){
                fputs(buff , res_file);
                //printf("%s  -> %d \n" , buff , strlen(buff));
                fgets(buff, 255 , input_file);
            }
            if(!feof(input_file)){
                fputs("-----------> finished Reading a block\n\n" , res_file);
            }
        }
    }
    printf("---------------------------------------------------------------------------------------------------------");
    printf("Finished reading the input file - Now lets start processing ...\n");
    /* ------------------------------------------------ File Reading ------------------------------------------------ */
    /* -------------------------------------------------------------------------------------------------------------- */
    /* ------------------------------------------------ File Closing ------------------------------------------------ */
    fclose(input_file);
    fclose(res_file);
    /* ------------------------------------------------ File Closing ------------------------------------------------ */
    return 0;
}