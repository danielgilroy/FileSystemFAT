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
	FILE *output_file;
	char *img_filename = NULL;
	char *output_filename = NULL;
	SB_table super_block_info;
	directory_struct target_entry;
	int result;

	if(argc != REQUIRED_ARGS){
		printf("--Argument Error--\n");
		printf("Please enter two arguments as shown: ./diskget <img_filename> <output_filename>\n");
		exit(EXIT_FAILURE);
	}else{
		img_filename = argv[1];
		output_filename = argv[2];
	}

	img_file = fopen(img_filename, "rb");
	if(img_file == NULL){
        fprintf(stderr, "%s: File not found\n", img_filename);
        exit(EXIT_FAILURE);
    }

    /* Get Super Block Info */
    result = get_super_block_info(&super_block_info, img_file);
    /*ERROR CHECKING NEEDED*/
    
    /* Get the root directory entry related to the output_filename argument */
    result = get_specific_rootdir_entry(output_filename, &target_entry, img_file, &super_block_info);
  	if(result == FILE_NOT_FOUND){
     	fprintf(stderr, "%s: File not found\n", output_filename);
		exit(EXIT_FAILURE);
	}

	/* fopen the file to write into and call the function to start writting */
	output_file = fopen(target_entry.file_name,"wb"); 
  	result = get_file_from_entry(&target_entry, output_file, img_file, &super_block_info);
  	if(result != 0){
  		fprintf(stderr, "--Read Error--\n");
		fprintf(stderr, "There was an error reading the entire contents of %s into %s\n", output_filename, img_filename);
		exit(EXIT_FAILURE);
  	}

  	printf("%s: File successfully copied from %s\n", output_filename, img_filename);

    fclose(img_file);
    fclose(output_file);

	return 0;
}