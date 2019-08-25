#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#define FILE_NOT_FOUND -1
#define READ_WRITE_ERROR -2
#define FREE_BLOCK_ERROR -3
#define FREE_DIR_ENTRY_ERROR -4
#define DIRECTORY_ENTRY_SIZE 64
#define DIRECTORY_ENTRY_UNUSED_SIZE 6
#define AVAILABLE_ENTRY 0x0
#define IN_USE_FILE 0x3
#define IN_USE_DIRECTORY 0x5
#define FILES_FIRST_BLOCK 1
#define AVAILABLE_FAT_BLOCK 0x00000000
#define RESERVED_FAT_BLOCK 0x00000001
#define ALLOCATED_FAT_RANGE 0xFFFFFF00
#define LAST_BLOCK 0xFFFFFFFF

typedef struct SB_table{
	char fs_id[8];
	unsigned short block_size;
    unsigned int block_count;
	unsigned int FAT_start;
	unsigned int FAT_blocks;
	unsigned int rootdir_start;
	unsigned int rootdir_blocks;
}__attribute((packed)) SB_table;

typedef struct FAT_table{
	unsigned int free_blocks;
	unsigned int reserved_blocks;
	unsigned int allocated_blocks;
}FAT_table;

typedef struct directory_struct{
	unsigned char status;
	unsigned int start_block;
	unsigned int block_count;
	unsigned int file_size;
	unsigned short create_time_year;
	unsigned char create_time[5];
	unsigned short modify_time_year;
	unsigned char modify_time[5];
	unsigned char file_name[31];
	unsigned char unused[6]; /* (set to 0xFF) */
}__attribute((packed)) directory_struct;

int get_super_block_info(SB_table *, FILE *);
int get_FAT_info(FAT_table *, FILE *, SB_table *);
int get_all_rootdir_entries(directory_struct *, FILE *, SB_table *);
int get_specific_rootdir_entry(char *, directory_struct *, FILE *, SB_table *);
int get_file_from_entry(directory_struct *, FILE *, FILE *, SB_table *);
int put_file_into_img(char *, FILE *, FILE *, SB_table *);
int get_next_block(unsigned int *, FILE *, SB_table *);
int get_next_free_block(unsigned int *, FILE *, SB_table *);
int set_entry_modification_date(directory_struct *);
int write_new_entry(directory_struct *, FILE *, SB_table *);

#endif