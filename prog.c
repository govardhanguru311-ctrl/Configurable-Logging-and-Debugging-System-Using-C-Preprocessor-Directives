#include "header.h"

void file_inclusion1(void)
{
	FILE *fp = fopen("data", "r");
	if (fp == NULL)
	{
		perror("data");
		return;
	}

	FILE *fp1 = fopen("dummy", "w+");
	if (fp1 == NULL)
	{
		perror("dummy");
		fclose(fp);
		return;
	}

	char path[50];
	char header[50];
	char s[100];
	char str[100];

	getcwd(path, sizeof(path));

	while (fgets(str, sizeof(str), fp) != NULL)
	{
		char *p;
		int i = 0;
		int delimiter;

		if (strstr(str, "#include"))
		{
			if ((p = strchr(str, '<')) != NULL)
			{
				delimiter = '<';
			}
			else if ((p = strchr(str, '"')) != NULL)
			{
				delimiter = '"';
			}
			else
			{
				continue;
			}

			p++;

			while (*p != '>' && *p != '"' && *p != '\0')
			{
				header[i++] = *p++;
			}

			header[i] = '\0';

			if (delimiter == '<')
			{
				sprintf(s, "/usr/include/%s", header);
			}
			else
			{
				sprintf(s, "%s/%s", path, header);
			}

			FILE *fp2 = fopen(s, "r");

			if (fp2 != NULL)
			{
				file_inclusion(fp1, fp2);
				fclose(fp2);
			}
			else
			{
				printf("Something wrong: cannot open %s\n", s);
			}
		}

		if (strstr(str, "main("))
			break;
	}

	/*
	 * Go back to beginning of data
	 * and go to end of dummy.
	 */
	fseek(fp, 0, SEEK_SET);
	fseek(fp1, 0, SEEK_END);

	/*
	 * Copy remaining source code,
	 * excluding #include lines.
	 */
	while (fgets(str, sizeof(str), fp) != NULL)
	{
		if (strstr(str, "#include"))
			continue;

		fputs(str, fp1);
	}

	fclose(fp);
	fclose(fp1);

	remove("data");
	rename("dummy", "data");
}


void comment_removal(FILE *fp1)
{
	FILE *fp3 = fopen("data", "r+");

	if (fp3 == NULL)
	{
		perror("data");
		return;
	}

	int ch;

	while ((ch = fgetc(fp1)) != EOF)
	{
		if (ch == '/')
		{
			ch = fgetc(fp1);

			/* // comment */
			if (ch == '/')
			{
				while ((ch = fgetc(fp1)) != '\n')
				{
					if (ch == EOF)
						break;
				}

				continue;
			}

			/* /* comment */
			else if (ch == '*')
			{
l:
				while ((ch = fgetc(fp1)) != '*')
				{
					if (ch == EOF)
						break;
				}

				if (ch == EOF)
					break;

				ch = fgetc(fp1);

				if (ch != '/')
				{
					if (ch == EOF)
						break;

					fseek(fp1, -1, SEEK_CUR);
					goto l;
				}

				continue;
			}

			/*
			 * It was just '/' and not a comment.
			 * Move back so that '/' is written.
			 */
			fseek(fp1, -1, SEEK_CUR);
			ch = '/';
		}

		fputc(ch, fp3);
	}

	/*
	 * Remove old leftover data.
	 */
	long pos = ftell(fp3);

	fflush(fp3);
	ftruncate(fileno(fp3), pos);

	fclose(fp3);
}


void file_inclusion(FILE *fp1, FILE *fp2)
{
	int ch;

	while ((ch = fgetc(fp2)) != EOF)
	{
		fputc(ch, fp1);
	}
}
void macro_replacement()
{
    FILE *fp = fopen("data", "r");

    if (fp == NULL)
    {
        printf("data file not found\n");
        return ;
    }

    char s[100], macro[50], replace[200];
    char arg[50];

    int ch;

    while ((fscanf(fp, "%s", s)) != EOF)
    {
        if (strstr(s, "#define"))
        {
            int i = 0, j = 0;

            while ((ch = fgetc(fp)) == ' ');

            /*
             * Read macro name
             */
            while (ch != '(' && ch != ' ' && ch != '\n')
            {
                macro[i++] = ch;
                ch = fgetc(fp);
            }

            /*
             * OBJECT-LIKE MACRO
             *
             * #define PI 3.14
             */
            if (ch == ' ')
            {
                macro[i] = '\0';

                while ((ch = fgetc(fp)) == ' ');

                replace[j++] = ch;

                while ((ch = fgetc(fp)) != '\n' && ch != EOF)
                    replace[j++] = ch;

                replace[j] = '\0';

                printf("MACRO = %s   REPLACE = %s\n",
                       macro, replace);

                macro_replace_const(macro, replace);
            }

            /*
             * FUNCTION-LIKE MACRO
             *
             * #define count(num) ...
             */
            else if (ch == '(')
            {
                macro[i] = '\0';

                /*
                 * Read parameter
                 */
                i = 0;

                while ((ch = fgetc(fp)) != ')' && ch != EOF)
                {
                    if (ch != ' ' && ch != '\t')
                        arg[i++] = ch;
                }

                arg[i] = '\0';

                /*
                 * Skip spaces after )
                 */
                while ((ch = fgetc(fp)) == ' ');

                /*
                 * Replacement starts here
                 */
                j = 0;

                if (ch != EOF)
                    replace[j++] = ch;

                while ((ch = fgetc(fp)) != '\n' && ch != EOF)
                {
                    if (ch == '\\')
                    {
                        replace[j++] = ' ';

                        /*
                         * Skip newline and indentation
                         */
                        while ((ch = fgetc(fp)) == ' ' || ch == '\t' || ch == '\n')
                        {
                        }

                        if (ch != EOF)
                            replace[j++] = ch;

                        continue;
                    }

                    replace[j++] = ch;
                }

                replace[j] = '\0';

                printf("MACRO = %s   ARG = %s   REPLACE = %s\n",
                       macro, arg, replace);

                macro_replace_fun(macro, arg, replace);
            }
        }
    }

    fclose(fp);
}


/*
 * Check whether character can be part of a C identifier.
 */
int is_word_char(char ch)
{
    return isalnum((unsigned char)ch) || ch == '_';
}



void macro_replace_const(char *macro, char *replace)//#define PI 3.14
{
    FILE *fp = fopen("data", "r");
    FILE *fp1 = fopen("data1", "w+");

    if (fp == NULL || fp1 == NULL)
    {
        printf("File opening error\n");
        return;
    }

    char str[500];

    while (fgets(str, sizeof(str), fp) != NULL)
    {
        /*
         * Don't copy #define line.
         */
        if (strncmp(str, "#define", 7) == 0)
            continue;

        char *p = str;

        while (*p != '\0')
        {
        	if(*p=='"')
        	{
        		fputc(*p, fp1);
          	 	p++;
        		
        		while(*p!='"')
        		{
        			fputc(*p, fp1);
           			 p++;
        		}
        		fputc(*p, fp1);
           			 p++;
           		continue;
        		
        	}
            /*
             * Check whether macro starts here
             */
            if (strncmp(p, macro, strlen(macro)) == 0)
            {
                /*
                 * Make sure it is a complete identifier.
                 *
                 * PI should match PI
                 * but should not match PII.
                 */
                char before = (p == str) ? ' ' : *(p - 1);
                char after = *(p + strlen(macro));

                if (!is_word_char(before) &&
                    !is_word_char(after))
                {
                    fputs(replace, fp1);
                    p += strlen(macro);
                    continue;
                }
            }

            fputc(*p, fp1);
            p++;
        }
    }

    fclose(fp);
    fclose(fp1);

    remove("data");
    rename("data1", "data");
}


void macro_replace_fun(char *macro, char *arg, char *replace)      //#define P1() 3.14 and  #define count(num) (...) 
{
    FILE *fp = fopen("data", "r");
    FILE *fp1 = fopen("data1", "w+");

    if (fp == NULL || fp1 == NULL)
    {
        printf("File opening error\n");
        return;
    }

    char str[500];
    int skip = 0;

    while (fgets(str, sizeof(str), fp) != NULL)
    {
        /*
         * Skip the #define line and all its
         * continuation lines.
         */
        if (strncmp(str, "#define", 7) == 0 &&
            strstr(str, macro) != NULL)
        {
            skip = 1;
            continue;
        }

        if (skip)
        {
            /*
             * Continuation lines belong to the
             * previous #define.
             *
             * If this line does not contain '\',
             * the macro definition has ended.
             */
            if (strchr(str, '\\') != NULL)
                continue;

            skip = 0;
            continue;
        }

        char *p = str;

        while (*p != '\0')
        {
       		 if(*p=='"')
        	{
        		fputc(*p, fp1);
          	 	p++;
        		
        		while(*p!='"')
        		{
        			fputc(*p, fp1);
           			 p++;
        		}
        		fputc(*p, fp1);
           			 p++;
           		continue;
        		
        	}
            if (strncmp(p, macro, strlen(macro)) == 0)
            {
                char *q = p + strlen(macro);

                if (*q == '(')
                {
                    char actual_arg[100];
                    int i = 0;

                    q++;

                    while (*q != ')' && *q != '\0')
                        actual_arg[i++] = *q++;

                    actual_arg[i] = '\0';

                    if (*q == ')')
                    {
                        char *r = replace;

                        while (*r != '\0')
                        {
                            if (strncmp(r, arg, strlen(arg)) == 0)
                            {
                                char before =
                                    (r == replace) ? ' ' : *(r - 1);

                                char after =
                                    *(r + strlen(arg));

                                if (!is_word_char(before) &&
                                    !is_word_char(after))
                                {
                                    fputs(actual_arg, fp1);
                                    r += strlen(arg);
                                    continue;
                                }
                            }

                            fputc(*r, fp1);
                            r++;
                        }

                        p = q + 1;
                        continue;
                    }
                }
            }

            fputc(*p, fp1);
            p++;
        }
    }

    fclose(fp);
    fclose(fp1);

    remove("data");
    rename("data1", "data");
}
