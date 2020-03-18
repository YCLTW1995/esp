// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "sha.hpp"

#define SHA_BLOCKSIZE 64

// Optional application-specific helper functions
void sha::do_sha(uint32_t input_size,uint32_t input_v_size, 
		unsigned char *indata,
		uint32_t *sha_info_digest, uint32_t *in_ct,
		uint32_t* result_sha_info_digest)
{
	int count, iter ;
	unsigned char *start_buff; 
	// Initial data 
	// bit_count[0] = sha_info_count_lo
	// bit_count[1] = sha_info_count_hi
	uint32_t bit_count[2] = {0L,0L} ;

	uint32_t sha_info_data[16] ;
	uint32_t * update_out = new uint32_t[23] ;

	for(iter = 0 ; iter < input_v_size ; iter ++ ){
		count = in_ct[iter] ;
		start_buff = &indata[iter*input_size] ;

		update_out = sha_update(start_buff, count,bit_count,sha_info_data);
		// sha_info_count_lo
		// sha_info_count_hi 
		//sha_info_data[16]
		//sha_info_digest[5]
		bit_count[0] = update_out[0] ;
		bit_count[1] = update_out[1] ;
		for(int i = 2 ; i < 18 ; i ++ ) sha_info_data[i-2] = update_out[i] ;
		for(int i = 18; i < 23 ; i ++ ) sha_info_digest[i-18] = update_out[i] ; 
		
	}
	// sha_final will return sha_info_digest

	uint32_t* final_sha_info_digest = new uint32_t[5] ;
	final_sha_info_digest =  sha_final(sha_info_data,sha_info_digest,bit_count[0],bit_count[1]) ;
	for(iter = 0; iter < 5 ; iter ++ ){
		result_sha_info_digest[iter] = final_sha_info_digest[iter];
	}


}

uint32_t* sha::sha_update(unsigned char  *buffer, int count, uint32_t* bit_count,uint32_t* sha_info_data)
{

	uint32_t sha_info_count_lo = bit_count[0] ;
	uint32_t sha_info_count_hi = bit_count[1] ;
	if((sha_info_count_lo + ((uint32_t)count<<3) ) < sha_info_count_hi){
		++sha_info_count_hi;
	}
	sha_info_count_lo += (uint32_t) count << 3;
	sha_info_count_hi += (uint32_t) count >> 29;

	// function data configure

	int m ;
	uint32_t tmp ;
	uint32_t * tmp_sha_info_digest = new uint32_t[5] ;

	while(count >= SHA_BLOCKSIZE){
	
		////// Local memcpy start
		
		/*
		   m = SHA_BLOCKSIZE / 4 ;
		   for(int i = 0 ; i < m ; i ++){
		   sha_info_data[i] = (buffer[4*i+3] & 0xff)
		   | (buffer[4*i+2] & 0xff)<<8
		   | (buffer[4*i+1] & 0xff)<<16
		   | (buffer[4*i+0] & 0xff)<<24;
		   }
		   */
		// 
		m = SHA_BLOCKSIZE /4 ;
		int tmp_i = 0 ;
		int tmp_2 = 0 ;
		while(m-->0){
			tmp = 0 ;
			tmp |= (0xFF & buffer[tmp_i]) ;
			tmp_i++;
			tmp |= (0xFF & buffer[tmp_i]) << 8;
			tmp_i++;
			tmp |= (0xFF & buffer[tmp_i]) << 16;
			tmp_i++ ;
			tmp |= (0xFF & buffer[tmp_i]) << 24;
			tmp_i++ ;
			sha_info_data[tmp_2] = tmp ;
			tmp_2++ ;
		}
		///// Local memcpy done 


		tmp_sha_info_digest = sha_transform(sha_info_data,sha_info_digest) ;
		for(int ii = 0 ; ii < 5 ; ii ++ ){
			sha_info_digest[ii] = tmp_sha_info_digest[ii];
		}

		buffer += SHA_BLOCKSIZE ;
		count -= SHA_BLOCKSIZE;
	}
	
	////// Local memcpy start
	m = count/4 ;
	for(int i = 0 ; i < m ; i ++){
		sha_info_data[i] = (buffer[4*i+3] & 0xff)
			| (buffer[4*i+2] & 0xff)<<8
			| (buffer[4*i+1] & 0xff)<<16
			| (buffer[4*i+0] & 0xff)<<24;
	}
	///// Local memcpy done 
	/*
	   tmp = 0 ;
	   m = count / 4 ;
	   while(m-- > 0){
	   tmp = 0;
	   tmp |= 0xFF & *buffer++;
	   tmp |= (0xFF & *buffer++) << 8;
	   tmp |= (0xFF & *buffer++) << 16;
	   tmp |= (0xFF & *buffer++) << 24;
	 *sha_info_data = tmp;
	 sha_info_data++;
	 }*/
	///// Local memcpy done 
	// sha_info_count_lo
	// sha_info_count_hi 
	//sha_info_data[16]
	//sha_info_digest[5]
	uint32_t  update_out[23] ;
	update_out[0] = sha_info_count_lo;
	update_out[1] = sha_info_count_hi;
	for(int i = 2 ; i < 18 ; i ++ ) update_out[i] = sha_info_data[i-2] ;
	for(int i = 18; i < 23 ; i ++ ) update_out[i] = sha_info_digest[i-18] ; 
	return update_out ;
}

// this will return sha_info_digest
uint32_t* sha::sha_transform(uint32_t* sha_info_data, uint32_t*sha_info_digest)
{
	int i ;
	uint32_t temp,A,B,C,D,E,W[80];
	for (i = 0; i < 16; ++i){
		W[i] = sha_info_data[i];
	}
	for (i = 16; i < 80; ++i){
		W[i] = W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16];
	}
	A = sha_info_digest[0];
	B = sha_info_digest[1];
	C = sha_info_digest[2];
	D = sha_info_digest[3];
	E = sha_info_digest[4];
	uint32_t CONST1 = 0x5a827999L;
	uint32_t CONST2 = 0x6ed9eba1L;
	uint32_t CONST3 = 0x8f1bbcdcL;
	uint32_t CONST4 = 0xca62c1d6L;

	uint32_t f1,f2,f3,f4 ;
	uint32_t rot32_a5, rot32_b30 ;
	
	for(i = 0 ; i < 20 ; ++ i){
		//FUNC(1,i)
		f1 = ((B & C) | (~B & D)) ;
		rot32_a5 = ((A << 5) | (A >> (32 - 5))) ;
		rot32_b30 = ((B << 30) | (B >> (32 - 30))) ;
		temp = rot32_a5 + f1 + E + W[i] + CONST1;
		E = D; 
		D = C; 
		C = rot32_b30 ; 
		B = A; 
		A = temp;
	}
	
	for(i = 20 ; i < 40 ; ++i){
		//FUNC(2,i)
		f2 = (B ^ C ^ D);
		rot32_a5 = ((A << 5) | (A >> (32 - 5))) ;
		rot32_b30 = ((B << 30) | (B >> (32 - 30))) ;
		temp = rot32_a5 + f2 + E + W[i] + CONST2;	
		E = D; 
		D = C; 
		C = rot32_b30; 
		B = A; 
		A = temp;
	}
	for (i = 40; i < 60; ++i){
		//FUNC (3, i);
		f3 = ((B & C) | (B & D) | (C & D));
		rot32_a5 = ((A << 5) | (A >> (32 - 5))) ;
		rot32_b30 = ((B << 30) | (B >> (32 - 30))) ;
		temp = rot32_a5 + f3 + E + W[i] + CONST3;	
		E = D; 
		D = C; 
		C = rot32_b30; 
		B = A; 
		A = temp;
	}
	for (i = 60; i < 80; ++i){
		//FUNC (4, i);
		f4 = (B ^ C ^ D);
		rot32_a5 = ((A << 5) | (A >> (32 - 5))) ;
		rot32_b30 = ((B << 30) | (B >> (32 - 30))) ;
		temp = rot32_a5 + f4 + E + W[i] + CONST4;	
		E = D; 
		D = C; 
		C = rot32_b30; 
		B = A; 
		A = temp;
	}

	sha_info_digest[0] += A;
	sha_info_digest[1] += B;
	sha_info_digest[2] += C;
	sha_info_digest[3] += D;
	sha_info_digest[4] += E;
	return sha_info_digest ;
}

uint32_t* sha::sha_final(uint32_t* sha_info_data,uint32_t* sha_info_digest,
		uint32_t sha_info_count_lo, uint32_t sha_info_count_hi)
	// sha_info_count_lo
	// sha_info_count_hi 
	//sha_info_data[16]
	//sha_info_digest[5]
{
	// input configuration
	/*uint32_t sha_info_data[16] ;
	  uint32_t sha_info_digest[5] ;
	  uint32_t sha_info_count_lo = input[0] ;
	  uint32_t sha_info_count_hi = input[1] ;

	  for(int i = 2 ; i < 18 ; i ++ ) {
	  sha_info_data[i-2] = input[i] ;
	  }
	  for(int i = 18; i < 23 ; i ++ ) {
	  sha_info_digest[i-18] = input[i] ; 
	  }
	  */
	uint32_t count ;
	uint32_t lo_bit_count;
	uint32_t hi_bit_count;

	uint32_t * tmp_sha_info_digest = new uint32_t[5] ;

	lo_bit_count = sha_info_count_lo;
	hi_bit_count = sha_info_count_hi;
	count = (int) ((lo_bit_count >> 3) & 0x3f);
	sha_info_data[count++] = 0x80; 


	if(count > 56){
		//Local mem set start
		int shift_num = count ; // e
		int num_0 = (64-count)/4 ; // = m 
		int iter = 0 ;
		while( shift_num-->0){
			iter ++ ;
		}
		while(num_0-->0){
			sha_info_data[iter] = 0;
			iter ++ ;
		}
		// Local mem set done 
		tmp_sha_info_digest = sha_transform(sha_info_data,sha_info_digest) ;
		for(int ii = 0 ; ii < 5 ; ii ++ ){
			sha_info_digest[ii] = tmp_sha_info_digest[ii];
		}
		//local mem set start
		//local_memset (sha_info_data, 0, 56, 0);
		shift_num = 0 ;//e
		num_0 = 56/4 ;// m
		iter = 0 ;
		while(shift_num-->0){
			iter ++ ;
		}
		while(num_0-->0){
			sha_info_data[iter] = 0 ;
			iter ++ ;
		}
		//local mem set done
	}
	else{
		//local mem set start
		//local_memset (sha_info_data, 0, 56 - count, count);
		int num_0 = (56-count)/4 ; // m
		int shift_num = count ; // e
		int iter = 0;
		while(shift_num-->0){
			iter++ ;
		}
		while(num_0 -- > 0){
			sha_info_data[iter] = 0;
			iter ++ ;
		}
		//local mem set done
	}
	sha_info_data[14] = hi_bit_count ;
	sha_info_data[15] = lo_bit_count ;
	tmp_sha_info_digest = sha_transform(sha_info_data,sha_info_digest) ;
	for(int ii = 0 ; ii < 5 ; ii ++ ){
		sha_info_digest[ii] = tmp_sha_info_digest[ii];
	}
	return sha_info_digest;
}
