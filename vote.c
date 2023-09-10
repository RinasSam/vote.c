/*
 * vote.c: a simple C program that allows users to vote with multiple runs.
 *    
 * Copyright (C) 2023 RinasSam
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* Very important definition cuz we need a function that needs this definition */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* States for controlling code flow. */
enum state {
	S_STANDBY,
	S_VOTE,
	S_POSTVOTE
};


/* We use global variables for ease of use.*/
enum state curr_state;
char options[256][256];
int votes[256] = {0};
int count = 0;

/* Convert string to lower case. */
char 
*lowercase_str(char str[256])
{
	int i = 0;
	for(i; str[i-1]; i++){
		str[i] = tolower(str[i]);
	}
	return str;
}



/* 
 * The purpose of this function is to handle states and call the proper function for each state. 
 * This is seperate from main() because it helps us reuse the function as much as needed.
 */

void state_handler();




/* This function handles VOTE mode (i.e. the actual program). */
void
vote_mode()
{
	char buffer[256];
	char *command = NULL;
	printf("\nV>");

	/* Read command from STDIN and convert it to lowercase. */
	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {	
		command = lowercase_str(buffer);

		/* Switch to STANDBY Mode. */
		if(strncmp(command, "exit\n", 5) == 0 || strncmp(command, "quit\n", 5) == 0 || strncmp(command, "q\n", 2) == 0) {
			printf("\n\n--SWITCHING TO STANDBY MODE--\n\n");
			return;
		}

		else if(strncmp(command, "help\n", 5) == 0 || strncmp(command, "h\n", 2) == 0)
		{
			printf("\nYou are now in VOTE_MODE. Available commands:\n");
			printf("\nquit (q): return to STANDBY_MODE");
			printf("\nhelp (h): display this message.");
			printf("\n\nstart (s): start a vote. This commands REQUIRES A FILE TITLED EXACTLY `options.txt\' IN\n");
			printf("THE SAME DIRECTORY AS THE PROGRAM, WHICH INCLUDES OPTIONS SEPERATED BY NEWLINES.\n");
			printf("After typing this command, what you must do is type the line numbers corresponding to\n");
			printf("the options available (ranges are accepted). For instance: `1 2 5-7, 12\'\n");
			printf("You need to do this for each voter. When you are done, type `done\'.\n");
		}
		


		/*
		 * This is the nitty gritty part of the program. The most difficult to probably implement.
		 *
		 */

		else if(strncmp(command, "start\n", 6) == 0 || strncmp(command, "s\n", 2) == 0) {
			if(count == 0) {
				FILE *foptions = fopen("options.txt", "r");
				
				if(foptions == NULL) {
					perror("\nERROR");
					fprintf(stderr, "Make sure that `options.txt\' exists, is in the same directory as\nprogram, and is named exactly `options.txt\' with no capital letters.\n");
					exit(-1);
				}
				printf("\n\n--BEGINNING VOTE--\n\n");
	
	
				/* We begin by reading line by line. */
				char line[256];
				while(fgets(line, sizeof(line), foptions) != NULL) {
	
					if(strnlen(line, 256) >= 2) {
						/* Remove trailing \n */
						line[strcspn(line, "\n")] = 0;
						strncpy(options[count], line, 256);
						count++;					
					}
				}
				fclose(foptions);
			} else {
				int m = 0;
				for(m; m < count; m++) {
					votes[m] = 0;
				}
			}

			/* Print candidates and their ID */
			printf("\nThere are %d options. Showing list alongside numbers:\n\n", count+1);
			printf("%-10s %-50s\n", "ID:", "Option:");

			int i = 1;
			for(i; i <= count; i++) {
				
				printf("%-10d %-50s\n", i, options[i-1]);
			}
			
			printf("\nPlease begin voting. Begin inputting numbers corresponding to the option ID,\nand type `done\' or `d\' to finish.");

			/* Here we take a sequence of numbers. We test if there is a number range, and we do that.*/
			char sequence[256];
			char *token;
			char *subtoken;
			int old_num = -1;
			int lower_bound = 0;
			int upper_bound = 0;
			printf("\nN>");
			

			while(fgets(sequence, sizeof(sequence), stdin) != NULL) {
				
				old_num = -1; /* This values needs to be reset. */

				/* Go to POSTVOTE Mode. */
				if(strncmp(sequence, "done\n", 5) == 0 || strncmp(sequence, "d\n", 2) == 0) {
					break;
				}

				/* Break down the input into small tokens. */
				token = strtok(sequence, " ");
				while(token != NULL) {

					/* Check if a range exists. */
					if(strchr(token, '-') != NULL) {	
						subtoken = strtok(token, "-"); 
					} else {
						subtoken = NULL;
					}

					/* Handle range of numbers. */
					if(subtoken != NULL) {
						
						lower_bound = atoi(subtoken);

						if(lower_bound < 0) {
							fprintf(stderr, "\nERROR: INVALID NUMBER RANGE. Try again!\n");
							break;
						}

						subtoken = strtok(NULL, "-");
						if(subtoken == NULL) {
							fprintf(stderr, "\nERROR: INVALID NUMBER RANGE. Try again!\n");	
							break;
						}

						upper_bound = atoi(subtoken);
						

						lower_bound -= 1;
						upper_bound -= 1;

						if(upper_bound < 0 || upper_bound > count || lower_bound < 0) {
							fprintf(stderr, "\nERROR: INVALID NUMBER RANGE. Try again!\n");
							break;
						} else {
							
							if(lower_bound > upper_bound) {
								lower_bound += upper_bound;
								upper_bound = lower_bound - upper_bound;
								lower_bound -= upper_bound;
							}
							
							for(lower_bound; lower_bound <= upper_bound; lower_bound++)
							{

								if(lower_bound != old_num) {
									votes[lower_bound] += 1;
									old_num = lower_bound;
									printf("\n%d votes for %s!", votes[lower_bound], options[lower_bound]);
								}
							}
						 	break;

						}

					/* Handle singular numbers */
					} else if (token != NULL) {

						lower_bound = atoi(token);
						

						lower_bound -= 1;
						if(lower_bound < 0 || lower_bound > count) {
							fprintf(stderr, "\nERROR: INVALID NUMBER INPUT. Try again!\n");
							break;
						} else {
							if(lower_bound != old_num) {
								votes[lower_bound] += 1;
								old_num = lower_bound;
								printf("\n%d votes for %s!", votes[lower_bound], options[lower_bound]);
							}
						}
					}
					token = strtok(NULL, " ");					
				}
				printf("\nN>");
				
			}
			
			printf("\n\n--SWITCHING TO POSTVOTE_MODE--\n");
			curr_state = S_POSTVOTE;
			state_handler(curr_state);


		}
		
		else {
			printf("\nUnrecognized or misspelled command. Try again or type `help\' for help.\n");
		}

		
		printf("\nV>");

	}

}




/* This function handles POSTVOTE mode (i.e. the results). */
void
postvote_mode()
{
	char buffer[256];
	char *command = NULL;
	
	printf("\nP>");

	/* Read command from STDIN and convert it to lowercase. */
	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {	
		command = lowercase_str(buffer);

		/* Exit gracefully. */
		if(strncmp(command, "exit\n", 5) == 0 || strncmp(command, "quit\n", 5) == 0 || strncmp(command, "q\n", 2) == 0) {
			printf("\n\n--SWITCHING TO VOTE_MODE--\n");
			return;
		}
		
		else if(strncmp(command, "display\n", 8) == 0 || strncmp(command, "d\n", 2) == 0) {
			printf("\n%-50s %-3s", "Options:", "Votes:\n");
			
			int k = 0;
			for(k; k < count; k++) {
				printf("%-50s %-3d\n", options[k], votes[k]);
			}

		}

		
		else if(strncmp(command, "runoff\n", 7) == 0 || strncmp(command, "r\n", 2) == 0) {

			printf("\nPlease enter the cutoff value (i.e. options with votes lower than this number will be removed): ");
			char str[256];
			int cutoff = 0;

			while(fgets(str, sizeof(str), stdin) != NULL) {
				cutoff = atoi(str);
				if(cutoff < 0 || cutoff >= 256) {
					fprintf(stderr, "ERROR: INVALID VALUE! Try again: ");
				} 

				else if(cutoff == 0 && (strncmp(str, "0\n", 2) != 0)) {
					fprintf(stderr, "ERROR: INVALID VALUE! Try again: ");
				}
				else {
					break;
				}
			}
			

			int i = 0;
			int k = 0;
			int j = 0;
			int l = 0;

			char temp[256][256];
			int tempvote[256] = {0};

			for(i; i < count; i++) {
				
				if(votes[i] >= cutoff) {
					strncpy(temp[k], options[i], 256);
					tempvote[k] = votes[i];
					k++;
				} else {
					l++;
				}
			}
			
			count -= l;	
			for(j; j < 256; j++) {
				
				votes[j] = tempvote[j];				
				strncpy(options[j], temp[j], 256);
			}
			printf("\nSuccessfully shaved off %d options! Type `display\' or `d\' to view the new results!\n", l);
			
		}


		else if(strncmp(command, "help\n", 5) == 0 || strncmp(command, "h\n", 2) == 0) {
			printf("\nYou are now in POSTVOTE_MODE. Available commands:\n");
			printf("\nquit (q): return to VOTE_MODE.");
			printf("\nhelp (h): display this message.");
			printf("\ndisplay (d): displays the last vote's results.");
			printf("\nsave (s): save results to `results.txt\' on your computer.\n");
			printf("\n\nrunoff (r): hold another vote, but eliminate the lowest scoring options.\n");
			printf("By default, this command takes a cutoff range and deletes every option with less votes.\n");
		}

		else if(strncmp(command, "save\n", 5) == 0 || strncmp(command, "s\n", 2) == 0) {
			FILE *fsave = fopen("results.txt", "w");
			if(fsave == NULL) {
				perror("ERORR");
				fprintf(stderr, "\nCOULD **NOT** SAVE!");
				exit(-2);
			}
			
			fprintf(fsave, "\n%-50s %-3s", "Options:", "Votes:\n");
			
			for(int k = 0; k < count; k++) {
				fprintf(fsave, "%-50s %-3d\n", options[k], votes[k]);
			}
			fclose(fsave);
			printf("\nFinished save!\n");


		}

		else if(command == NULL)
		{
			continue;
		}

		else {
			printf("\nUnrecognized or misspelled command. Try again or type `help\' for help.\n");
		}

		printf("\nP>");
	} 
}







/* This function handles STANDBY mode (i.e. the "start menu"). */
void
standby_mode()
{
	char buffer[256];
	char *command = NULL;
	printf("\nS>");

	/* Read command from STDIN and convert it to lowercase. */
	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {	
		command = lowercase_str(buffer);

		/* Exit gracefully. */
		if(strncmp(command, "exit\n", 5) == 0 || strncmp(command, "quit\n", 5) == 0 || strncmp(command, "q\n", 2) == 0) {
			printf("\nTake care!\n");
			return;
		}
		
		else if(strncmp(command, "vote\n", 5) == 0 ||  strncmp(command, "v\n", 2) == 0) {
			printf("\n\n--SWITCHING TO VOTE_MODE--\n");
			curr_state = S_VOTE;
			state_handler();
		}

		/* The following are all information commands, not related to the actual functionality of said program. */
		else if(strncmp(command, "help\n", 5) == 0 || strncmp(command, "h\n", 2) == 0) {
			printf("\nYou are now in STANDBY_MODE. Available commands:\n");
			printf("\nquit (q): exit the program.");
			printf("\nhelp (h): display this message.");
			printf("\nwarranty (w): display warranty information about the program.");
			printf("\ncopying (c): display information about copying this program and using its code.\n");
			printf("\n\nvote (v): hold a new vote. (this command will switch you to VOTE_MODE. Commands\n");
			printf("there differ. Use `help\' after you use `vote\')\n");
		}

		else if(strncmp(command, "copying\n", 8) == 0 || strncmp(command, "c\n", 2) == 0 ) {
			printf("\nYou may convey verbatim copies of the Program's source code as you receive it,\n");
			printf("in any medium, provided that you conspicuously and appropriately publish on each copy\n");
			printf("an appropriate copyright notice; keep intact all notices stating that this License and\n");
			printf("any non-permissive terms added in accord with section 7 apply to the code; keep intact\n");
			printf("all notices of the absence of any warranty; and give all recipients a copy of the license\n");
			printf("along with the Program.\n");
			printf("\nYou may charge any price or no price for each copy that you convey, and you may offer\n");
			printf("support or warranty protection for a fee.\n");
			printf("\nSee the file titled `LICENSE\' for more details. Particurly Section 4 and beyond.\n");
		}

		else if(strncmp(command, "warranty\n", 9) == 0 || strncmp(command, "w\n", 2) == 0 ) {
			printf("vote.c is a simple C program for holding votes.\n");
			printf("\nTHERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.\n");
			printf("EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n");
			printf("PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED,\n");
			printf("INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n");
			printf("FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE\n");
			printf("PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL\n");
			printf("NECESSARY SERVICING, REPAIR OR CORRECTION.\n");
			printf("\nIN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY\n");
			printf("COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS THE PROGRAM AS\n");
			printf("PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,\n");
			printf("INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE\n");
			printf("PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE\n");
			printf("OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH\n");
			printf("ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY\n");
			printf("OF SUCH DAMAGES.\n");
			printf("\nSee the file titled `LICENSE\' for more details.");
		}

		else if(command == NULL)
		{
			continue;
		}

		else {
			printf("\nUnrecognized or misspelled command. Try again or type `help\' for help.\n");
		}

		printf("\nS>");
	} 
}



int
main(int argc, char *argv[])
{
	/* Print short notice. */
	printf("vote.c  Copyright (C) 2023  RinasSam\nThis program comes with ABSOLUTELY NO WARRANTY; for details type `warranty\'.\nThis is free software, and you are welcome to redistribute it\nunder certain conditions; type `copying\' for details.\n");

	/* Start in STANDBY mode. */
	curr_state = S_STANDBY;
	state_handler();

	return 0;
}

void 
state_handler()
{
	switch(curr_state)
	{
	case S_STANDBY:
		printf("\nYou are now in STANDBY_MODE. Available commands:\n");
		printf("\nquit (q): exit the program.");
		printf("\nhelp (h): display this message.");
		printf("\nwarranty (w): display warranty information about the program.");
		printf("\ncopying (c): display information about copying this program and using its code.\n");
		printf("\n\nvote (v): hold a new vote. (this command will switch you to VOTE_MODE. Commands\n");
		printf("there differ. Use `help\' after you use `vote\')\n");
		standby_mode();
		break;

	case S_VOTE:
		printf("\nYou are now in VOTE_MODE. Available commands:\n");
		printf("\nquit (q): return to STANDBY_MODE");
		printf("\nhelp (h): display this message.");
		printf("\n\nstart (s): start a vote. This commands REQUIRES A FILE TITLED EXACTLY `options.txt\' IN\n");
		printf("THE SAME DIRECTORY AS THE PROGRAM, WHICH INCLUDES OPTIONS SEPERATED BY NEWLINES.\n");
		printf("After typing this command, what you must do is type the line numbers corresponding to\n");
		printf("the options available (ranges are accepted). For instance: `1 2 5-7, 12\'\n");
		printf("You need to do this for each voter. When you are done, type `done\'.\n");
		vote_mode();
		break;

	case S_POSTVOTE:
		printf("\nYou are now in POSTVOTE_MODE. Available commands:\n");
		printf("\nquit (q): return to VOTE_MODE.");
		printf("\nhelp (h): display this message.");
		printf("\ndisplay (d): displays the last vote's results.");
		printf("\nsave (s): save results to `results.txt\' on your computer.\n");
		printf("\n\nrunoff (r): hold another vote, but eliminate the lowest scoring options.\n");
		printf("By default, this command deletes every option with 0 votes, and then the 5 lowest.\n");
		postvote_mode();
		break;
	}
}
