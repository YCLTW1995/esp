#ifndef _VITDECODER_H_
#define _VITDECODER_H_

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#ifndef __user
#define __user
#endif
#endif /* __KERNEL__ */

#include <esp.h>
#include <esp_accelerator.h>

struct vitdecoder_access {
	struct esp_access esp;
	/* <<--regs-->> */
	unsigned src_offset;
	unsigned dst_offset;
};

#define VITDECODER_IOC_ACCESS	_IOW ('S', 0, struct vitdecoder_access)

#endif /* _VITDECODER_H_ */