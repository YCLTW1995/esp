int32_t* system_t::read_txt(){
	char ch;
	int32_t arr[16384] ;
	int ct = 0 ;
	FILE *file_ptr = NULL ;
	if(file_ptr == NULL){
		file_ptr = fopen("../tb/in.txt","r");
	}
	if(file_ptr == NULL){
		printf("Failed to open input text file\n");
	}
	else{
		while ((ch = fgetc(file_ptr)) != EOF){
			arr[ct++] = (int32_t) ch ;
		}
	}
	fclose(file_ptr);
	file_ptr = NULL;
	//arr[16383] store the total count of the input 
	for(int i = ct ; i < 16384 ; i ++ ) arr[i] = 0 ;
	arr[16383] = (int32_t)ct ;
	
	return arr ;
}