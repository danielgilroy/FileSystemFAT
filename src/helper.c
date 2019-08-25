/*---------------------------------------------------*/
/*               CSC 360 - p3 Assingment             */
/*                                                   */
/* Date: December 1, 2014                            */
/* Name: Daniel Gilroy                               */
/* Student ID: V00813027                             */
/*                                                   */
/*---------------------------------------------------*/

#include <time.h>
#include "helper.h"

extern int get_super_block_info(SB_table *super_block_info, FILE *file){

	fread(super_block_info, sizeof(SB_table), 1, file);
    super_block_info->block_size = ntohs(super_block_info->block_size);
    super_block_info->block_count = ntohl(super_block_info->block_count);
    super_block_info->FAT_start = ntohl(super_block_info->FAT_start);
    super_block_info->FAT_blocks = ntohl(super_block_info->FAT_blocks);
    super_block_info->rootdir_start = ntohl(super_block_info->rootdir_start);
    super_block_info->rootdir_blocks = ntohl(super_block_info->rootdir_blocks);

	return 0;
}

extern int get_FAT_info(FAT_table *FAT_info, FILE *file, SB_table *super_block_info){

	int index;
	unsigned int FAT_entry;
    // int FAT_starting_address = (super_block_info->block_size * super_block_info->FAT_start);
    unsigned int FAT_address = (super_block_info->block_size * super_block_info->FAT_start);

	for(index = 0; index < super_block_info->block_count; index++){

 		/* fseek to the start of the FAT table which is*/
    	/* the block size * the FAT starting block */
    	/* each loop adds sizeof(unsigned int) to traverse the FAT table */
		fseek(file, FAT_address + (sizeof(unsigned int) * index), SEEK_SET);
		fread(&FAT_entry, sizeof(unsigned int), 1, file);

		FAT_entry = ntohl(FAT_entry);

		if(FAT_entry == AVAILABLE_FAT_BLOCK){
			FAT_info->free_blocks++;
		}else if(FAT_entry == RESERVED_FAT_BLOCK){
			FAT_info->reserved_blocks++;
		}else if(FAT_entry > RESERVED_FAT_BLOCK && FAT_entry <= ALLOCATED_FAT_RANGE){
			FAT_info->allocated_blocks++;
		}else if(FAT_entry == LAST_BLOCK){
			FAT_info->allocated_blocks++;
		}
	}

	return 0;
}

extern int get_all_rootdir_entries(directory_struct *rootdir_entries, FILE *file, SB_table *super_block_info){

	int index;
	int entries_processed = 0;
	int rootdir_array_index = 0;
	unsigned int block_result;
    unsigned int rootdir_entries_per_block = ((super_block_info->block_size) / DIRECTORY_ENTRY_SIZE);
    unsigned int current_block = super_block_info->rootdir_start;
    unsigned int rootdir_block_address;

    do{

    	rootdir_block_address = (current_block * super_block_info->block_size);

		for(index = 0; index < rootdir_entries_per_block; index++){
			
			/* Use fread to get directory entry */
			fseek(file, rootdir_block_address + (sizeof(directory_struct) * index), SEEK_SET);
			fread(&rootdir_entries[rootdir_array_index], sizeof(directory_struct), 1, file);

			/* Only stores files that are in use into the rootdir_entries array*/
			if(rootdir_entries[rootdir_array_index].status == IN_USE_FILE  || \
				rootdir_entries[rootdir_array_index].status == IN_USE_DIRECTORY){

				/* Convert numerical data to Big Endian */
				rootdir_entries[rootdir_array_index].start_block = ntohl(rootdir_entries[rootdir_array_index].start_block);
				rootdir_entries[rootdir_array_index].block_count = ntohl(rootdir_entries[rootdir_array_index].block_count);
				rootdir_entries[rootdir_array_index].file_size = ntohl(rootdir_entries[rootdir_array_index].file_size);
			 	rootdir_entries[rootdir_array_index].create_time_year = ntohs(rootdir_entries[rootdir_array_index].create_time_year);
				rootdir_entries[rootdir_array_index].modify_time_year = ntohs(rootdir_entries[rootdir_array_index].modify_time_year);
				
				rootdir_array_index++;

			}
		
			entries_processed++;
		}

		/* send current_block and this function will update it to the next block */
		block_result = get_next_block(&current_block, file, super_block_info);
		
	}while(block_result != LAST_BLOCK);


	/* Error checking to make sure all blocks were read: Compares # of blocks specified in the super_block */
	if(super_block_info->rootdir_blocks != (entries_processed/rootdir_entries_per_block)){
		fprintf(stderr, "Error reading root directory: Did not read all blocks \n");
		fprintf(stderr, "%d blocks were not read: Error reaching final 0xFFFFFFFF entry in FAT table \n", \
			super_block_info->rootdir_blocks - (entries_processed/rootdir_entries_per_block));
		exit(1);
	}

	return 0;
}

extern int get_specific_rootdir_entry(char *filename, directory_struct *target_entry, FILE *file, SB_table *super_block_info){

	int index;
	unsigned int block_result;
    unsigned int rootdir_entries_per_block = ((super_block_info->block_size) / DIRECTORY_ENTRY_SIZE);
    unsigned int current_block = super_block_info->rootdir_start;
    unsigned int rootdir_block_address;

    do{

    	rootdir_block_address = (current_block * super_block_info->block_size);

		for(index = 0; index < rootdir_entries_per_block; index++){
			
			/* Use fread to get directory entry */
			fseek(file, rootdir_block_address + (sizeof(directory_struct) * index), SEEK_SET);
			fread(target_entry, sizeof(directory_struct), 1, file);

			/* Check if match found */
			if(!strcmp(target_entry->file_name, filename)){

				/* Convert numerical data to Big Endian */
				target_entry->start_block = ntohl(target_entry->start_block);
				target_entry->block_count = ntohl(target_entry->block_count);
				target_entry->file_size = ntohl(target_entry->file_size);
			 	target_entry->create_time_year = ntohs(target_entry->create_time_year);
				target_entry->modify_time_year = ntohs(target_entry->modify_time_year);
				
				/* Assumes no duplicate filenames and returns after first match is found */
				return 0;
			}
		}

		/* send current_block and this function will update it to the next block */
		block_result = get_next_block(&current_block, file, super_block_info);
		
	}while(block_result != LAST_BLOCK);

	/* If file was not found, return FILE_NOT_FOUND error */
	return FILE_NOT_FOUND;
}

extern int get_file_from_entry(directory_struct *target_entry, FILE *output_file, FILE *input_file, SB_table *super_block_info){

	unsigned int current_block;
	unsigned int block_result;
	unsigned int target_block_address;
	unsigned char buffer[super_block_info->block_size];
	unsigned int bytes_to_read = target_entry->file_size;

	/* Use starting block and FAT table to read file */
	current_block = target_entry->start_block;

	do{

		target_block_address = (current_block * super_block_info->block_size);

		/*-------------TEST-------------*/
		// printf("Reading block: %u at location: %u\n", current_block, target_block_address);
		// printf("Remaining: %u\tBLOCK:%u\t", bytes_to_read, current_block);
		/*------------END TEST----------*/


		fseek(input_file, target_block_address, SEEK_SET);

		if(bytes_to_read >= super_block_info->block_size){
			fread(&buffer, 1, super_block_info->block_size, input_file);
			fwrite(buffer, 1, super_block_info->block_size, output_file);
			bytes_to_read -= super_block_info->block_size;
			//printf("Read: %u\n", super_block_info->block_size);
		}else{
			fread(&buffer, 1, bytes_to_read, input_file);
			fwrite(buffer, 1, bytes_to_read, output_file);
			//printf("Read: %u\n", bytes_to_read);
			bytes_to_read -= bytes_to_read;
		}
	
		block_result = get_next_block(&current_block, input_file, super_block_info);

	}while(block_result != LAST_BLOCK);

	if(bytes_to_read != 0){
		return READ_WRITE_ERROR;
	}

	return 0;
}

extern int put_file_into_img(char *input_filename, FILE* img_file, FILE* input_file, SB_table *super_block_info){

	int result = 0;
	int index;
	unsigned int finish_block = LAST_BLOCK;
	unsigned int bytes_read;
	unsigned int remaining_bytes_to_read;
	unsigned int current_free_block = FILES_FIRST_BLOCK;
	unsigned char buffer[super_block_info->block_size];
	unsigned int FAT_address = (super_block_info->block_size * super_block_info->FAT_start);

	/* Create a new root directory entry for the file */
	directory_struct new_entry;
	memset(&new_entry, 0x00, DIRECTORY_ENTRY_SIZE); /* Initialize all bytes in entry to 0x00 */
	memset(&new_entry.unused, 0xFF, DIRECTORY_ENTRY_UNUSED_SIZE); /* Set unsed part of entry to 0xFFFFFFFF */

	/* Get file_size of input_file */
	fseek(input_file, 0, SEEK_END);
	new_entry.file_size = ftell(input_file);
	remaining_bytes_to_read = new_entry.file_size;
	fseek(input_file, 0, SEEK_SET); 

	result = get_next_free_block(&current_free_block, img_file, super_block_info);
	new_entry.block_count++;
	new_entry.start_block = current_free_block;
	if(result == FREE_BLOCK_ERROR){
		return FREE_BLOCK_ERROR;
	}

	while(1){

		bytes_read = fread(&buffer, 1, super_block_info->block_size, input_file);
		fseek(img_file, current_free_block * super_block_info->block_size, SEEK_SET);
		
		if(remaining_bytes_to_read >= super_block_info->block_size){	
			fwrite(buffer, 1, super_block_info->block_size, img_file);
		}else{
			fwrite(buffer, 1, remaining_bytes_to_read, img_file);
		}

		remaining_bytes_to_read -= bytes_read;

		/*-------------TEST-------------*/
		//printf("Current_block: %u\tRemeaining Read: %u\n", current_free_block, remaining_bytes_to_read);
		/*------------END TEST----------*/

		if(remaining_bytes_to_read > 0){

			get_next_free_block(&current_free_block, img_file, super_block_info);
			new_entry.block_count++;
			if(result == FREE_BLOCK_ERROR){
				return FREE_BLOCK_ERROR;
			}

		}else{
			break;
		}
	}

	/* Error Checking: Return error if not all bytes were read or too many bytes were read */
	if(remaining_bytes_to_read != 0){
		return READ_WRITE_ERROR;
	}

	/* Last block: Set current_free_block FAT entry to 0xFFFFFFFF */
	fseek(img_file, FAT_address + (sizeof(unsigned int) * current_free_block), SEEK_SET);
	fwrite(&finish_block, sizeof(unsigned int), 1, img_file);

	/* Complete new root directory entry */
	new_entry.status = IN_USE_FILE;
	new_entry.start_block = htonl(new_entry.start_block);
	new_entry.block_count = htonl(new_entry.block_count);
	new_entry.file_size = htonl(new_entry.file_size);
	
	/* Store filename in new root directory entry */
	for(index = 0; input_filename[index] != '\0' && index <= 31; index++){
		new_entry.file_name[index] = input_filename[index];
	}

	set_entry_modification_date(&new_entry);
	result = write_new_entry(&new_entry, img_file, super_block_info);

	return result;
}


extern int get_next_block(unsigned int *current_block, FILE *file, SB_table *super_block_info){

	int return_result = 0;
	unsigned int FAT_block;
	unsigned int FAT_address = (super_block_info->block_size * super_block_info->FAT_start);

	fseek(file, FAT_address + (*current_block * sizeof(unsigned int)), SEEK_SET);
	fread(&FAT_block, sizeof(unsigned int), 1, file);
	FAT_block = ntohl(FAT_block);

	*current_block = FAT_block;

	/* Check if FAT entry is the last block */
	if(*current_block == LAST_BLOCK){
		return_result = LAST_BLOCK;
	} 
	else {
		return_result = 0;
	}

	return return_result;
}


extern int get_next_free_block(unsigned int *current_free_block, FILE *img_file, SB_table *super_block_info){

	int result = FREE_BLOCK_ERROR; /* Assumes no free blocks available */
	unsigned int index;
	unsigned int FAT_entry;
	unsigned int FAT_address = (super_block_info->block_size * super_block_info->FAT_start);

	for(index = *current_free_block + 1; index < super_block_info->block_count; index++){

		fseek(img_file, FAT_address + (sizeof(unsigned int) * index), SEEK_SET);
		fread(&FAT_entry, sizeof(unsigned int), 1, img_file);

		FAT_entry = htonl(FAT_entry);

		if(FAT_entry == AVAILABLE_FAT_BLOCK){
			
			result = 0; /* Free block found: Change result to success */

			if(*current_free_block != FILES_FIRST_BLOCK){
				fseek(img_file, FAT_address + (sizeof(unsigned int) * *current_free_block), SEEK_SET);
				*current_free_block = index;
				/* Convert index to Big Endian for writting in img */
				index = htonl(index);
				fwrite(&index, sizeof(unsigned int), 1, img_file);
				break;
			}else{
				*current_free_block = index;
				break;
			}
		} 
	}

	return result;
}

extern int set_entry_modification_date(directory_struct *new_entry){

	time_t rawtime;
	time(&rawtime);
    struct tm *current_time = localtime(&rawtime);

    new_entry->modify_time_year = htons(current_time->tm_year + 1900);
	new_entry->modify_time[0] = current_time->tm_mon + 1;
	new_entry->modify_time[1] = current_time->tm_mday;
	new_entry->modify_time[2] = current_time->tm_hour;
	new_entry->modify_time[3] = current_time->tm_min;
	new_entry->modify_time[4] = current_time->tm_sec;

	return 0;
}

extern int write_new_entry(directory_struct *new_entry, FILE* img_file, SB_table *super_block_info){

	int index;
	unsigned int block_result;
    unsigned int rootdir_entries_per_block = ((super_block_info->block_size) / DIRECTORY_ENTRY_SIZE);
    unsigned int current_block = super_block_info->rootdir_start;
    unsigned int rootdir_block_address;
    directory_struct temp_entry;

    do{

    	rootdir_block_address = (current_block * super_block_info->block_size);

		for(index = 0; index < rootdir_entries_per_block; index++){
			
			/* Use fread to get directory entry */
			fseek(img_file, rootdir_block_address + (sizeof(directory_struct) * index), SEEK_SET);
			fread(&temp_entry, sizeof(directory_struct), 1, img_file);

			/* Check if match found */
			if(temp_entry.status == AVAILABLE_ENTRY){

				fseek(img_file, rootdir_block_address + (sizeof(directory_struct) * index), SEEK_SET);
				fwrite(new_entry, sizeof(directory_struct), 1, img_file);

				return 0;
			}
		}

		/* send current_block and this function will update it to the next block */
		block_result = get_next_block(&current_block, img_file, super_block_info);
		
	}while(block_result != LAST_BLOCK);

	/* If above loop did not return 0, there was an error finding a free directory entry */
	return FREE_DIR_ENTRY_ERROR;
}