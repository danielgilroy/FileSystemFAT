/*---------------------------------------------------*/
/*               CSC 360 - p3 Assingment             */
/*                                                   */
/* Date: December 1, 2014                            */
/* Name: Daniel Gilroy                               */
/* Student ID: V00813027                             */
/*                                                   */
/*---------------------------------------------------*/

#include "helper.h"

#define REQUIRED_ARGS 2

int main(int argc, char *argv[]){

	FILE *file;
	char *filename = NULL; argv[1];
	SB_table super_block_info;
	FAT_table FAT_info = {0};
	int result;

	if(argc != REQUIRED_ARGS){
		printf("--Argument Error--\n");
		printf("Please enter one argument as shown: ./diskinfo <img_filename>\n");
		exit(EXIT_FAILURE);
	}else{
	    filename = argv[1];
    }

	file = fopen(filename, "rb");
	if(file == NULL){
        fprintf(stderr, "%s: File not found\n", filename);
        exit(EXIT_FAILURE);
    }
    
    /* Get Super Block Info */
    result = get_super_block_info(&super_block_info, file);
    /*ERROR CHECKING NEEDED*/

    /* Get FAT info using FAT starting block and block count*/
    result = get_FAT_info(&FAT_info, file, &super_block_info);
    /*ERROR CHECKING NEEDED*/

	fclose(file);
    
    /* PRINT RESULTS */

    /* Print Super block information */
    printf("\nSuper block information:\n");
    printf("Block size: %u\n", super_block_info.block_size);
    printf("Block count: %u\n", super_block_info.block_count);
    printf("FAT start: %u\n", super_block_info.FAT_start);
    printf("FAT blocks: %u\n", super_block_info.FAT_blocks);
    printf("Root directory start: %u\n", super_block_info.rootdir_start);
    printf("Root directory blocks: %u\n\n", super_block_info.rootdir_blocks);

    /* Print FAT information */
    printf("FAT information:\n");
    printf("Free blocks: %u\n", FAT_info.free_blocks);
    printf("Reserved blocks: %u\n", FAT_info.reserved_blocks);
	printf("Allocated blocks: %u\n", FAT_info.allocated_blocks);

	return 0;
}