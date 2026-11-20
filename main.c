#include"header.h"

int main(int argc, char **argv)
{
	FILE *fp = fopen(argv[1], "r");
	if(argc!=2)
	{
		printf("Usage: make file_name\n");
		return 0;
	}
	if (fp == NULL)
	{
		printf("File not found\n");
			return 1;
	}
	
	 // Create data if it does not exist,  "w+" also clears old data.
	
	FILE *fp1 = fopen("data", "w+");
	if (fp1 == NULL)
	{
		printf("coudnt data create data file ");
			fclose(fp);
		return 1;
	}
	fclose(fp1);

	/* 1. Remove comments from original file.c */
	comment_removal(fp);
	fclose(fp);
	macro_replacement();
	/* 2. Expand #include files */
	file_inclusion1();

	/* 3. Remove comments which came from included files */
	FILE *fp3 = fopen("data", "r+");
	if (fp3 == NULL)
	{
		perror("data");
		return 1;
	}

	comment_removal(fp3);
	fclose(fp3);

	return 0;
}

