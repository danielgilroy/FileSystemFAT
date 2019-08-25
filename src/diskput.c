/*---------------------------------------------------*/
/*               CSC 360 - p3 Assingment             */
/*                                                   */
/* Date: December 1, 2014                            */
/* Name: Daniel Gilroy                               */
/* Student ID: V00813027                             */
/*                                                   */
/*---------------------------------------------------*/

#include "helper.h"

#define REQUIRED_ARGS 3

int main(int argc, char *argv[]){

	FILE *img_file;
	FILE *input_file;
	char *img_filename = NULL;
	char *input_filename = NULL;
	SB_table super_block_info;
	directory_struct *rootdir_entries;
	directory_struct target_entry;
	int index;
	int result;

	if(argc != REQUIRED_ARGS){
		fprintf(stderr, "--Argument Error--\n");
		fprintf(stderr, "Please enter two arguments as shown: ./diskget <img_filename> <input_filename>\n");
		exit(EXIT_FAILURE);
	}else{
		img_filename = argv[1];
		input_filename = argv[2];
	}

	if(strlen(input_filename) > 31){
		fprintf(stderr, "--Filename Length Error--\n");
		fprintf(stderr, "Maximum supported filename length is 31 characters\n");
		exit(EXIT_FAILURE);
	}

	img_file = fopen(img_filename, "r+b");
	if(img_file == NULL){
		fprintf(stderr, "--Img Filename Error--\n");
        fprintf(stderr, "%s: File not found\n", img_filename);
        exit(EXIT_FAILURE);
    }

    input_file = fopen(input_filename, "rb");
	if(input_file == NULL){
		fprintf(stderr, "--Input Filename Error--\n");
        fprintf(stderr, "%s: File not found\n", input_filename);
        exit(EXIT_FAILURE);
    }

    /* Get Super Block Info */
    result = get_super_block_info(&super_block_info, img_file);
    /*ERROR CHECKING NEEDED*/

    /* Check if file already exists in the img file */
    result = get_specific_rootdir_entry(input_filename, &target_entry, img_file, &super_block_info);
  	if(result != FILE_NOT_FOUND){
  		fprintf(stderr, "--Input Filename Error--\n");
     	fprintf(stderr, "%s: A file with that name already exists in %s\n", input_filename, img_filename);
		exit(EXIT_FAILURE);
	}

    /* Write the input_file into the img_file */
    result = put_file_into_img(input_filename, img_file, input_file, &super_block_info);
    if(result == READ_WRITE_ERROR){
    	fprintf(stderr, "--Write Error--\n");
		fprintf(stderr, "There was an error writting the entire contents of %s into %s\n", input_filename, img_filename);
		exit(EXIT_FAILURE);
    }else if(result == FREE_BLOCK_ERROR){
    	fprintf(stderr, "--Block Error--\n");
    	fprintf(stderr, "There are no remaining free blocks in %s\n", img_filename);
    	exit(EXIT_FAILURE);
    }else if(result == FREE_DIR_ENTRY_ERROR){
    	fprintf(stderr, "--Directory Entry Error--\n");
    	fprintf(stderr, "There are no remaining free root directory entries in %s\n", img_filename);
    	exit(EXIT_FAILURE);
    }

  	printf("%s: File successfully written to %s\n", input_filename, img_filename);

    fclose(img_file);
    fclose(input_file);

	return 0;
}