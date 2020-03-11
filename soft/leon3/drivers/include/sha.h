#ifndef _SHA_H_
#define _SHA_H_

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

struct sha_access {
	struct esp_access esp;
	/* <<--regs-->> */
	unsigned input_size;
	unsigned input_v_size;
	unsigned src_offset;
	unsigned dst_offset;
};

#define SHA_IOC_ACCESS	_IOW ('S', 0, struct sha_access)

#endif /* _SHA_H_ */
