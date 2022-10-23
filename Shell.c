#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

int MAX_CHAR = 2048; // Max number of chars in a command line
int MAX_CMD = 512; // Max number of commands 
int NEW_LINE_CHAR_VALUE = 10; // "\n" ASCII Value
int CATCH_VAR = 0; // For catching SIGTSTP
int BG_ARRAY_PLACEMENT = 0; // Keeps track of the pid_t placements in the array

// ================================================================================================
/*
Struct: command
char * commandline      The users entire command command line
char * command          The users command
char * parameters		A string of the parameters
char * inputFile
char * outputFile
int backgroundValue     The value is to check if '&' is in the command if so run in the background (1 = TRUE / 0 = FALSE)
*/
struct command {
	char* commandLine;
	char* command;
	char* parameters;
	char* inputFile;
	char* outputFile;
	int backgroundValue;
};

// Headers ========================================================================================
char* getCommandLine();
struct command* fillCommand(pid_t smallshPID);
void processCommandLine(struct command* currCommand, int smallshPID, int killCount, pid_t backgroundIdArray[]);
void sigtstpHandler(int signal);
void cdCommand(struct command* currCommand);
void statusCommand(int stat);
void exitCommand();
// ================================================================================================

// ================================================================================================
/*
char * getCommandLine
Reads the current line in the script and returns a char pointer of the line
Parameters: ---
Returns: char * cmdLine
*/
char* getCommandLine() {
	// Initialize Values
	char* cmdLine;
	char* currentLine = NULL;
	size_t maxChar = MAX_CHAR;

	// Setting the cmdLine to NULL and ensures it hold the correct ammount of chars
	cmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	printf(": ");
	fflush(stdout);

	// Gets the current Command Line
	getline(&currentLine, &maxChar, stdin);
	cmdLine = currentLine;

	return cmdLine;
}
// ================================================================================================
/*
struct command * fillCommand
Fills a command struct with the information given from the current command line then returns the
struct so that it can be used to process the command line
Parameters: pid_t smallshPID
Returns: currCommand
*/
struct command* fillCommand(pid_t smallshPID) {
	// Initialize command struct
	struct command* currCommand = malloc(sizeof(struct command));
	// Initialize values
	char* currCmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	char* tempCmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	char* temp = calloc(MAX_CHAR + 1, sizeof(char));
	char* null = "\0";

	int parameterLength = 0;
	int commandLength = 0;
	int cmdLineLength = 0;

	// Setting all of struct to null
	currCommand->commandLine = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->commandLine, null);
	currCommand->command = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->command, null);
	currCommand->parameters = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->parameters, null);
	currCommand->inputFile = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->inputFile, null);
	currCommand->outputFile = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->outputFile, null);
	currCommand->backgroundValue = 0;


	// Copys the current command line into currCmdLine and fills out struct
	strcpy(currCmdLine, getCommandLine());

	// Places the currCmdLine into the currCommand->commandLine
	currCommand->commandLine = calloc(strlen(currCmdLine) + 1, sizeof(char));
	strcpy(currCommand->commandLine, currCmdLine);
	strcpy(tempCmdLine, currCmdLine);

	// Checks to see if the command was starting with '#' if so apply ignore command else continue normally
	if (currCommand->commandLine[0] == '#') {
		currCommand->command = "ignore";
	}
	else {
		// Gets the length of the full command line
		cmdLineLength = strlen(currCmdLine);

		// This makes a temporary command line
		temp = calloc(strlen(currCmdLine) + 1, sizeof(char));
		strcpy(temp, currCmdLine);

		// Finds the command by tokenizing until ' '
		char* token = strtok(temp, " ");
		currCommand->command = calloc(strlen(token) + 1, sizeof(char));
		strcpy(currCommand->command, token);

		// Places the rest of the line in the parameters
		commandLength = strlen(currCommand->command) + 1;

		// If they are equal that means there is no parameters so change name of command to echo and have parameter of " "
		if (token[commandLength - 2] == '\n') {
			if (token[0] == 'e') {
				currCommand->command = "echo";
				currCommand->parameters = " ";
			}
			if (token[0] == 'l') {
				currCommand->command = "ls";
				currCommand->parameters = "\0";
			}
		}
		// We make token the command line then we delete the command from it
		else {
			token = calloc(strlen(tempCmdLine) + 1, sizeof(char));
			strcpy(token, tempCmdLine);
			memmove(token, token + commandLength, strlen(token));
			currCommand->parameters = calloc(strlen(token) + 1, sizeof(char));
			strcpy(currCommand->parameters, token);
		}

		// Searches at the last value for an &
		parameterLength = strlen(token);
		if (token[parameterLength - 2] == '&')
			currCommand->backgroundValue = 1;
		else
			currCommand->backgroundValue = 0;

		// Checks for the '$$' to change it into the PID
		if (strstr(currCommand->parameters, "$$") != NULL) {
			// PID of the smallsh into a char and storing it int cpid
			char* charPID = calloc(MAX_CHAR + 1, sizeof(char));
			sprintf(charPID, "%d", smallshPID);

			// Tokenizing the first part the parameters then cating the pid to the token
			char* moneyToken = strtok(currCommand->parameters, "$$");
			strcat(moneyToken, charPID);

			// Making the currCommand->parameters = to the parameters with $$ replaced by the PID of smallsh
			currCommand->parameters = calloc(strlen(moneyToken) + 1, sizeof(char));
			strcpy(currCommand->parameters, moneyToken);
		}

	}
	return currCommand;
}
// ================================================================================================
/*
void processCommandLine
Takes the current command struct and processes it corresponding to whats been filled in the
fillCommand function
Parameters: struct command* currCommand, int smallshPID, int killCount
Returns: ---
*/
void processCommandLine(struct command* currCommand, int smallshPID, int killCount, pid_t backgroundIdArray[]) {
	// Initialize a temp char 
	char* temp = calloc(MAX_CHAR + 1, sizeof(char));
	pid_t spawnpidsleep = -5;

	// Echo Command -------------------------------------------------------------------------------
	if (strcmp(currCommand->command, "echo") == 0) {
		// Tokenizes the parameters untill the '\n' and echos it out unless thats the only value
		if (strcmp(currCommand->parameters, " ") != 0) {
			char* echoToken = strtok(currCommand->parameters, "\n");
			currCommand->parameters = calloc(strlen(echoToken) + 1, sizeof(char));
			strcpy(currCommand->parameters, echoToken);
		}
		// Executes echo with the given parameters
		char* newargv[] = { "/bin/echo", currCommand->parameters, NULL };
		execv(newargv[0], newargv);
	}

	// Ignore Command -----------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "ignore") == 0) {
		// Do nothing and ignore the line
		exit(0);
	}

	// Ls Command ---------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "ls") == 0) {
		// If the command line is just ls then run ls
		if (strcmp(currCommand->parameters, "\0") == 0) {
			char* newargv[] = { "/bin/ls", NULL };
			execv(newargv[0], newargv);
		}
		// If there is more to the command line that means theres output files invloved
		else {
			// Parse the parameters so just the output file is left then store that in currCommand->outputFile
			temp = currCommand->parameters;
			char* token2 = strtok(temp, "\n");
			memmove(token2, token2 + 2, strlen(token2));
			currCommand->outputFile = token2;

			// Outsource the next data into currCommand->outputFile
			int out = open(currCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out, 1); // http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html

			// Writing the information to be outsourced
			char* newargv[] = { "/bin/ls", NULL };
			execvp(newargv[0], newargv);
		}
	}

	// PWD Command --------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "pwd\n") == 0) {
		// Create a buffer and the current working directory
		char* buffer2 = calloc(MAX_CHAR + 1, sizeof(char));
		char* workingPath = calloc(MAX_CHAR + 1, sizeof(char));
		strcpy(workingPath, getcwd(buffer2, MAX_CHAR));

		// Print the current working directory out
		printf("%s\n", workingPath);
		fflush(stdout);
		exit(0);
	}

	// Cat Command --------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "cat") == 0) {
		// Find the file name of whats going to be cated
		temp = currCommand->parameters;
		char* token2 = strtok(temp, "\n");

		// Execute the cat command
		char* newargv[] = { "/bin/cat", token2, NULL };
		execvp(newargv[0], newargv);
	}

	// Wc Command =--------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "wc") == 0) {
		// Checks to see if the parameters have both '<' and '>'
		if (strchr(currCommand->parameters, '<') != NULL && strchr(currCommand->parameters, '>') != NULL) {
			// Parse the paramters to find the file name and store in currCommand->inputFile and currCommand->outputFile
			temp = currCommand->parameters;
			char* token2 = strtok(temp, "\n");
			char* token3 = calloc(strlen(token2) + 1, sizeof(char));
			strcpy(token3, token2);

			// Input file name stored in currCommand->inputFile
			memmove(token2, token2 + 2, strlen(token2));
			token2 = strtok(temp, " ");
			currCommand->inputFile = calloc(strlen(token2) + 1, sizeof(char));
			strcpy(currCommand->inputFile, token2);

			int inputLength = strlen(currCommand->inputFile);

			// Output file name stored in currCommand->outputFile
			memmove(token3, token3 + inputLength + 5, strlen(token3));
			currCommand->outputFile = calloc(strlen(token3) + 1, sizeof(char));
			strcpy(currCommand->outputFile, token3);

			// Opens input and output files
			int in = open(currCommand->inputFile, O_RDONLY);
			int out = open(currCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

			dup2(in, 0);
			dup2(out, 1);

			char* newargv[] = { "wc", currCommand->inputFile, NULL };
			execvp(newargv[0], newargv);

		}
		// Checks to see if the parameter has only the '<' and not '>'
		else if (strchr(currCommand->parameters, '<') != NULL) {
			// Parse the paramters to find the file name and store in currCommand->inputFile
			temp = currCommand->parameters;
			char* token2 = strtok(temp, "\n");
			memmove(token2, token2 + 2, strlen(token2));
			currCommand->inputFile = token2;

			// Create input
			int in = open(currCommand->inputFile, O_RDONLY);
			dup2(in, 0);

			char* newargv[] = { "/bin/wc", currCommand->inputFile, NULL };
			execvp(newargv[0], newargv);
		}
		exit(0);
	}

	// Test Command -------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "test") == 0) {
		// Tokenizing to the type of test to make (in this assignment its '-f') 
		char* testToken = strtok(currCommand->parameters, " ");
		strcpy(currCommand->parameters, testToken);

		// Executes the test command ( | /bin/test -f badfile | this returns a fail so exit value should be 1)
		char* newargv[] = { "/bin/test", currCommand->parameters, "badfile", NULL };
		execvp(newargv[0], newargv);

	}

	// Status Command -----------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "status") == 0) {
		// Done in statusCommand
	}

	// Bad file
	else if (strcmp(currCommand->command, "badfile\n") == 0) {
		// Tries to open badfile, this file should not exist so it should return an error 
		FILE* file;
		file = fopen(currCommand->command, "r");
		if (file == NULL)
			perror("badfile: ");
	}

	// Sleep command ------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "sleep") == 0) {
		// Checks to see if the foreground-only mode is in effect
		if (killCount == 1) {
			strtok(currCommand->parameters, " ");

			char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
			execvp(newargv[0], newargv);
		}
		// Foreground-only mode not on
		else {
			// Gets rid of the new line character in the parameters so it can execute
			strtok(currCommand->parameters, "\n");
			// Checks to see if its a background command
			if (currCommand->backgroundValue == 1) {
				// Tokenize the currCommand->parameters so that it holds the number of seconds to sleep
				strtok(currCommand->parameters, " ");

				// Initializes forking values
				pid_t ogPID;
				int backgroundStatus = 0;
				int bgStat = 0;
				ogPID = getpid();

				// Forks and because its in the background it doesnt wait for it to end
				spawnpidsleep = fork();
				pid_t bPID;
				bPID = getpid();
				// This checks to see if its the child of the child
				if (bPID - ogPID == 1) {
					printf("Background PID is |%d|\n", bPID);
					fflush(stdout);

					// Stores the background process id in the global array for the pid_t's and increment the arrays placement so it doesnt override any other background process id's
					backgroundIdArray[BG_ARRAY_PLACEMENT] == bPID;
					BG_ARRAY_PLACEMENT++;



					char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
					execvp(newargv[0], newargv);
				}

			}
			else {
				// If foreground-only mode is not on and its not background conduct a normal sleep command
				strtok(currCommand->parameters, "\n");

				char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
				execvp(newargv[0], newargv);
			}
		}
		exit(0);
	}

	// Pkill Command ------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "pkill") == 0) {
		char* pkillToken = strtok(currCommand->parameters, "\n");
		currCommand->parameters = calloc(strlen(pkillToken) + 1, sizeof(char));
		strcpy(currCommand->parameters, pkillToken);

		char* newargv[] = { "/bin/pkill", currCommand->parameters, NULL };
		execvp(newargv[0], newargv);
	}

	// Cd Command to go to home dir ---------------------------------------------------------------
	else if (strcmp(currCommand->command, "cd\n") == 0) {
		// Ignore done in cdCommand
	}

	// Cd command to go to specifc dir ------------------------------------------------------------
	else if (strcmp(currCommand->command, "cd") == 0) {
		// Ignore done in cdCommand
	}

	// Mkdir Command ------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "mkdir") == 0) {
		// Creates a directory with full mode
		mkdir(currCommand->parameters, 0777);
		printf("\n");
		fflush(stdout);
	}

	// Date Command -------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "date\n") == 0) {
		// Showcases the current date
		char* newargv[] = { "/bin/date", NULL };
		execvp(newargv[0], newargv);
	}

	// Kill Command -------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "kill") == 0) {
		// Makes the smallshPID into a char *
		char* PIDc = calloc(MAX_CHAR + 1, sizeof(char));
		sprintf(PIDc, "%d", smallshPID);

		// This tokenizes so that the currCommand->parameter holds the type of signal to kill 
		char* killToken = strtok(currCommand->parameters, " ");
		currCommand->parameters = calloc(strlen(killToken) + 1, sizeof(char));
		strcpy(currCommand->parameters, killToken);
		memmove(currCommand->parameters, currCommand->parameters + 1, strlen(currCommand->parameters));

		// Checks to see if currCommand->parameters is "SIGTSTP"
		if (strcmp(currCommand->parameters, "SIGTSTP") == 0) {
			if (signal(SIGTSTP, sigtstpHandler) == SIG_ERR) {
				printf("Signal handler failed\n");
				fflush(stdout);
				exit(1);
			}

			// If killCount is even that means that foreground-only mode is off, so turn it on
			if (killCount % 2 == 0)
				CATCH_VAR = 1;

			// Checks to see if entering or exiting foreground only mode
			if (CATCH_VAR) {
				printf("Entering foreground-only mode (& is now ignored)\n");
				fflush(stdout);
			}
			else {
				printf("Exiting foreground-only mode\n");
				fflush(stdout);
			}
		}
	}

	// CATCH --------------------------------------------------------------------------------------
	else {
		/* This happens only if there is a command asked for that has not been implemented
		 Also to catch any children that made it all the way down here to stop them in case of an
		error occuring
		*/
		printf("CURR CMD |%s|\n", currCommand->command);
		fflush(stdout);
		printf("CURR PAR |%s|\n", currCommand->parameters);
		fflush(stdout);
		char* newargv[] = { "/bin/echo", "This command has not been implemented", NULL };
		execv(newargv[0], newargv);
	}
}
// ================================================================================================
/*
void sigtstpHandler
Takes the signal and changes the value of CATCH_VAR
Parameters: int signal
Returns: ---
*/
void sigtstpHandler(int signal) {
	if (signal == SIGTSTP) {
		if (CATCH_VAR)
			CATCH_VAR = 0;
		else
			CATCH_VAR = 1;
	}
}
// ================================================================================================
/*
void sigintCatcher
Ignores the SIGINT signal
Parameters: int sig
Returns: ---
*/
void sigintCatcher(int sig) {
	// Catches the signal of |Ctrl + C| and ignores it
	(void)signal(SIGINT, SIG_DFL);
}
// ================================================================================================
/*
void sigtstpCatcher
Ignores the SIGTSTP signal
Parameters: int sig
Returns: ---
*/
void sigtstpCatcher(int sig) {
	// Catches the signal of |Ctrl + V| and ignores it 
	(void)signal(SIGINT, SIG_DFL);
}
// ================================================================================================
/*
void exitCommand
Prints that you are exiting the program
Parameters: ---
Returns: ---
*/
void exitCommand() {
	printf("\nExit program\n");
	fflush(stdout);
}
// ================================================================================================
/*
void statusCommand
Returns the exit value of the previous signal through the parameter stat
Parameters: int stat
Returns: ---
*/
void statusCommand(int stat) {
	// Checks to see if the last stat was succesful or not and prints out the value
	if (WIFEXITED(stat))
		stat = WEXITSTATUS(stat);
	else
		stat = WTERMSIG(stat);
	printf("Exit value |%d|\n", stat);
}
// ================================================================================================
/*
void cdCommand
Changes the current working dirrectory if the currCommand fufills the if statements
Parameters: struct command* currCommand
Returns: ---
*/
void cdCommand(struct command* currCommand) {
	// Initialize variables
	char* buffer2 = calloc(MAX_CHAR + 1, sizeof(char));
	char* currDIR = calloc(MAX_CHAR + 1, sizeof(char));

	// Changes to home dir 
	strcpy(currDIR, getcwd(buffer2, MAX_CHAR));
	if (strcmp(currCommand->command, "echo") != 0)
		if (strcmp(currCommand->command, "pwd\n") != 0)
			if (strcmp(currCommand->command, "cd\n") == 0)
				chdir(getenv("HOME"));

	// Changes to currDirr ++ currCommand->parameters (Example: "./" ++ "testdir123456" )
	if (strcmp(currCommand->command, "cd") == 0) {
		strcat(currDIR, "/");
		strcat(currDIR, currCommand->parameters);
		chdir(currDIR);
	}
}
// ================================================================================================
int main() {
	// Initialize Values
	struct command* currCommand = malloc(sizeof(struct command));
	char* begin = "Ive spent too long on this assignment";

	// Setting the structs command line to something so it can ceck its not "exit\n"
	currCommand->commandLine = calloc(strlen(begin) + 1, sizeof(char));
	strcpy(currCommand->commandLine, begin);

	// Finding the current working dir and saving it into smallshDIR
	char* buffer = calloc(MAX_CHAR + 1, sizeof(char));
	char* smallshDIR = calloc(MAX_CHAR + 1, sizeof(char));
	strcpy(smallshDIR, getcwd(buffer, MAX_CHAR));

	// Keeps track of the background process id 
	pid_t backgroundIdArray[512];

	// Initializes forking variables and other vriables
	pid_t smallshPID = getppid();
	pid_t spawnpid = -5;
	int childPid = 0;
	int childStatus = 0;
	int backPid = 0;
	int backStatus = 0;
	int stat = 0;
	int killCount = 0; // If its 1 then we ignore "&" if its 0 we take "&" into account

	while (strcmp(currCommand->commandLine, "exit") - NEW_LINE_CHAR_VALUE != 0) {
		// Fill out the command struct with the correct values of the current line
		currCommand = fillCommand(smallshPID);

		// These catch the |Ctrl + C (SIGINT)| and |Ctrl + V (SIGTSTP)| signals
		(void)signal(SIGINT, sigintCatcher);
		(void)signal(SIGTSTP, sigtstpCatcher);

		// Activates CD if needed
		cdCommand(currCommand);

		// Activates the status command if the command is "status"
		if (strcmp(currCommand->command, "status") == 0) {
			statusCommand(childStatus);
		}

		// Fork - PARENT |Reads the current command| and CHILD |executes the command line|
		spawnpid = fork();
		switch (spawnpid) {
		case -1:
			perror("fork() failed!");
			exit(1);
			break;
		case 0:
			processCommandLine(currCommand, smallshPID, killCount, backgroundIdArray);
			exit(0);
		default:
			// Waits for a child to be complete if its in the foreground
			childPid = wait(&childStatus);

			// Checks to see if a background process is terminated --- Does not work
			//backPid = waitpid(backgroundIdArray[BG_ARRAY_PLACEMENT], &backStatus, WNOHANG);
			//if (backPid != 0) {
			//	printf("Background Process |%d| ended with ", backPid);
			//	fflush(stdout);
			//	statusCommand(backPid);
			//}

			// Changes the CATCH_VAR depending if its the first or second time, to make it 
			// seem like the foreground-only mode is opening and closing
			if (strcmp(currCommand->command, "kill") == 0 && killCount % 2 != 0) {
				CATCH_VAR = 1;
				killCount++;
			}
			if (strcmp(currCommand->command, "kill") == 0 && killCount % 2 == 0) {
				CATCH_VAR = 0;
				killCount++;
			}
			break;
		}
	}

	// After all processes have been completed and the currCommand->command is now "exit"
	exitCommand();

	return EXIT_SUCCESS;
}
