/*********************************************************************
** Author: Collin James
** Date: 11/2/16
** Description: Logic for smallsh program; See header for more 
** information about functions
*********************************************************************/

/* includes */
#include "smallsh.h"

/* Global constants */
const int MAX_CMD_SIZE = 2048;
const int MAX_NUM_ARGS = 512;
const char * DELIM = " ";

/* Global vars */
char * USAGE = "Usage: command [arg1 arg2 ...] [< input_file] [> output_file] [&]";

int main(int argc, char const *argv[])
{
	/* set up Pidkeeper */
	struct Pidkeeper PK = new_PK(-1, -1);

	/* set up array */
	struct cirListDeque Pid_queue;
	initCirListDeque(&Pid_queue);

	/* set to ignore certain signals */
	struct sigaction ignore_action;
	ignore_action.sa_handler = SIG_IGN;
	sigfillset(&(ignore_action.sa_mask)); // fill w/ all signals
	sigaction(SIGINT, &ignore_action, NULL);
	sigaction(SIGTERM, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);
	sigaction(SIGABRT, &ignore_action, NULL);

	/* pid to keep track of child and parent */
	pid_t pid = -1;

	/* variables */
	struct Statuskeeper * theSK = new_SK(0, -1);
	enum Status theStatus = CONTINUE;
	struct Commandkeeper theCK;	
	char * input; // allocated in getInput, hold user input
	char exitSt[] = "exit";
	// int r; 

	/* main logic; loop while not exit command and not child pid */
	while(theStatus != EXIT && pid != 0){
		/* keep and print all processes*/
		do{
			queueChecker(&Pid_queue, PK, 0);
			
			/* gets input and puts in input variable */
			getInput(&input);
		} while(validateInput(&input) != 0);

		/* get rid of a \n if it exists at end of string */
		removeLineEnding(&input);

		/* process string */
		theCK = parseInString(&input);

		/* check for redirect errors and print usage */
		if(theCK.red_error){
			/* print usage and go to next loop iteration */
			usage();
			continue;
		}

		/* otherwise check for built in commands and run */
		if(theCK.bltin){
			// theSK->type = 1;
			if(strcmp(theCK.cmd, "cd") == 0){
				/* run cd */
				theSK->type = 1;
				theSK->sk_sig = mycd(&theCK);
			}
			else if(strcmp(theCK.cmd, "status") == 0){
				/* run status*/
				theSK->sk_sig = mystatus(&theCK, theSK);
				theSK->type = 1;
			} 
			else if(strcmp(theCK.cmd, exitSt) == 0){
				/* test for exit */
				theSK->sk_sig = myexit(&theCK);
				theSK->type = 1;
				if(theSK->sk_sig == 0)
					theStatus = EXIT; // gets us out of the loop
			}
		}
		/* otherwise handle external commands */
		else{
			/* if no bg symbol, run in foreground */
			if(!theCK.bg)
				pid = runInFore(&theCK, theSK);
			/* otherwise, run in background */
			else{
				PK = runInBack(&theCK, theSK);
				pid = PK.pid;
				/* add your process to the queue and print out */
				addBackCirListDeque(&Pid_queue, PK);
				printStatusMsg(PK.pid, "background pid is ");
			}
		}
	}

	/* cleanup at exit */
	if(pid != 0){ // don't do this from a child!
		/* kill any remaining processes with killp flag */
		queueChecker(&Pid_queue, PK, 1);
		/* free malloc'd vars */
		free_sk(theSK);
		free(input);
		removeAllCirListDeque(&Pid_queue);
	}

	return 0;
}

void printWaitPidStatus(pid_t bgpid, int bgstatus, struct Pidkeeper PK){
	int bgexitstatus;
	if(bgpid != 0 && bgpid != -1){
		printStatusMsgNL(bgpid, "background pid ", 0);
		// if (WIFEXITED(PK.status != 1 ? bgstatus : PK.status))
		if (WIFEXITED(bgstatus))
		{
			// printf("The process exited normally\n"); 
			bgexitstatus = WEXITSTATUS(bgstatus); 
			// printf("exit status was %d\n", exitstatus);
			printStatusMsg( (PK.status != 1 ? bgexitstatus : PK.status), " is done: exit value ");
		} 
		else{ // terminated by a signal
			printStatusMsg(bgstatus, " is done: terminated by signal ");
		}
	}
}

void queueChecker(struct cirListDeque * Pid_queue, struct Pidkeeper PK, int killp){
	int pc, bgstatus;
	pid_t bgpid = 0;

	int size = getSizeCirListDeque(Pid_queue);
	for (pc = 0; pc < size; pc++)
	{
		/* get the top process */
		PK = frontCirListDeque(Pid_queue);
		// printf("Pid_queue %d=%d\n", pc, PK.pid);
		removeFrontCirListDeque(Pid_queue);
		bgpid = waitpid(PK.pid, &bgstatus, WNOHANG);
		/* if pgpid is collected, print it out */
		// printf("bgpid=%d\n", bgpid);
		if(bgpid != 0 && bgpid != -1){
			printWaitPidStatus(bgpid, bgstatus, PK);
		}
		/* if not collected, put it back in the queue */
		else {
			if(killp){
				// printOut("in killp", 1);
				kill(PK.pid, SIGKILL);
				bgpid = waitpid(PK.pid, &bgstatus, WNOHANG);
				if(bgpid != 0 && bgpid != -1){
					printWaitPidStatus(bgpid, bgstatus, PK);
				}
			}
			else
				addBackCirListDeque(Pid_queue, PK);
		}
	}
}

void usage(){
	printOut(USAGE, 1);
}

struct Pidkeeper runInBack(struct Commandkeeper * theCK, struct Statuskeeper * theSK){

	/* catch signals and perform default action */
	struct sigaction def_action;
	def_action.sa_handler = SIG_DFL;
	sigfillset(&(def_action.sa_mask)); // fill w/ all signals
	// sigaction(SIGINT, &def_action, NULL);
	sigaction(SIGTERM, &def_action, NULL);
	sigaction(SIGQUIT, &def_action, NULL);
	sigaction(SIGABRT, &def_action, NULL);

	/* set up pipe for communicating failures with parent */
	int r, 
		pipeFDs[2],
	 	send = 1, 	// data to be sent
		stat_msg,	// receive data here
		pipe_status; // save status of pipe
	long int msg_size = sizeof(send);

	if( (pipe_status = pipe(pipeFDs)) == -1)
		perror("failed to set up pipe");

	char * cmd = theCK->cmd;
	int f_error = 0;
	
	/* fork a process */
	int pid = fork();
	char ** arguments = malloc( sizeof(char *) * theCK->num_args+2);
	arguments[0] = cmd;

	/* make an array of arguments */
	int i;
	for (i = 0; i < theCK->num_args; i++)
	{
		arguments[i+1] = theCK->args[i].arg;
	}
	arguments[theCK->num_args+1] = NULL;

	/* in child, set up redirection & exec */
	if(pid == 0){
		if(pipe_status != -1){
			close(pipeFDs[0]); // close input pipe
			fcntl(pipeFDs[1], F_SETFD, FD_CLOEXEC); // close output pipe on exec
		}

		/* set up redirect stderr for bg process */
		f_error = redirectOut(theCK, 2);
		/* if redin, do dup2 w/ stdin */
		if(theCK->red_in){
			f_error = redirectIn(theCK, 0);
		}
		/* otherwise, redirect to /dev/null */
		else
			f_error = redirectIn(theCK, 1);
		/* if redout, do dup2 w/ stdout */
		if(theCK->red_out){
			f_error = redirectOut(theCK, 0);
		} 
		/* redirect out to /dev/null */
		else
			f_error = redirectOut(theCK, 1);

		if(!f_error){
			/* execute the command */
			if(theCK->red_in)
				execlp(cmd, cmd, NULL); // w/ file
			else
				execvp(cmd, arguments); // w/ arguments
			
			/* print out any error if exec fails */
			// printf("there was an error\n");
			perror(cmd);
			/* send error status message to parent, i.e. 1 (sending int disguised as void *) 
				you will never get to this point if exec occurs, and output pipe will be closed
				on exec, causing read to receive 0 */
			if(pipe_status != -1)	
				write(pipeFDs[1], &send, msg_size);
		}
	}

	/* in parent, wait for pid */
	else if(pid > 0){
		// printf("%s\n", "I'm the parent");
		if(pipe_status != -1)
			close(pipeFDs[1]); // close output pipe
		int status;
		
		pid_t exitpid = waitpid(pid, &status, WNOHANG);
		/* figure out exit status and fill Statuskeeper */
		if (WIFEXITED(status))
		{
			// printf("The process exited normally\n"); 
			int exitstatus = WEXITSTATUS(status); 
			/* try to read from pipe; if you get a result, it's going to be status 1 from child 
				we want this to be the status that we pass on */
			if(pipe_status != -1){
				r = read(pipeFDs[0], &stat_msg, msg_size);
				if (r > 0)
					exitstatus = stat_msg;
			}
			theSK->type = 1;
			theSK->sk_sig = exitstatus;
		} 
		else if(WEXITSTATUS(status) != 127 && WEXITSTATUS(status) != 52) { // terminated by a signal other than 127
		// else { // terminated by a signal other than 127
			theSK->type = 2;
			// int exitstatus = WEXITSTATUS(status); 
			// printStatusMsg(exitstatus, "actual signal? ");
			printStatusMsg(status, "terminated by signal ");
			theSK->sk_sig = status;
		}			
		// printf("status was %d, exitpid was %d\n", status, exitpid);
	}
	
	return new_PK(pid, theSK->sk_sig);
}

int runInFore(struct Commandkeeper * theCK, struct Statuskeeper * theSK){
	/* set up child to do default when receiving SIGINT */
	struct sigaction child_action;
	child_action.sa_handler = SIG_DFL;

	/* set up pipe for communicating failures with parent */
	int r, 
		pipeFDs[2],
	 	send = 1, 	// data to be sent
		stat_msg,	// receive data here
		pipe_status; // save status of pipe
	long int msg_size = sizeof(send);

	if( (pipe_status = pipe(pipeFDs)) == -1)
		perror("failed to set up pipe");

	char * cmd = theCK->cmd;
	int f_error = 0;

	/* fork a process */
	int pid = fork();
	char ** arguments = malloc( sizeof(char *) * theCK->num_args+2);
	arguments[0] = cmd;

	/* make an array of arguments */
	int i;
	for (i = 0; i < theCK->num_args; i++)
	{
		arguments[i+1] = theCK->args[i].arg;
	}
	arguments[theCK->num_args+1] = NULL;

	/* in child, set up signal catching & exec */
	if(pid == 0){
		if(pipe_status != -1){
			close(pipeFDs[0]); // close input pipe
			fcntl(pipeFDs[1], F_SETFD, FD_CLOEXEC); // close output pipe on exec
		}
		/* catch */
		sigaction(SIGINT, &child_action, NULL);

		/* set up redirects */
		/* if redin, do dup2 w/ stdin */
		if(theCK->red_in){
			f_error = redirectIn(theCK, 0);
		}
		/* if redout, do dup2 w/ stdut */
		if(theCK->red_out){
			f_error = redirectOut(theCK, 0);
		}
		// printf("f_error=%d\n", f_error);
		if(!f_error){
			/* execute the command */
			if(theCK->red_in)
				execlp(cmd, cmd, NULL); // w/ file
			else
				execvp(cmd, arguments); // w/ arguments
			
			/* print out any error if exec fails */
			perror(cmd);	
		} else {
			printOut("cannot open ", 0);
			printOut(theCK->infile, 0);
			printOut(" for output", 1);
		}
		/* send error status message to parent, i.e. 1 (sending int disguised as void *) 
			you will never get to this point if exec occurs, and output pipe will be closed
			on exec, causing read to receive 0 */
		if(pipe_status != -1)	
			write(pipeFDs[1], &send, msg_size);
	}

	/* in parent, wait for pid */
	else if(pid > 0){
		// printf("%s\n", "I'm the parent");
		if(pipe_status != -1)
			close(pipeFDs[1]); // close output pipe
		int status;
		
		pid_t exitpid = waitpid(pid, &status, 0);
		/* figure out exit status and fill Statuskeeper */
		if (WIFEXITED(status))
		{
			/* get normal exit status */
			int exitstatus = WEXITSTATUS(status);
			/* try to read from pipe; if you get a result, it's going to be status 1 from child 
				we want this to be the status that we pass on */
			if(pipe_status != -1){
				r = read(pipeFDs[0], &stat_msg, msg_size);
				if (r > 0)
					exitstatus = stat_msg;
			}
			
			theSK->type = 1;
			theSK->sk_sig = exitstatus;
		} 
		else{ // terminated by a signal
			theSK->type = 2;
			printStatusMsg(status, "terminated by signal ");
			theSK->sk_sig = status;
		}			
	}
	
	return pid;
}

int redirectOut(struct Commandkeeper * theCK, int bg){
	int fd, fd2, fd3, fde, f_error = 0;
	/* if foreground process, open the outfile, otherwise redirect to /dev/null */
	if(bg == 0)
		fd = open(theCK->outfile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	else
		fd = open("/dev/null", O_WRONLY, 0644);

	/* redirect bg stderr to null */
	if(bg == 1){
		fde = open("/dev/null", O_WRONLY, 0644);
		fd3 = dup2(fde, 2); // redirect stderr to null
	}

	/* test for file */
	if (fd == -1)
	{
	   	perror(theCK->cmd);
		f_error = 1;
	}
	/* file is open, so redirect it */
	else {
		fd2 = dup2(fd, 1); 
		if (fd2 == -1)
		{
		   	perror(theCK->cmd);
		   	f_error = 1;
		}
	}
	/* 1 or 0 */
	return f_error;
}

int redirectIn(struct Commandkeeper * theCK, int bg){
	/* similar to above function */
	int fd, fd2, f_error = 0;
	if(bg == 0)
		fd = open(theCK->infile, O_RDONLY); 
	else
		fd = open("/dev/null", O_RDONLY);

	if (fd == -1)
	{
		f_error = 1;
	}
	else {
		fd2 = dup2(fd, 0); 
		if (fd2 == -1)
		{
		   	perror(theCK->cmd);
		   	f_error = 1;
		}
	}

	return f_error;
}

struct Commandkeeper parseInString(char ** inputStr){
	/* create variables for holding various CK items and the string */
	char * tmp = *inputStr;
	us_int bg = 0,
		bltin = 0,
		no_cmd = 0;
	char * cmd;
	char * infile = NULL;
	char * outfile = NULL;
	struct argArray args[MAX_NUM_ARGS];
	struct Commandkeeper CK;
	int str_len = strlen(tmp);
	int red_in_count = 0;
	int red_out_count = 0;
	int red_in_sat = 0;
	int red_out_sat = 0;
	int red_error = 0;

	/* check last character for & and set bg */
	if(tmp[str_len-1] == '&'){
		bg = 1;
		tmp[str_len-1] = '\0';
		str_len = strlen(tmp);
		/* remove space at end of string if necessary */
		if(tmp[str_len] == ' '){
			tmp[str_len] = '\0';
			str_len = strlen(tmp);
		}
	}

	/* extract your command from the front of the string */
	char * subString;
	subString = strtok(tmp,DELIM);
	cmd = subString;

	/* get built in status */
	if(strcmp(cmd, "cd") == 0 || strcmp(cmd, "status") == 0 || strcmp(cmd, "exit") == 0){
		bltin = 1;
	}

	/* parse remaining tokens and determine if you have a > or <, argument, or filename */
	int i = 0;
	/* get next token */
	subString = strtok(NULL, DELIM);
	/* loop until you have all the arguments, or until you hit the max of 512 */
	while (subString != NULL && i < MAX_NUM_ARGS)
	{
		/* is it < or >? If so, start your counter, which should always be 1 */
		if(strcmp(subString, "<") == 0){
			// printOut("input redirect", 1);
			red_in_count += 1; // set to 
		} 
		else if(strcmp(subString, ">") == 0){
			// printf("output redirect\n");
			red_out_count += 1;
		} 
		else if(strcmp(subString, "&") == 0){
			// printf("output redirect\n");
			red_error += 1;
		}
		/* not a redirection character */
		else {
			if(red_in_count == 1 || red_out_count == 1){ // you've had a redirection operator, so no more args
				if(red_in_count){ 
					if(red_in_sat == 0){ // you've got a filename
						infile = subString;
					}
					red_in_sat += 1;
				}
				if(red_out_count){ // you've had a redirection operator, so no more args
					if(red_out_sat == 0){ // you've got a filename
						outfile = subString;
					}
					red_out_sat += 1;
				}
			}
			else {	// you've got an argument; save it for CK
				args[i].arg = subString;
				i++;
			}
		}
		/* get next token */
		subString = strtok(NULL, DELIM);
	}
	/* now all args, redirects figured; create initial Commandkeeper object */
	CK = new_CK(cmd, args, i);

	/* check for redirect errors */
	/* no filenames */
	if( (red_in_count > 0 && red_in_sat == 0) || (red_out_count > 0 && red_out_sat == 0))
		red_error = 1;
	/* too many operators or files */
	else if ( (red_in_count > 1 || red_out_count > 1) || (red_in_sat > 1 || red_out_sat > 1) )
		red_error = 1;
	
	/* check whether to mark redirects as true */
	if(red_in_count == 1 && red_error == 0) {		
		CK.red_in = 1;
		CK.infile = infile;
	}
	if(red_out_count == 1 && red_error == 0){
		CK.red_out = 1;
		CK.outfile = outfile;
	}
	/* set the rest of the variables in the CK object */
	CK.red_error = red_error;
	CK.bg = bg;
	CK.bltin = bltin;
	CK.no_cmd = no_cmd;

	return CK;
}

void removeLineEnding(char ** input){
	/* create a string of 2049 and copy into tmp string */
	char tmp[MAX_CMD_SIZE+1];
	strcpy(tmp, *input);
	/* get size of string, find last character, and delete if \n */
	int le = strlen(tmp)-1;
	if(tmp[le] == '\n'){
		tmp[le] = '\0';
		/* copy the string back to input (returning it) */
		strcpy(*input, tmp);
	}
}

int validateInput(char **input) {
	/* if string contains no info, return 1 */
	if(strlen(*input) <= 1){ // i.e. null or just \0
		/* check for 0 length string or ^C so no newline */
		if(strlen(*input) < 1 || (strlen(*input) == 1 && *input[0] != '\n'))
			printOut("", 1); // print a new line
		return 1;
	}
	/* if string begins with #, print string and return 1 */
	if(**input == '#'){
		// printOut(*input, 0);
		return 1;
	}

	return 0;
}

void getInput(char **retString){
	/* allocate your string w/ room for \0 */
	*retString = malloc( sizeof(char) * (MAX_CMD_SIZE + 1) );
	
	/* print and accept input while the string is not a possible command */
	printOut(": ", 0);
	/* get a string of max size 2048 to leave room for \0 */
	fgets(*retString, MAX_CMD_SIZE, stdin);
}

void caughtSig(){
	printf("%s\n", "caught signal");
}

void childSig(){
	printf("%s\n", "signal: child exited");
}

void printStatusMsgNL(int sk_sig, char * msg, int nl){
	int buf_s = 30;
	// char * fullmessage;
	char sig[buf_s];
	int n;

	n = snprintf(sig, buf_s, "%d", sk_sig);
	printOut(msg, 0);
	if(nl)
		printOut(sig, 1);
	else
		printOut(sig, 0);
}

void printAllCK(struct Commandkeeper ck) {
	printf("bg=%d, bltin=%d, red_in=%d, red_out=%d, red_error=%d, num_args=%d\n", ck.bg, ck.bltin, ck.red_in, ck.red_out, ck.red_error, ck.num_args);
	printOut(ck.cmd, 1);
}