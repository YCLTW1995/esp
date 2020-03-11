#ifndef __ESP_CFG_000_H__
#define __ESP_CFG_000_H__

#include "libesp.h"

typedef int32_t token_t;

/* <<--params-def-->> */
#define INPUT_SIZE 8192
#define INPUT_V_SIZE 2

/* <<--params-->> */
const int32_t input_size = INPUT_SIZE;
const int32_t input_v_size = INPUT_V_SIZE;

#define NACC 1

esp_thread_info_t cfg_000[] = {
	{
		.run = true,
		.devname = "sha.0",
		.type = sha,
		/* <<--descriptor-->> */
		.desc.sha_desc.input_size = INPUT_SIZE,
		.desc.sha_desc.input_v_size = INPUT_V_SIZE,
		.desc.sha_desc.src_offset = 0,
		.desc.sha_desc.dst_offset = 0,
		.desc.sha_desc.esp.coherence = ACC_COH_NONE,
		.desc.sha_desc.esp.p2p_store = 0,
		.desc.sha_desc.esp.p2p_nsrcs = 0,
		.desc.sha_desc.esp.p2p_srcs = {"", "", "", ""},
	}
};

#endif /* __ESP_CFG_000_H__ */
