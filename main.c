#include <stdio.h>
#include <stdlib.h>
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
    printf(" --- Opening File --- \n");
    //input_file = fopen("//home//mihuahams//project_files//input_example.txt" , "r");
    input_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\input_example.txt" , "r");
    res_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\res_file.txt" , "w");
    if(input_file == NULL){ //check the file was opened successfully - if not terminate
        printf(" --- Can't open input file --- \n");
        return 0;
    }
    printf("---------------------------------------------------------------------------------------------------------");
    /* ------------------------------------------------ File Opening ------------------------------------------------ */
    /* -------------------------------------------------------------------------------------------------------------- */
    /* ------------------------------------------------ File Reading ------------------------------------------------ */

    int block_line_count = 0;
    char dir_name_hash[11];
    int namespace_depth;
    int file_size;
    int file_attribute;
    char file_id[16];

    printf(" --- Start Reading the file --- \n");
    do{ //skip till the first empty line - over the file system description
        fgets(buff, 255 , input_file);
    } while(strlen(buff) > 1);
    printf(" --- Skipped over the file-system data block - lets start reading the real data --- \n");

    while(!feof(input_file)){//Read File till the end - parse each block and add it to the corresponding structure
        fgets(buff, 255 , input_file);
        block_line_count++;
        if(strcmp(buff , "LOGCOMPLETE\n") == 0){ //Check if we have reached the end of the file, nothing more to read
            fileEnd = true;
            fgets(buff, 255 , input_file); // read the last line before EOF
        }
        if(fileEnd == false){ // We havent seen the LOGCOMPLETE line yet
            while (strlen(buff) > 1 && !feof(input_file)){ // Check if we haven't reached the end of the current input block
                fputs(buff , res_file);
                switch(block_line_count){
                    case 1: //save the directory name
                        strncpy(dir_name_hash , buff , 10); //only first 10 digits depict the hashed directory name
                        dir_name_hash[10] = '\0';
                        fprintf(res_file , "-->File name is: %s \n" , dir_name_hash);
                        break;
                    case 4: //save the namespace depth
                        namespace_depth = atoi(buff);
                        fprintf(res_file , "--> Namespace depth is : %d \n" , namespace_depth);
                        break;
                    case 5: // save the file size
                        file_size = atoi(buff);
                        fprintf(res_file , "--> File size is : %d \n" , file_size);
                        break;
                    case 6: //save the file attributes value
                        file_attribute = atoi(buff);
                        fprintf(res_file , "--> File attribute is : %d \n" , file_attribute);
                        break;
                    case 7: //save the hashed file id
                        strncpy(file_id , buff , 15); //only first 10 digits depict the hashed directory name
                        file_id[15] = '\0';
                        fprintf(res_file , "-->File id is: %s \n" , file_id);
                        break;
                    default:
                        break;
                }

                fgets(buff, 255 , input_file); //read next line in current block
                block_line_count++;
            }
            if(!feof(input_file)){ // !!! WE HAVE REACHED THE END OF THE CURRENT INPUT BLOCK !!!
                fputs("-------> finished Reading a block\n\n" , res_file);
                block_line_count = 0; //zero the line count for the next block
            }
        }
    }

    printf(" --- Finished reading the input file - Now lets start processing ---\n");
    /* ------------------------------------------------ File Reading ------------------------------------------------ */
    /* -------------------------------------------------------------------------------------------------------------- */
    /* ------------------------------------------------ File Closing ------------------------------------------------ */
    fclose(input_file);
    fclose(res_file);
    /* ------------------------------------------------ File Closing ------------------------------------------------ */
    return 0;
}