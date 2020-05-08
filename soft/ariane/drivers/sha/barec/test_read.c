#include <stdlib.h>
//#include <stdio.h>

int main (){
	char ch;
	int arr[16384] ;
	int ct = 0 ;
	FILE *file_ptr = NULL ;
	if(file_ptr == NULL){
		file_ptr = fopen("./in.txt","r");
	}
	if(file_ptr == NULL){
		printf("Failed to open input text file\n");
	}
	else{
		while ((ch = fgetc(file_ptr)) != EOF){
			arr[ct++] = (int) ch ;
		}
	}
	fclose(file_ptr);
	file_ptr = NULL;
	//arr[16383] store the total count of the input 
	int i = 0 ;
	for( i = ct ; i < 16384 ; i ++ ) arr[i] = 0 ;
	arr[16383] = ct ;
	
	printf("\nTotal Input Size is %d\n",ct);
	return 0 ;
}