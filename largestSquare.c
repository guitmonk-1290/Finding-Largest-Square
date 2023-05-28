/*
	Author: Aditya Rana
	E-mail: adityarana1290@gmail.com
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "util.h"

#define MAX_SIZE 20
#define FILENAME "mapSol.txt"

int* initMat(char** mat, int* resMat, long matCols, long matRows, char* chrs) {
	int i=0, index=0;

	while(i<matCols) {
		*(resMat+i) = (mat[0][i]==chrs[2] ? 1 : 0); 
		i++;
	}
	
	i=0;
	while(i<matRows) {
		*(resMat+index) = (mat[i][0]==chrs[2] ? 1 : 0);
		index += matCols;
		i++;
	}

	return resMat;
}

/*
	getSol(char** mat, long rows, long cols, char* chrs)
		-> replaces the empty 'vacant' characters with 'full' characters forming the largest square
*/
void getSol(char** mat, int* resMat, long matRows, long matCols, char* chrs) {
	int x =matRows + (matCols-1), i=0, index=matCols+1;
	x = (matRows*matCols) - x;
	int row=1, col=1, max_sq=0, max_index;

	resMat = initMat(mat, resMat, matCols, matRows, chrs);

	while(x--) {
		if (index % matCols == 0) {
			row += 1;
			col = 1;
			index += 1;
		}
		if(mat[row][col] == chrs[1]) {
			*(resMat+index) = 0;
		}
		else {
			// get minimum of left, top and topleft values from the resMat matrix respectively [DP]
			*(resMat+index) = min(*(resMat+(index-1)), min(*(resMat+(index-matCols)), *(resMat+(index-(matCols+1))))) + 1;
			if (max(max_sq, *(resMat+index)) != max_sq) {
				max_sq = *(resMat+index);
				max_index = index;
			}
		}	
		index += 1; col += 1;
	}
	
	i=0, x = max_sq * max_sq;
	while(x--) {
		if(i==max_sq) {
			max_index -= matCols; i = 0;
		}
		mat[(max_index-i)/matRows][(max_index-i)%matCols] = chrs[0];
		i++;
	}
	return;
}


/*
	save_res(char** mat, char* filename)
		-> saves the result in a text file 
*/
void save_res(char** mat, long matRows, char* filename) {
	FILE* fptr = fopen(filename, "w");
	if (fptr == NULL) {
		perror("[ERROR] fopen() failed");
		exit(1);
	}
	int i=0;
	while(i<matRows) {
		if (fputs(mat[i], fptr) == 0) {
			perror("[ERROR] fputs() failed");
			exit(1);
		}
		i++;
	}
	if (fclose(fptr) == -1) {
		perror("[ERROR] fclose() failed");
		exit(1);
	}
}

void printSol(char** mat, long matRows, char* chrs) {
	int i=0;
	printf("%d%c%c%c\n", (int)matRows, chrs[2], chrs[1], chrs[0]);
	while(i<matRows) {
		printf("%s", mat[i]);
		i++;
	}
}


/*
	get_Lsquare(FILE* fptr, long rows, long cols, char* chrs)
		-> finds the largest square in the grid and saves the result in a file. chrs is a character array storing special characters
*/
void get_Lsquare(FILE* fptr, long matRows, long matCols, char* chrs) {
	char* mat[matRows];
	int i = 0;
	size_t chars, len=0;

	while(i<matRows) {
		char* str = (char*) malloc(matCols * sizeof(char));
		if ((chars = getline(&str, &len, fptr)) == (size_t)-1) {
			perror("[ERROR] getline() failed");
			exit(1);
		}
		mat[i] = str;
		i++;
	}
	int resMat[matRows][matCols];

	getSol(mat, &resMat, matRows, matCols, chrs);
	printSol(mat, matRows, chrs);
	
	save_res(mat, matRows, FILENAME);

	// free space
	i = 0;
	while(i<matRows) {
		free(mat[i]);
		i++;
	}
}

int main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("Usage: ./program <filename>");
		exit(1);
	}
	FILE* fptr = fopen(argv[1], "r");
	if(fptr == NULL) {
		perror("[ERROR] fopen() failed");
		return 1;
	}

	// file variables
	char *line = NULL;
	size_t chars, len = 0;

	// square variables
	char matSize[MAX_SIZE], chrs[3];
	unsigned int fLen;
	long matSizeRows_in, matSizeCols_in;

	chars = getline(&line, &len, fptr);
	fLen = (unsigned int)chars;
	strncpy(matSize, line, chars-4);
	chrs[0] = line[chars-2];
	chrs[1] = line[chars-3];
	chrs[2] = line[chars-4];
	char* temp_ptr;
	matSizeRows_in = strtol(matSize, &temp_ptr, 10);
	chars = getline(&line, &len, fptr);
	matSizeCols_in = (long)chars-1;
	fseek(fptr, fLen, SEEK_SET); 

	get_Lsquare(fptr, matSizeRows_in, matSizeCols_in, chrs);

	if(line)
		free(line);

	if ((fclose(fptr)) == -1) {
		perror("[ERROR] fclose() failed");
		return 1;
	}
	return 0;
}
