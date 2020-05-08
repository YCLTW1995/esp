// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __SHA_HPP__
#define __SHA_HPP__

#include "sha_conf_info.hpp"
#include "sha_debug_info.hpp"

#include "esp_templates.hpp"

#include "sha_directives.hpp"


#define __round_mask(x, y) ((y)-1)
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
/* <<--defines-->> */
#define DATA_WIDTH 32
#define DMA_SIZE SIZE_WORD
#define PLM_OUT_WORD 6
#define PLM_IN_WORD 16384

class sha : public esp_accelerator_3P<DMA_WIDTH>
{
public:
    // Constructor
    SC_HAS_PROCESS(sha);
    sha(const sc_module_name& name)
    : esp_accelerator_3P<DMA_WIDTH>(name)
        , cfg("config")
    {
        // Signal binding
        cfg.bind_with(*this);

        // Map arrays to memories
        /* <<--plm-bind-->> */
        HLS_MAP_plm(plm_out_pong, PLM_OUT_NAME);
        HLS_MAP_plm(plm_out_ping, PLM_OUT_NAME);
        HLS_MAP_plm(plm_in_pong, PLM_IN_NAME);
        HLS_MAP_plm(plm_in_ping, PLM_IN_NAME);
    }

    // Processes

    // Load the input data
    void load_input();

    // Computation
    void compute_kernel();

    // Store the output data
    void store_output();

    uint32_t preprocess(unsigned char * input);

    void do_sha(uint32_t input_size,uint32_t input_v_size,
                unsigned char *indata, 
                uint32_t *sha_info_digest, uint32_t *in_ct,
                uint32_t* result_sha_info_digest) ;

    uint32_t* sha_update(unsigned char *buffer, int count, 
                        uint32_t* bit_count,uint32_t* sha_info_data);
    uint32_t* sha_transform(uint32_t* sha_info_data, uint32_t*sha_info_digest);
    uint32_t* sha_final(uint32_t* sha_info_data,uint32_t* sha_info_digest,
                        uint32_t sha_info_count_lo, uint32_t sha_info_count_hi);
    // Configure sha
    esp_config_proc cfg;

    // Functions

    // Private local memories
    sc_dt::sc_int<DATA_WIDTH> plm_in_ping[PLM_IN_WORD];
    sc_dt::sc_int<DATA_WIDTH> plm_in_pong[PLM_IN_WORD];
    sc_dt::sc_int<DATA_WIDTH> plm_out_ping[PLM_OUT_WORD];
    sc_dt::sc_int<DATA_WIDTH> plm_out_pong[PLM_OUT_WORD];

    unsigned char indata[PLM_IN_WORD] ;
    uint32_t sha_info_digest[5] ;
    uint32_t in_ct[2] ;

    uint32_t in_length ;
    uint32_t result_sha_info_digest[5];
};


#endif /* __SHA_HPP__ */
