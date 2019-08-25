
# File System FAT

University of Victoria - CSC 360 Operating Systems (Fall 2014): Project 3

This project implements utilities that perform operations on a file system similar to Microsoft’s FAT file system. After running the Makefile, there will be four applications called "diskinfo", "disklist", "diskput", and "diskget".

- The "diskinfo" application returns information about the FAT disk image
- The "disklist" application lists the files within the disk image
- The "diskput" application stores files into the disk image
- The "diskget" application retrieves files from the disk image

## Usage

Note: A disk image sample named "disk.img" has been included for use

>## diskinfo

To see the FAT information for a given disk image, use the "diskinfo" application with the filename of the disk image given as an argument

    diskinfo <img_filename>

>## disklist

To see a list of files contained in the disk image, use the "disklist" application with the filename of the disk image given as an argument

    disklist <img_filename>

>## diskput

To store files in the disk image, use the "diskput" application with the filenames of the disk image and input file given as arguments

    diskput <img_filename> <input_filename>

>## diskget

To retrieve files from the disk image, use the "diskget" application with the filenames of the disk image and output file given as arguments

    diskget <img_filename> <output_filename>

## Limitations

This project has the following limitations which would be beneficial to have implemented for a real-world application

- No support for deleting files in the disk image or overwriting files with the same filename
- Only works with regular filetypes so directories are not supported
- No support for creating an empty disk image
