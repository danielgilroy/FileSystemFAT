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
	char *filename = NULL;
	SB_table super_block_info;
	directory_struct *rootdir_entries;
	unsigned int num_rootdir_entries;
	int index;
	int result;

	if(argc != REQUIRED_ARGS){
		fprintf(stderr, "--Argument Error--\n");
		fprintf(stderr, "Please enter one argument as shown: ./disklist <img_filename>\n");
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

    /* Get the number of root directory entries and allocate an array of structs to hold them */
    num_rootdir_entries = (super_block_info.rootdir_blocks * super_block_info.block_size / DIRECTORY_ENTRY_SIZE);
    rootdir_entries = (directory_struct *) malloc (sizeof(directory_struct) * num_rootdir_entries);
    if(rootdir_entries == NULL){
    	perror("memory error");
       	exit(EXIT_FAILURE);
    }

	/* Get root directory entries */    
    result = get_all_rootdir_entries(rootdir_entries, file, &super_block_info);
 	/*ERROR CHECKING NEEDED*/

	fclose(file);

    /* PRINT RESULTS */
    printf("\n");

    /* Loop through root directory entries and print files currently in root */
	for(index = 0; index < num_rootdir_entries; index++){

    	/* Print file/directory indicator for in use directory entries*/
    	/* F for normal file / D for directory */
	    if(rootdir_entries[index].status == IN_USE_FILE){
	    	printf("F");
	    }else if(rootdir_entries[index].status == IN_USE_DIRECTORY){
	    	printf("D");
	    }else if(rootdir_entries[index].status == AVAILABLE_ENTRY){
	    	break;
	    }

	    printf("%10u%30s ", rootdir_entries[index].file_size, rootdir_entries[index].file_name);
	    printf("%hu/%02d", rootdir_entries[index].modify_time_year, (int)rootdir_entries[index].modify_time[0]);
	    printf("/%02d %d:", (int)rootdir_entries[index].modify_time[1], (int)rootdir_entries[index].modify_time[2]);
	    printf("%02d:%02d\n", (int)rootdir_entries[index].modify_time[3], (int)rootdir_entries[index].modify_time[4]);
	}

    printf("\n");

    free(rootdir_entries);
	return 0;
}