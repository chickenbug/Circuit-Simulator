#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int low_chars[26];
int upp_chars[26];

const char *circ_types[5] = {
	"NOT",
	"AND",
	"OR",
	"DECODER",
	"MULTIPLEXER"
	};

// sets the varialbes at a given char
void set_variable(char a, int i){
	if(islower(a)) low_chars[a - 97] = i;
	else if (isupper(a)) upp_chars[a - 65] = i;
	else{
		printf("Not a valid character. Variable must be an uppercase or lowercase char\n");
	}
	return;
}

// returns the variable at a given char
int get_variable(char a){
	if(islower(a)) return low_chars[a - 97];
	else if(isupper(a)) return upp_chars[a - 65];
	else if(a == '1') return 1;
	else if(a == '0') return 0;
	else{
		printf("Not a recognized variable\n");
		return -1;
	}
}

// initializes all variables to -1 (NULL)
void initialize_var_array(){
	int i;
	for(i = 0; i < 26; i++){
		low_chars[i] = -1;
		upp_chars[i] = -1;
	}
	return;
}

unsigned int binaryToGray(unsigned int num){
    return num ^ (num >> 1);
}

unsigned int grayToBinary(unsigned int num){
    unsigned int mask;
    for (mask = num >> 1; mask != 0; mask = mask >> 1){
        num = num ^ mask;
    }
    return num;
}

// takes in the input variable string and a set of values, and adds them to the data struct
void read_input(char* input_string, char* input_values){
	int  i;
	char *token, input_variables[50];

	const char s[2] = " ";
	strtok(input_string, s);
    int count = atoi(strtok(NULL, s));
	for(i = 0; i < count; i++){
       	token = strtok(NULL, s);
		printf( " %s\n", token );
		input_variables[i] = *token;
	}

	for (i = 0; i < count; i++){
		token = strtok(input_values, s);
		set_variable(input_variables[i], atoi(token));
	}
	return;
}

//returns a null terminated string of the needed output variables
char* read_output(char* output_string){
	int  i;
	char *token;
	char *output_variables = malloc(50);

	const char s[2] = " ";
    strtok(output_string, s);
    int count = atoi(strtok(NULL, s));
	for(i = 0; i < count; i++){
       	token = strtok(NULL, s);
		printf( " %s\n", token );
		output_variables[i] = *token;
	}
	return output_variables;
}

// determines the circuit_type interger given the circuit's name
int ciruit_type(char* name){
	int i;
	for (i = 0; i < 5; i++){
		if(strcmp(name,circ_types[i]) == 0) return i;
	}
	return -1;
}

// Attempts to run the not circuit. Returns 0 if an input variable has not been given yet, and 1 on sucess
int not(char* circuit_string){
	char in, out;
	int in_val;
	sscanf(circuit_string, "%*s %c %c", &in, &out);
	in_val = get_variable(in);
	if(in_val == -1) return 0;
    else{
    	in_val ? set_variable(out, 0) : set_variable(out, 1);
    	return 1;
    }
}

//TODO allow this to deal with interger inputs.
int and(char* circuit_string){
	char in1, in2, out;
	int  in1_val, in2_val;
	sscanf(circuit_string, "%*s %c %c %c", &in1, &in2, &out);

	in1_val = get_variable(in1);
    in2_val = get_variable(in2);
    if(in1_val == -1  || in2_val == -1) return 0;

    if(in1_val == in2_val) set_variable(out, in1_val);
    else set_variable(out, 0);
    return 1;
}

int or(char* circuit_string){
	char in1, in2, out;
	int  in1_val, in2_val;
	sscanf(circuit_string, "%*s %c %c %c", &in1, &in2, &out);

	in1_val = get_variable(in1);
    in2_val = get_variable(in2);

    if( in1_val == -1  || in2_val == -1) return 0;
    if( in1_val == 1 || in2_val == 1) set_variable(out, 1);
    else set_variable(out, 0);
    return 1;
}

int decoder(char* circuit_string){
	char out[50], ctemp;
	int inval[50], count, i;
	int gray = 0;
	const char s[2] = " ";

    strtok(circuit_string, s);
    count = atoi(strtok(NULL, s));
    for(i = 0; i <count; i++){
    	inval[i] = get_variable(*strtok(NULL,s));
    	if(inval[i] == -1) return 0;
    }
    for(i = 0; i < 1<<(count); i++){
    	ctemp = *strtok(NULL,s);
    	out[i] = ctemp;
    	set_variable(ctemp, 0);
    }
    for(i = 0; i <count; i++){
    	if(inval[i]) gray += 1<<(count - 1 - i);
    }
    set_variable(out[grayToBinary(gray)], 1);
    return 1;
}

int multiplexer(char* circuit_string){
	char out;
	int inval[50], inswitch[50], count, i;
	int gray = 0;
	int switch_num = 0;
	const char s[2] = " ";

    strtok(circuit_string, s);
    count = atoi(strtok(NULL, s));
    for(i = 0; i < count; i++){
    	inval[i] = get_variable(*strtok(NULL,s));
    	if(inval[i] == -1) return 0;
    }
    while(count != 1){
    	switch_num++;
    	count>>=1;
    }
    for(i = 0; i < switch_num; i++){
    	inswitch[i] = get_variable(*strtok(NULL, s));
    	if(inswitch[i] == -1) return 0;
    }
    for(i = 0; i < switch_num; i++){
    	if(inswitch[i]) gray += 1<<(switch_num - 1 - i);
    }
    out = *strtok(NULL, s);
    set_variable(out, inval[grayToBinary(gray)]);
    return 1;
}

int use_circuit_piece(char* circuit_string){
	const char s[2] = " ";
 	char* token = strtok(circuit_string, s);
	int type = ciruit_type(token);
	int succ;
	switch(type) {
		case 0:
			succ = not(circuit_string);
			break;
		case 1:
			succ = and(circuit_string);
			break;
		case 2:
			succ = or(circuit_string);
			break;
		case 3:
			succ = decoder(circuit_string);
			break;
		case 4: 
			succ = multiplexer(circuit_string);
			break;
		default:
			printf("Unrecognized circuit type. Exiting...\n");
			exit(0);
	}
	if(!succ) return 0;
	else return 1;
}

//Takes in the file containing a circuit and returns a null terminated list of the ciruit pieces
void run_circuit(FILE* circuit_file, FILE* input_file){
	char circuit_buffer[2000], input_buffer[2000];
	char *output_variables, *circuit_set[100];
	int count = 0;

	while(fgets(input_buffer, 2000, input_file) != NULL){
		initialize_var_array();

		fgets(circuit_buffer, 2000, circuit_file);
		read_input(circuit_buffer, input_buffer);
		fgets(circuit_buffer, 2000, circuit_file);
		output_variables = read_output(circuit_buffer);

		while(fgets(circuit_buffer, 2000, circuit_file) != NULL ) {
			//TODO consider a linked list?
			if (!use_circuit_piece(circuit_buffer)){	
				char* new = malloc(strlen(circuit_buffer) + 1);
				strcpy(new, circuit_buffer);
				circuit_set[count] = new;
				count++;
			}
	  	}
	  	//loop through the remainder.
	}
	return;
}

FILE* safe_fopen(char* file_name){
	FILE* new = fopen(file_name, "r");
	if(!new){
		printf("%s could not be opened\n", file_name);
		exit(0);
	}
	return new;
}

int mains(int argc, char** argv){
	if( !argv[1] || !argv[2]){
		printf("One of the inputs was NULL. Exiting now\n");
	}
	FILE* circuit_file = safe_fopen(argv[1]);
	FILE* input_file = safe_fopen(argv[2]);
	run_circuit(circuit_file, input_file);
	fclose(circuit_file);
	fclose(input_file);
	return 0;
}

int main(){
	initialize_var_array();
	return 0;
}
