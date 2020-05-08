#include <linux/of_device.h>
#include <linux/mm.h>

#include <asm/io.h>

#include <esp_accelerator.h>
#include <esp.h>

#include "sha.h"

#define DRV_NAME	"sha"

/* <<--regs-->> */
#define SHA_INPUT_SIZE_REG 0x44
#define SHA_INPUT_V_SIZE_REG 0x40

struct sha_device {
	struct esp_device esp;
};

static struct esp_driver sha_driver;

static struct of_device_id sha_device_ids[] = {
	{
		.name = "SLD_SHA",
	},
	{
		.name = "eb_18a",
	},
	{
		.compatible = "sld,sha",
	},
	{ },
};

static int sha_devs;

static inline struct sha_device *to_sha(struct esp_device *esp)
{
	return container_of(esp, struct sha_device, esp);
}

static void sha_prep_xfer(struct esp_device *esp, void *arg)
{
	struct sha_access *a = arg;

	/* <<--regs-config-->> */
	iowrite32be(a->input_size, esp->iomem + SHA_INPUT_SIZE_REG);
	iowrite32be(a->input_v_size, esp->iomem + SHA_INPUT_V_SIZE_REG);
	iowrite32be(a->src_offset, esp->iomem + SRC_OFFSET_REG);
	iowrite32be(a->dst_offset, esp->iomem + DST_OFFSET_REG);

}

static bool sha_xfer_input_ok(struct esp_device *esp, void *arg)
{
	/* struct sha_device *sha = to_sha(esp); */
	/* struct sha_access *a = arg; */

	return true;
}

static int sha_probe(struct platform_device *pdev)
{
	struct sha_device *sha;
	struct esp_device *esp;
	int rc;

	sha = kzalloc(sizeof(*sha), GFP_KERNEL);
	if (sha == NULL)
		return -ENOMEM;
	esp = &sha->esp;
	esp->module = THIS_MODULE;
	esp->number = sha_devs;
	esp->driver = &sha_driver;
	rc = esp_device_register(esp, pdev);
	if (rc)
		goto err;

	sha_devs++;
	return 0;
 err:
	kfree(sha);
	return rc;
}

static int __exit sha_remove(struct platform_device *pdev)
{
	struct esp_device *esp = platform_get_drvdata(pdev);
	struct sha_device *sha = to_sha(esp);

	esp_device_unregister(esp);
	kfree(sha);
	return 0;
}

static struct esp_driver sha_driver = {
	.plat = {
		.probe		= sha_probe,
		.remove		= sha_remove,
		.driver		= {
			.name = DRV_NAME,
			.owner = THIS_MODULE,
			.of_match_table = sha_device_ids,
		},
	},
	.xfer_input_ok	= sha_xfer_input_ok,
	.prep_xfer	= sha_prep_xfer,
	.ioctl_cm	= SHA_IOC_ACCESS,
	.arg_size	= sizeof(struct sha_access),
};

static int __init sha_init(void)
{
	return esp_driver_register(&sha_driver);
}

static void __exit sha_exit(void)
{
	esp_driver_unregister(&sha_driver);
}

module_init(sha_init)
module_exit(sha_exit)

MODULE_DEVICE_TABLE(of, sha_device_ids);

MODULE_AUTHOR("Emilio G. Cota <cota@braap.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sha driver");
