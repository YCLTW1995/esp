#include <stdlib.h>
#include <stdio.h>

int main (){
	char ch;
	int arr[16384] ;
	int ct = 0 ;
	FILE *file_out = NULL ;
	//arr[16383] store the total count of the input 
	int i = 0 ;
	int r ;
	char rc ;
	for(i = 0 ; i < 16384 ; i ++){
		r = rand() % 26 ;
		rc = 'a' +r ;
		arr[i] = (int) rc ;
	}
	arr[16383] = 16383 ;
	if(file_out == NULL){
		file_out = fopen("./data_16383.h","w");
	}
	if(file_out == NULL){
		printf("Failed to open out text file\n");
	}
	for( i = 0 ; i < 16384 ; i ++ ){
		fprintf (file_out, "in[%d]=%d;\n",i, arr[i]);
	}
	ct = 16383 ;
	printf("\nTotal Input Size is %d\n",ct);
	return 0 ;
}
