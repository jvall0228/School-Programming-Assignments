/* HW4 */
/* NAME: Jaeson Valles */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef int bool;
#define true 1
#define false 0

void add(char * dest, char * op1, char * op2){
	printf("add %s,%s,%s\n",dest, op1, op2);
}
void sub(char * dest, char * op1, char * op2){
	printf("sub %s,%s,%s\n",dest, op1, op2);
}
void mult(char * dest, char * op1, char * op2){
	printf("mult %s,%s\n", op1, op2);
	printf("mflo %s\n",dest);
}
void mod(char * dest, char *op1, char * op2){
	printf("div %s,%s\n",op1,op2);
	printf("mfhi %s\n",dest);
}
void divide(char * dest, char * op1, char * op2){
	printf("div %s,%s\n",op1,op2);
	printf("mflo %s\n",dest);
}
int power(int base, int exp){
	int result = base;
	if(exp == 0)
		return 1;
	else{
		while(exp != 1){
			result = result * base;
			exp--;
		}
		return result;
	}
}
void multi(char * dest, char * op1, int op2, int t){
	if(op2 == 0){
		printf("li %s,0\n",dest);
		return;
	}
	else if(op2 == 1){
		printf("move $t%d,%s\n",t,op1);
		printf("move %s,$t%d\n",dest,t);
		return;
	}
	else if(op2 == -1){
		printf("move $t%d,%s\n",t,op1);
		printf("sub %s,$zero,$t%d\n",dest,t);
		return;
	}
	int exp;
	int temp;
	bool isNeg;
	isNeg = false;
	exp = 0;

	if(op2 < 0){
		isNeg = true;
		temp = op2 * (-1);
	}
	else
		temp = op2;
	while(temp > power(2,exp)){
		exp++;
	}
	exp = (temp == power(2,exp)) ? exp : exp - 1;
	temp = temp - power(2,exp);
	if(exp != 0 ){
		printf("sll $t%d,%s,%d\n",t,op1,exp);
		printf("move $t%d,$t%d\n",(t == 9) ? 0 : t+1, t);
	}
	exp = 0;
	while(temp != 0){
		while(temp > power(2,exp)){
			exp++;
		}
		exp = (temp == power(2,exp)) ? exp : exp - 1;
		temp = temp - power(2,exp);
		if(exp != 0){
			printf("sll $t%d,%s,%d\n",t,op1,exp);
			printf("add $t%d,$t%d,$t%d\n",(t == 9) ? 0 : t+1,(t == 9) ? 0 : t+1, t);
		}else{
			printf("add $t%d,$t%d,%s\n",(t == 9) ? 0 : t+1, (t == 9) ? 0 : t+1, op1);
		}
		exp = 0;
	}
	if(isNeg)
		printf("sub %s,$zero,$t%d\n",dest,(t == 9) ? 0 : t+1);
	else
		printf("move %s,$t%d\n", dest, (t == 9) ? 0 : t+1);
}
void divi(char * dest, char * op1, int op2, int t,int L){
	if(op2 == 1){
		printf("move %s,%s\n",dest,op1);
		return;
	}

	if(op2 == -1){
		printf("sub %s,$zero,%s\n",dest,op1);
		return;
	}

	if(op2 % 2 != 0){
		printf("li $t%d,%d\n",t,op2);
		printf("div %s,$t%d\n",op1,t);
		printf("mflo %s\n",dest);
		return;
	}
	int temp = 0;
	int exp = 0;
	if(op2 < 0){
		temp = temp - op2;
	}
	else{
		temp = op2;
	}

	printf("bltz %s,L%d\n",op1,L);
	while(temp != power(2,exp)){
	       exp++;
      	}
	printf("srl %s,%s,%d\n",dest,op1,exp);
	if(op2 < 0){
		printf("sub %s,$zero,%s",dest,dest);
	}
	printf("j L%d\n",L+1);
	printf("L%d:\n",L);
	printf("li $t%d,%d\n",t,op2);
	printf("div %s,$t%d\n",op1,t);
	printf("mflo %s\n",dest);
	printf("L%d:\n",L + 1);
}
int getVarsIndex(char* vars, char var){
	int i;
	for(i = 0; i < 8; i++){
		if(*(vars + i) == var)
			return i;
	}
	return -1;
}
int getVarsNew(char* vars){
	int i;
	for(i = 0; i <8; i++){
		if(*(vars + i + 1) == '\0'){
			return i;
		}
		if(i + 1 == 7){
			return i + 1;
		}
	}
	return -1;
}
int main(int argc, char *argv[]){
	if(argc == 0){
		fprintf(stderr,"ERROR: No arguments were given");
		return EXIT_FAILURE;
	}
	FILE * file;
	char * line;
	size_t length = 0;
	file = fopen(argv[1], "r");
	if(file == NULL){
		fprintf(stderr,"ERROR: Invalid File Name Given");
		return EXIT_FAILURE;
	}

	int t = 0;
	int L = 0;
	char * vars = malloc(8 * sizeof(char));
	//Array where every element represents a unique variable
	while(getline(&line, &length, file) != -1){
		printf("# %s", line);
		//Assigns a register to each unique variable where the index is the register number
		int i;
		for(i = 0; i < 8; i++){
			if(*(vars + i) == '\0'){
				*(vars + i) = *line;
				break;
			}
		}

		//Isolates the expression to the right of the equals sign
		line = line + 4;

		//Finds the first operator on the right side and calls the appropriate function.
		int size = strlen(line);
		bool isOperator = false;
		int j = 0;
		int k = 0;
		char * d = (char*)malloc(32*sizeof(char));
		char * o1 = (char*)malloc(32*sizeof(char));
		char * o2 = (char*)malloc(32*sizeof(char));
		for(i = 0; i < size; i++){
			printf("\n line:%s\n",line);
			//printf("\n line + i:%s\n",line+i);
			printf("\n point: %c\n",*(line+i));
			switch(*(line + i)){
				case '+':
					isOperator = true;
					//Checks if the left operand is a temporary register or a variable. Since there can be no constants to the left of an operator I assume this is a temporary register.
					if(isdigit(*(line + i - 2))){
						sprintf(o1, "$t%c", *(line + i - 2));
					}
					//Assumes that the left oeprand is a variable we have encountered before.
					else{
						sprintf(o1, "$s%d", getVarsIndex(vars,*(line + i -2)));
					}
					//Checks if the right operand is a constant or a variable
					j = i + 2;
					if(isdigit(*(line + j)) || *(line + j) == '-'){
						//Accounts for constants with multiple digits.
						k = 0;
						while(isdigit(*(line + j)) || *(line + j ) == '-'){
							*(o2 + k) = *(line + j);
							j++;
							k++;
						}
						k--;
					}
					// Assumes that the right operand is a variable.
					else {
						sprintf(o2, "$s%d", getVarsIndex(vars, *(line + i + 2)));
					}
					//Checks to see if the character after the right operand is a space to see if there is another operation after this one.
					if(*(line + i + 2 + k + 1) == ' '){
						//printf("another operation\n");
						sprintf(d, "$t%d", t);
						//Inserts the number of the register holding the result of this operation into the string to be accessed by the next string
						*(line + i + 2 + k) = '0' + t;
						//Cycles to next temporary register.
						t = (t == 9) ? 0 : t + 1;
						i++;
					}
					//If there are no more operations then assume that the result of this operation is what is being stored in the newest register.
					else{
						sprintf(d, "$s%d", getVarsNew(vars));
						break;
					}
					add(d, o1, o2);
					break;
				case '-':
					if(isdigit(*(line + i + 1))){
						break;
					}
					isOperator = true;
					//Checks if the left operand is a temporary register or a variable. Since there can be no constants to the left of an operator I assume this is a temporary register.
					if(isdigit(*(line + i - 2))){
						sprintf(o1, "$t%c", *(line + i - 2));
					}
					//Assumes that the left oeprand is a variable we have encountered before.
					else{
						sprintf(o1, "$s%d", getVarsIndex(vars,*(line + i -2)));
					}
					//Checks if the right operand is a constant or a variable
					j = i + 2;
					if(isdigit(*(line +j)) || *(line + j) == '-'){
						//Accounts for constants with multiple digits.
						k = 0;
						while(isdigit(*(line + j)) || *(line + j) == '-'){
							*(o2 + k) = *(line + j);
							j++;
							k++;
						}
						k--;
					}
					// Assumes that the right operand is a variable.
					else {
						sprintf(o2, "$s%d", getVarsIndex(vars, *(line + i + 2)));
					}

					//Checks to see if the character after the right operand is a space to see if there is another operation after this one.
					if(*(line + i + 2 + k + 1) == ' '){
						sprintf(d, "$t%d", t);
						//Inserts the number of the register holding the result of this operation into the string to be accessed by the next string
						*(line + i + 2 + k) = '0' + t;
						//Cycles to next temporary register.
						t = (t == 9) ? 0 : t + 1;
						i++;
					}
					//If there are no more operations then assume that the result of this operation is what is being stored in the newest register.
					else{
						sprintf(d, "$s%d", getVarsNew(vars));
					}
					sub(d, o1, o2);
					break;
				case '*':
					isOperator = true;
					//Checks if the left operand is a temporary register or a variable. Since there can be no constants to the left of an operator I assume this is a temporary register.
					if(isdigit(*(line + i - 2))){
						sprintf(o1, "$t%c", *(line + i -2));
					}
					//Assumes that the left oeprand is a variable we have encountered before.
					else{
						sprintf(o1, "$s%d", getVarsIndex(vars,*(line + i -2)));
					}
					//Checks if the right operand is a constant or a variable
					j = i + 2;
					if(isdigit(*(line +j)) || *(line + j) == '-'){
						//Accounts for constants with multiple digits.
						k = 0;
						while(isdigit(*(line + j)) || *(line + j) == '-'){
							*(o2 + k) = *(line + j);
							j++;
							k++;
						}
						k--;
					}
					// Assumes that the right operand is a variable.
					else {
						sprintf(o2, "$s%d", getVarsIndex(vars, *(line + i + 2)));
					}

					//Checks to see if the character after the right operand is a space to see if there is another operation after this one.
					if(*(line + i + 2 + k + 1) == ' '){
						sprintf(d, "$t%d", t);
						//Inserts the number of the register holding the result of this operation into the string to be accessed by the next string
						*(line + i + 2 + k) = '0' + t;
						//printf("\ninserted char: %d",t);
						//Cycles to next temporary register.
						t = (t == 9) ? 0 : t + 1;
						i++;
					}
					//If there are no more operations then assume that the result of this operation is what is being stored in the newest register.
					else{
						sprintf(d, "$s%d", getVarsNew(vars));
					}
					if(isdigit(*(o2))|| *o2 == '-'){
						multi(d, o1, atoi(o2), t);
						t = (t == 9) ? 0 : t + 1;
					}
					else{
						mult(d, o1, o2);
					}
					break;

				case '/':
					isOperator = true;
					//Checks if the left operand is a temporary register or a variable. Since there can be no constants to the left of an operator I assume this is a temporary register.
					if(isdigit(*(line + i - 2))){
						sprintf(o1, "$t%c", *(line + i -2));
					}
					//Assumes that the left oeprand is a variable we have encountered before.
					else{
						sprintf(o1, "$s%d", getVarsIndex(vars,*(line + i -2)));
					}
					//Checks if the right operand is a constant or a variable
					j = i + 2;
					if(isdigit(*(line +j)) || *(line + j) == '-'){
						//Accounts for constants with multiple digits.
						k = 0;
						while(isdigit(*(line + j)) || *(line + j) == '-'){
							*(o2 + k) = *(line + j);
							j++;
							k++;
						}
						k--;
					}
					// Assumes that the right operand is a variable.
					else {
						sprintf(o2, "$s%d", getVarsIndex(vars, *(line + i + 2)));
					}

					//Checks to see if the character after the right operand is a space to see if there is another operation after this one.
					if(*(line + i + 2 + k + 1) == ' '){
						sprintf(d, "$t%d", t);
						//Inserts the number of the register holding the result of this operation into the string to be accessed by the next string
						*(line + i + k + 2) = '0' + t;
						//printf("\n substitute: %c",'0' + t);
						//Cycles to next temporary register.
						t = (t == 9) ? 0 : t + 1;
						i++;
					}
					//If there are no more operations then assume that the result of this operation is what is being stored in the newest register.
					else{
						sprintf(d, "$s%d", getVarsNew(vars));
					}
					if(isdigit(*(o2)) || *o2 == '-'){
						divi(d, o1, atoi(o2), t, L);
						L = L + 2;
						t = (atoi(o2) == 1 || atoi(o2) == -1) ? t : (t == 9) ? 0 : t + 1;
						//printf("\nincrementing t: %c\n",'0'+ t);
					}
					else{
						divide(d, o1, o2);

					}
					break;

				case '%':
					isOperator = true;
					//Checks if the left operand is a temporary register or a variable. Since there can be no constants to the left of an operator I assume this is a temporary register.
					if(isdigit(*(line + i - 2))){
						sprintf(o1, "$t%c", *(line + i -2));
					}
					//Assumes that the left oeprand is a variable we have encountered before.
					else{
						sprintf(o1, "$s%d", getVarsIndex(vars,*(line + i -2)));
					}
					//Checks if the right operand is a constant or a variable
					j = i + 2;
					if(isdigit(*(line +j)) || *(line + j) == '-'){
						//Accounts for constants with multiple digits.
						k = 0;
						while(isdigit(*(line + j)) || *(line + j) == '-'){
							*(o2 + k) = *(line + j);
							j++;
							k++;
						}
						k--;
					}
					// Assumes that the right operand is a variable.
					else {
						sprintf(o2, "$s%d", getVarsIndex(vars, *(line + i + 2)));
					}

					//Checks to see if the character after the right operand is a space to see if there is another operation after this one.
					if(*(line + i + 2 + k + 1) == ' '){
						sprintf(d, "$t%d", t);
						//Inserts the number of the register holding the result of this operation into the string to be accessed by the next string
						*(line + i + 2 + k) = '0' + t;
						//printf("\n substitute: %c",'0' + t);
						//Cycles to next temporary register.
						t = (t == 9) ? 0 : t + 1;
						i++;
					}
					//If there are no more operations then assume that the result of this operation is what is being stored in the newest register.
					else{
						sprintf(d, "$s%d", getVarsNew(vars));
					}
					mod(d,o1,o2);
					break;
			}
			memset(o1,'\0',32*sizeof(char));
			memset(o2,'\0',32*sizeof(char));
			memset(d,'\0',32*sizeof(char));
		}
		if(!isOperator){
			*(line + strlen(line) - 2) = '\0';
			printf("li $s%d,%s\n", getVarsNew(vars), line);
		free(d);
		free(o1);
		free(o2);
		}
	}
	fclose(file);
	return EXIT_SUCCESS;
}
