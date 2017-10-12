#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define SIZE 9 // 9*9 Sudoku

using namespace std;

// Array to store the input and output matrices, & the index of asterisks
int arrayIn[SIZE*SIZE];
int arrayOut[SIZE*SIZE];
int astIndex[SIZE];

// Variables to count cell types
int nbGiven = 0;
int nbAsterisk = 0;
int nbClause = 0;

// Functions to be called
void readInput();
void createFormula();
string solSAT();
void getOutput();

int main(){
	readInput();
	createFormula();
	
	cout << endl;
	
	getOutput();
	
	return 0;
}

void readInput(){
	ifstream input;
	input.open("input.txt");
	
	// Checks whether input.txt is opened by input filestream
	if(!input){
		cout << "Unable to find input.txt" << endl;
		exit(-1);
	}

	// Inserts sudoku input matrix into arrayIn + counts number of asterisks & given numbers
	char tempChar;
	for(int i=0; input >> skipws >> tempChar; i++){ // skips all whitespaces in stream ex) ' ', '\n'
		if(tempChar == '*'){
			arrayIn[i] = '*'; // For display purpose
			astIndex[nbAsterisk++] = (9 * i) + 1; 
		}
		else{
			if(tempChar != '0')
				nbGiven++;
			arrayIn[i] = tempChar -'0'; // subtract '0' considering type conversion char->int
		}
	}
	
	cout << "Given Sudoku problem:" << endl;
	
	// Prints out arrayIn
	for(int i=0; i<81; i++){
		if(arrayIn[i] == '*')
			cout << "* ";
		else
			cout << arrayIn[i] << ' ';
		if((i+1) % 9 == 0)
			cout << endl;
	}	
	
	input.close();
}

void createFormula(){
	ofstream formula;
	formula.open("formula.txt");
	
/*----------------Get number of variables and clauses------------------*/
	nbClause = 3159/*fixed*/ + nbGiven;
	formula << "p cnf 729 " << nbClause << endl; // 9^3 = 729 variables
/*-------------------------(1)Row Constraint----------------------------*/
	for(int i=1; i<=9; i++){
		for(int n=1; n<=9; n++){ // n = {1, 2, ... 9}
			for(int j=1; j<=9; j++)
				formula << 81 * (i-1) + 9 * (j-1) + n << " ";
			formula << "0" << endl;
		}
	}
/*-------------------------(2)Column Constraint-------------------------*/
	for(int j=1; j<=9; j++){
		for(int n=1; n<=9; n++){ // n = {1, 2, ... 9}
			for(int i=1; i<=9; i++)
				formula << 81 * (i-1) + 9 * (j-1) + n << " ";
			formula << "0" << endl;
		}
	}	
/*-------------------------(3)Duplicate Constraint----------------------*/
	for(int i=1; i<=9; i++){
		for(int j=1; j<=9; j++){
			for(int n=1; n<9; n++){
				for(int np=n+1; np <=9; np++)
					formula << "-" << 81 * (i-1) + 9 * (j-1) + n << " -" << 81 * (i-1) + 9 * (j-1) + np << " 0" << endl; 
			}
		}
	}	
/*-------------------------(4)Sub-Grid Constraint-----------------------*/
	for(int r=0; r<=2; r++){
		for(int s=0; s<=2; s++){
			for(int n=1; n<=9; n++){
				for(int i=1; i<=3; i++){
					for(int j=1; j<=3; j++){
						formula << 81 * (i-1) + 9 * (j-1) + n + 243 * r + 27 * s << " ";
					}
				}
				formula << "0" << endl;
			}
		}
	}
/*-------------------------(+)Pre-assigned Constraint-------------------*/
	for(int i=0; i<81; i++){
		if(arrayIn[i] != 0 && arrayIn[i] != '*') // if a value is assigned
			formula << arrayIn[i] + 9*i << " 0" << endl;
	}
/*-------------------------(+)Asterisk Constraint-----------------------*/	
	for(int i=0; i<nbAsterisk-1; i++){
		for(int j=0; j<9; j++){
			formula << "-" << astIndex[i] + j << " " << astIndex[i+1] + j << " 0" << endl;
		}
	}

	formula.close();
}

string solSAT(){
	char buffer[128];
	string result = "";
	const char *cmd = "z3 -dimacs formula.txt";
	
	FILE *pipe = popen(cmd, "r");
	
	if(!pipe){
		cout << "Error opening formula.txt" << endl;
		exit(-1);
	}
	
	while(!feof(pipe)){
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

void getOutput(){
	string solution = solSAT(); // gets SAT solution in string format

	int arraySol[SIZE*SIZE*SIZE];

	// If the formula is unsatisfiable ('u' being the first letter case of stream), exit program
	if(solution[0] == 'u'){
		cout << "Formula unsatisfiable!" << endl;
		exit(-1);
	}

	// Extract integer values from z3 solution
	for (int i = 0; i < solution.length(); ++i)
    {
		if (isalpha(solution[i]))
            solution[i] = ' '; 	// Get rid of alphabets
    }
	
	// Insert solution sring value into stringstream
    stringstream SATout(solution);
    
	int n;
	
	for(int i=0; i<81; i++){
		for(int j=0; j<9; j++){
			SATout >> skipws >> arraySol[j];
			if(arraySol[j] > 0){
				n = arraySol[j] - (i*9);
				arrayOut[i] = n;
			}
		}
	}
	
	// Prints output & saves them to output.txt
	ofstream output;
	output.open("output.txt");
	
	cout << "Solution to the problem:" << endl;
	
	for(int i=0; i<81; i++){
		cout << arrayOut[i] << ' ';
		output << arrayOut[i] << ' ';
		if((i+1) % 9 == 0){
			cout << endl;
			output << endl;			
		}
	}
	
	output.close();
}
