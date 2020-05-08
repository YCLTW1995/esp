// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "sha.hpp"
#include "sha_directives.hpp"

// Functions

#include "sha_functions.hpp"
// Processes

void sha::load_input()
{

    // Reset
    {
        HLS_PROTO("load-reset");

        this->reset_load_input();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_size;
    int32_t input_v_size;
    {
        HLS_PROTO("load-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_size = config.input_size;
        input_v_size = config.input_v_size;
    }

    // Load
    {
        HLS_PROTO("load-dma");
        wait();

        bool ping = true;
        uint32_t offset = 0;

        // Batching
        for (uint16_t b = 0; b < 1; b++)
        {
            wait();
#if (DMA_WORD_PER_BEAT == 0)
            uint32_t length = input_v_size * input_size;
#else
            uint32_t length = round_up(input_v_size * input_size, DMA_WORD_PER_BEAT);
#endif
            // Chunking
            for (int rem = length; rem > 0; rem -= PLM_IN_WORD)
            {
                wait();
                // Configure DMA transaction
                uint32_t len = rem > PLM_IN_WORD ? PLM_IN_WORD : rem;
#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                dma_info_t dma_info(offset * DMA_BEAT_PER_WORD, len * DMA_BEAT_PER_WORD, DMA_SIZE);
#else
                dma_info_t dma_info(offset / DMA_WORD_PER_BEAT, len / DMA_WORD_PER_BEAT, DMA_SIZE);
#endif
                offset += len;

                this->dma_read_ctrl.put(dma_info);

#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                for (uint16_t i = 0; i < len; i++)
                {
                    sc_dt::sc_bv<DATA_WIDTH> dataBv;

                    for (uint16_t k = 0; k < DMA_BEAT_PER_WORD; k++)
                    {
                        dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH) = this->dma_read_chnl.get();
                        wait();
                    }

                    // Write to PLM
                    if (ping)
                        plm_in_ping[i] = dataBv.to_int64();
                    else
                        plm_in_pong[i] = dataBv.to_int64();
                }
#else
                for (uint16_t i = 0; i < len; i += DMA_WORD_PER_BEAT)
                {
                    HLS_BREAK_DEP(plm_in_ping);
                    HLS_BREAK_DEP(plm_in_pong);

                    sc_dt::sc_bv<DMA_WIDTH> dataBv;

                    dataBv = this->dma_read_chnl.get();
                    wait();

                    // Write to PLM (all DMA_WORD_PER_BEAT words in one cycle)
                    for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    {
                        HLS_UNROLL_SIMPLE;
                        if (ping)
                            plm_in_ping[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                        else
                            plm_in_pong[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                    }
                }
#endif
                this->load_compute_handshake();
                ping = !ping;
            }
        }
    }

    // Conclude
    {
        this->process_done();
    }
}



void sha::store_output()
{
    // Reset
    {
        HLS_PROTO("store-reset");

        this->reset_store_output();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_size;
    int32_t input_v_size;
    {
        HLS_PROTO("store-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_size = config.input_size;
        input_v_size = config.input_v_size;
    }

    // Store
    {
        HLS_PROTO("store-dma");
        wait();

        bool ping = true;
#if (DMA_WORD_PER_BEAT == 0)
        uint32_t store_offset = (input_v_size * input_size) * 1;
#else
        uint32_t store_offset = round_up(input_v_size * input_size, DMA_WORD_PER_BEAT) * 1;
#endif
        uint32_t offset = store_offset;

        wait();
        // Batching
        for (uint16_t b = 0; b < 1; b++)
        {
            wait();
#if (DMA_WORD_PER_BEAT == 0)
            uint32_t length = 5;
#else
            uint32_t length = round_up(5, DMA_WORD_PER_BEAT);
#endif
            // Chunking
            for (int rem = length; rem > 0; rem -= PLM_OUT_WORD)
            {

                this->store_compute_handshake();

                // Configure DMA transaction
                uint32_t len = rem > PLM_OUT_WORD ? PLM_OUT_WORD : rem;
#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                dma_info_t dma_info(offset * DMA_BEAT_PER_WORD, len * DMA_BEAT_PER_WORD, DMA_SIZE);
#else
                dma_info_t dma_info(offset / DMA_WORD_PER_BEAT, len / DMA_WORD_PER_BEAT, DMA_SIZE);
#endif
                offset += len;

                this->dma_write_ctrl.put(dma_info);

#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                for (uint16_t i = 0; i < len; i++)
                {
                    // Read from PLM
                    sc_dt::sc_int<DATA_WIDTH> data;
                    wait();
                    if (ping)
                        data = plm_out_ping[i];
                    else
                        data = plm_out_pong[i];
                    sc_dt::sc_bv<DATA_WIDTH> dataBv(data);

                    uint16_t k = 0;
                    for (k = 0; k < DMA_BEAT_PER_WORD - 1; k++)
                    {
                        this->dma_write_chnl.put(dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH));
                        wait();
                    }
                    // Last beat on the bus does not require wait(), which is
                    // placed before accessing the PLM
                    this->dma_write_chnl.put(dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH));
                }
#else
                for (uint16_t i = 0; i < len; i += DMA_WORD_PER_BEAT)
                {
                    sc_dt::sc_bv<DMA_WIDTH> dataBv;

                    // Read from PLM
                    wait();
                    for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    {
                        HLS_UNROLL_SIMPLE;
                        if (ping)
                            dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_ping[i + k];
                        else
                            dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_pong[i + k];
                    }
                    this->dma_write_chnl.put(dataBv);
                }
#endif
                ping = !ping;
            }
        }
    }

    // Conclude
    {
        this->accelerator_done();
        this->process_done();
    }
}


void sha::compute_kernel()
{
    // Reset
    {
        HLS_PROTO("compute-reset");

        this->reset_compute_kernel();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    uint32_t input_size;
    uint32_t input_v_size;
    {
        HLS_PROTO("compute-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_size = config.input_size;
        input_v_size = config.input_v_size;
        in_length = input_v_size * input_size;
        // set sha info digest data 
        sha_info_digest[0] = 0x67452301L;
		sha_info_digest[1] = 0xefcdab89L;
		sha_info_digest[2] = 0x98badcfeL;
		sha_info_digest[3] = 0x10325476L;
		sha_info_digest[4] = 0xc3d2e1f0L;
		in_ct[0] = 8192 ;
		in_ct[1] = 8192 ;
        //uint32_t* result_sha_info_digest = new uint32_t[5] ;
		///test
        /*
        input_size = 1024 ;
        input_v_size = 2 ;
        in_ct[0] = 1024 ;
        in_ct[1] = 1024;
        */
    }


    // Compute
    bool ping = true;
    bool out_ping = true; 
    {
        for (uint16_t b = 0; b < 1; b++)
        {
            
            uint32_t out_length = 5;
            int out_rem = out_length;

            for (int in_rem = in_length; in_rem > 0; in_rem -= PLM_IN_WORD)
            {

                uint32_t in_len  = in_rem  > PLM_IN_WORD  ? PLM_IN_WORD  : in_rem;
                uint32_t out_len = out_rem > PLM_OUT_WORD ? PLM_OUT_WORD : out_rem;

                this->compute_load_handshake();

                // Computing phase implementation
                // set input value 
                uint32_t tmp_total_size = 0 ;
                for(int i = 0 ; i < in_len ; i ++ ){
                	if (ping){
                        if(i != 16383)
                            indata[i] = (unsigned char)plm_in_ping[i];
                        else{
                            tmp_total_size = (uint32_t)plm_in_ping[i] ;
                        }
                    }
                    else{
                        if(i!=16383){
                            indata[i] = (unsigned char)plm_in_pong[i]; 
                        }
                        else{
                            tmp_total_size = (uint32_t)plm_in_ping[i] ;
                        }
                    }
                }
                // do data preprocess
                //uint32_t tmp_total_size = indata[16383] ;
                indata[16383] = 0 ;
                
                if (tmp_total_size % 2 != 0) tmp_total_size +=1 ;
                uint32_t tmpvalue = 1;
                while(tmpvalue < tmp_total_size){
                    tmpvalue = tmpvalue *2 ;
                }
                if (tmpvalue >= 16384) tmpvalue = 16384 ;
                uint32_t updated_input_size = tmpvalue/2 ;
                uint32_t updated_input_v_size = 2 ;
                in_ct[0] = updated_input_size ;
                in_ct[1] = updated_input_size ;
                
                /*
                uint32_t updated_input_size = 1024 ;
                uint32_t updated_input_v_size = 2 ;
                in_ct[0] = updated_input_size ;
                in_ct[1] = updated_input_size ;
                */
                // data preprocess done 
                printf("\nSRC Total Input Size is %d\n",updated_input_size*2);
                do_sha(updated_input_size, updated_input_v_size,
                	indata,sha_info_digest,in_ct,result_sha_info_digest);
                

                /****Original */
                for (int i = 0; i < out_length; i++) { 
                    if (ping)
                        plm_out_ping[i] = result_sha_info_digest[i];
                    else
                        plm_out_pong[i] = result_sha_info_digest[i];
                }
                /*****/ 
                out_rem -= PLM_OUT_WORD;
                this->compute_store_handshake();
                ping = !ping;
            }
        }

        // Conclude
        {
            this->process_done();
        }
    }
}
