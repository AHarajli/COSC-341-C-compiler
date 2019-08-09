/*Alaa Harajli
 COSC 341 Tuesday Thursday 530 to 710
 Pary harold 202
 Due date 3/7/2019
 Code and style in project based off code written by Professor S.Maniccam
 Eastern Michigan University
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*constants for true and false*/

#define FALSE 0
#define TRUE 1

/*enumerated types for token types*/
typedef enum
{
	ID, INTLITERAL, MAIN, READ, WRITE, IF, ELSE, WHILE,
	PLUSOP, MINUSOP, ASSIGNOP, LPAREN, RPAREN, MULTOP, DIVOP,
    COMMA, SEMICOLON, SCANEOF, GREATER, GREATEQ, LESS,
	LESSEQ, EQUALS, NOTEQ, LCURL, RCURL
}token;

/*string representations of the enum types*/
char *tokens[] = {"ID","INTLITERAL","MAIN","READ","WRITE", "IF", "ELSE",
				  "WHILE","PLUSOP","MINUSOP","ASSIGNOP","LPAREN","RPAREN",
				  "MULTOP","DIVOP","COMMA","SEMICOLON","SCANEOF",
				  "GREATER", "GREATEQ", "LESS","LESSEQ", "EQUALS", "NOTEQ",
				  "LCURL","RCURL"};


/*functions for main*/
void scan_file();
void parse_file();


/*functions for scanner*/
token scanner();
void clear_buffer();
void buffer_char(char c);
token check_reserved();
void lexical_error();

/*function for parser*/
void parser();
void program();
void statement_list();
void statement();
void id_list();
void expression_list();
void expression();
void term();
void bool();
void relop();
void add_op();
void mult_op();
void match(token tok);
void syntax_error();

/*global variables*/
FILE *fin;				//source file
FILE *fout;				//source file
token next_token;		//next token in source file
char token_buffer[100];	//token buffer
int token_ptr;			//buffer pointer
int line_num = 1;		//line number in source file
int error = FALSE;		//flag to indicate error

/*****************************************************************************/

/*returns next token from source file*/
/*original written by S.Maniccam*/
token scanner()
{


	char c;                    /* current character in source file*/

	clear_buffer();             /*empty token buffer */

	while(TRUE)                 /*loop reads and returns next token*/
	{
		c = getc(fin);          /*read a character from source file*/

		if(c == EOF)            /*end of file*/
			return SCANEOF;

		else if(isspace(c))     /*skip white space and count line numbers*/
		{
			if (c == '\n')
				 line_num = line_num + 1;
		}

		else if (isalpha(c))      /*identifier or reversed word*/
		{
			buffer_char (c);      /*buffer the first character*/
			c = getc (fin);
			while (isalnum(c) || c == '_')  /*read and buffer subsequent characters*/
			{
				buffer_char(c);
				c = getc(fin);
			}
			ungetc(c, fin);                 /*put back the character read*/
			return check_reserved();        /*return integer literal*/
		}
        /*integer literal */
		else if (isdigit(c))
		{
			buffer_char(c);            /*buffer the first character*/
			c = getc(fin);
			while(isdigit(c))           /*read and buffer subsequent characters*/
			{
				buffer_char(c);
				c = getc(fin);
			}
			ungetc(c,fin);              /*put back the last character read*/
			return INTLITERAL;          /*return integer literal*/
		}
         switch (c){
    case '(': return LPAREN;
    case ')': return RPAREN;

      /* Check for left brackets and right bracket. */
    case '{': return LCURL;
    case '}': return RCURL;

      /* Check for semicolon. */
    case ';': return SEMICOLON;

      /* Check for the comma. */
    case ',': return COMMA;

      /* Check for operators + - *. */
    case '+': return PLUSOP;
    case '-': return MINUSOP;
    case '*': return MULTOP;

    case'/':                 /* division operator*/

			    c = getc(fin);
				if(c == '/')                /*comment begins*/
				{
					do                      /*read and discard until end of line*/
						c = getc(fin);
					while (c != '\n');
					line_num = line_num + 1;
				}
				else                        /*division operator*/
				{
					ungetc(c, fin);
					return DIVOP;
				}
			break;
        /*check for assinment operator*/
		case ':':

			c = getc(fin);
			if(c == '=')
				return ASSIGNOP;

				ungetc(c, fin);
				lexical_error();
                break;
        /*check for equals operator*/
        case'=':

				c = getc(fin);
				if(c == '=')
				{
					return EQUALS;
				}

					ungetc(c, fin);
					lexical_error();
				break;
    /*check for not operator*/
		case '!':

				c = getc(fin);
				if(c == '=')
				{
					return NOTEQ;
				}

					ungetc(c, fin);
					lexical_error();
				break;
        /* check for less than operator*/
		case '<':

				c = getc(fin);
				if(c == '=')
				{
					return LESSEQ;
				}

					ungetc(c, fin);
					return LESS;
				break;

		case '>':   /*return greater than or greater or equal to*/

				c = getc(fin);
				if(c == '=')
				{
					return GREATEQ;
				}

					ungetc(c, fin);
					return GREATER;
				break;

         }
         }
}


/*****************************************************************************/

/*clear the buffer*/
void clear_buffer()
{
	token_ptr = 0;
	token_buffer[token_ptr] = '\0';
}

/*****************************************************************************/

/*appends the character to buffer*/
void buffer_char(char c)
{
	token_buffer[token_ptr] = c;
	token_ptr = token_ptr + 1;
	token_buffer[token_ptr] = '\0';
}

/*****************************************************************************/

/*checks whether buffer is a reserved word or identifier*/
token check_reserved()
{
	if(strcmp(token_buffer, "main") == 0)return MAIN;
	if(strcmp(token_buffer, "if") == 0)return IF;
	if(strcmp(token_buffer, "else") == 0)return ELSE;
	if(strcmp(token_buffer, "while") == 0)return WHILE;
    if(strcmp(token_buffer, "read") == 0)return WHILE;
	if(strcmp(token_buffer, "write") == 0)return WRITE;

		return ID;
}

/*****************************************************************************/

/*reports lexical error and sets error flag*/
void lexical_error()
{
	printf("lexical error in line %d/n", line_num);
	error = TRUE;
}

/*****************************************************************************/

/*parses source file*/
void parser()
{
	next_token = scanner();
	program();
	match(SCANEOF);
}

/*****************************************************************************/

/*parses a program*/
/* <program> --> main{<stmtlist>}*/
void program()
{
	match(MAIN);
	match(LCURL);
	statement_list();
	match(RCURL);
}

/*****************************************************************************/

/*parses list of statements*/
/* <stmtlist> --> <stmt>{<stmt>}*/
void statement_list()
{
	statement();
	while(TRUE)
	{
		if(next_token == ID ||
         next_token == READ ||
         next_token == WRITE||
         next_token == IF   ||
         next_token == ELSE ||
         next_token == WHILE)
         statement();
		else
			break;
	}
}

/*****************************************************************************/

/*parses one statement*/
/* 	<stmt> -->	id:=<expr>;
	<stmt> -->	read(<idlist>);
	<stmt> -->	write(<idlist>);
	<stmt> -->	if(<idlist>){<stmtlist>}{else(<idlist>){<stmtlist>}};
	<stmt> -->	while(<idlist>){<stmtlist>};
*/
void statement()
{
	if(next_token == ID)
	{
		match(ID);
		match(ASSIGNOP);
		expression();
		match(SEMICOLON);
	}
	else if(next_token == READ)
	{
		match(READ);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);
	}
	else if(next_token == WRITE)
	{
		match(WRITE);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);

	}
	else if(next_token == IF)
	{
		match(IF);
		match(LPAREN);
		bool();
		match(RPAREN);
		match(LCURL);
		statement_list();
		match(RCURL);
		if(next_token == ELSE)
		{
	 		match(ELSE);
			match(LCURL);
			statement_list();
			match(RCURL);
		}

	}
	else if(next_token == WHILE)
	{
		match(WHILE);
		match(LPAREN);
		bool();
		match(RPAREN);
		match(LCURL);
		statement_list();
		match(RCURL);
	}
	else
		syntax_error();
}

/*****************************************************************************/

/*parses list of identifiers*/
/* <idlist> --> id{, id}*/
void id_list()
{
	match(ID);
	while(next_token == COMMA)
	{
		match(COMMA);
		match(ID);
	}
}

/*****************************************************************************/

/*parses a boolean expression*/
/* <bool> --> (id|integer)<relop>(id|integer)*/
void bool()
{
	if (next_token == ID)
		match(ID);
	else
		match(INTLITERAL);
	relop();
	if (next_token == ID)
		match(ID);
	else
		match(INTLITERAL);
}
/*****************************************************************************/

/*parses list of expressions*/
/* <explist> --> <exp>,{,<exp>}*/
/*Note this hold over from Micro is not used*/
void expression_list()
{
	expression();
	while(next_token == COMMA)
	{
		match(COMMA);
		expression();
	}
}

/*****************************************************************************/

/*parses one expression*/
/* <exp> --> <term>{<adop><term>}*/
void expression()
{
	term();
	while(next_token == PLUSOP || next_token == MINUSOP)
	{
		add_op();
		term();
	}
}

/*****************************************************************************/

/*parses one term*/
/*	<term> --> id
	<term> --> integer
	<term> --> <term>
	<term> --> (<expr>)
	<term> --> <term> <multop> <term>*/
void term()
{
	if(next_token == ID)
	{
		match(ID);
		if (next_token == MULTOP || next_token == DIVOP)
		{
			mult_op();
			term();
		}
	}
	else if(next_token == INTLITERAL)
	{
		match(INTLITERAL);
		if (next_token == MULTOP || next_token == DIVOP)
		{
			mult_op();
			term();
		}
	}
	else if(next_token == LPAREN)
	{
		match(LPAREN);
		expression();
		match(RPAREN);
		if (next_token == MULTOP || next_token == DIVOP)
		{
			mult_op();
			term();
		}
	}
	else
		syntax_error();
}

/*****************************************************************************/

/*parses plus or minus operator*/
/* <adop> --> +|-
*/
void add_op()
{
	if(next_token == PLUSOP || next_token == MINUSOP)
		match(next_token);
	else
		syntax_error();
}

/*****************************************************************************/

/*parses multiplication or division operator*/
/* <multop> --> *|/
*/
void mult_op()
{
	if(next_token == MULTOP || next_token == DIVOP)
		match(next_token);
	else
		syntax_error();
}

/*****************************************************************************/

/*parses boolean operators*/
/* <relop> --> <|<=|>|>=|==|!=
*/
void relop()
{
	if(next_token == GREATER ||
       next_token == GREATEQ ||
       next_token == LESS    ||
       next_token == LESSEQ  ||
       next_token == EQUALS  ||
       next_token == NOTEQ)
		match(next_token);
	else
		syntax_error();
}

/*****************************************************************************/

/*checks whether the expected token and the actual token match,
and also reads the next token from source file
*/
void match(token tok)
{
	if(tok == next_token)
		;
	else
		syntax_error();
	next_token = scanner();
}

/*****************************************************************************/

/*reports syntax error*/
void syntax_error()
{
	printf("syntax error in line %d error with token%s\n", line_num,tokens[next_token]);
	error = TRUE;
}

/*****************************************************************************/
void parse_file()
{
  char input_file[50];
  printf("Please enter a file to parse:\n");
  scanf("%s", input_file);

  fin = fopen(input_file, "r");
  parser();

  fclose(fin);

  if (!error)
    printf("Parsing successful!\n");
}
void scan_file(){
            int linecounter = 1;	//keep track of line count for file output
            token tok;				//keep track of current token
            char input_file[50];	//input file name
	        char output_file[50];	//output file name
			printf("Enter the input file name: ");
			scanf("%s",input_file);
			fin = fopen(input_file, "r");
			printf("Enter the output file name: ");
			scanf("%s",output_file);
			fout = fopen(output_file, "w");

			//scan each token and output to user entered file
			do
			{
				tok = scanner();
				//go to next line
				if (linecounter < line_num)
				{
					fprintf(fout,"\n");
					linecounter = line_num;
				}
				fprintf(fout,"%s ",tokens[tok]);
			}while(tok != SCANEOF); //quit when EOF reached
			fclose(fin); //close file
			fclose(fout); //close file
			printf("File successfully scanned\n");
}
int main()
{


	int selection;			//user's menu selection

	printf("Please Select an Option\n");
	printf("1-Scan and Output Tokens to a File\n");
	printf("2-Parse a File\n");
	printf("3-Quit\n");

	scanf("%d",&selection);	//get user's selection

	switch(selection)
	{
		case 1: scan_file(); break;
        case 2: parse_file(); break;
        default:
        {
           printf("Invalid option %d\n", selection);
           return 0;
        }
  }
  return 0;
}





