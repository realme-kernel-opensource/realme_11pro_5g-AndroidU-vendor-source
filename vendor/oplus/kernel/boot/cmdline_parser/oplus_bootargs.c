#include <linux/module.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/of_fdt.h>
#include <linux/of.h>

char *bootargs_ptr = NULL;
EXPORT_SYMBOL(bootargs_ptr);

#define BOOTARGS_LEN (4*1024) //4K
static int __init oplus_bootargs_init(void)
{
    int ret;
    struct device_node *of_chosen;

    bootargs_ptr = (char *)kmalloc(BOOTARGS_LEN, GFP_KERNEL);
    if (!bootargs_ptr) {
        printk(KERN_ERR "kmalloc for bootargs ptr failed !\n");
        return -1;
    }

    of_chosen = of_find_node_by_path("/chosen");
    if (!of_chosen) {
        of_chosen = of_find_node_by_path("/chosen@0");
    }

    if (of_chosen) {
        ret = of_property_read_string(of_chosen, "bootargs", (const char **)&bootargs_ptr);
        if (ret) {
            kfree(bootargs_ptr);
            bootargs_ptr = NULL;
            printk(KERN_ERR "try to get bootargs failed !");
            return -1;
        }
    }

    return 0;
}

void __exit oplus_bootargs_exit(void)
{
    if(bootargs_ptr != NULL) {
        kfree(bootargs_ptr);
        bootargs_ptr = NULL;
    }
    printk(KERN_INFO "oplus_bootargs exit!\n");
}

/*Tongyang.Xu@BSP.Stability add for ftm agingtest 2023/5/30 begin*/
/*ftmaging:true ; not ftmaging:false*/
bool check_ftmaging_from_bootargs(void)
{
    struct device_node *of_chosen;

    of_chosen = of_find_node_by_path("/chosen");
    if (!of_chosen) {
        of_chosen = of_find_node_by_path("/chosen@0");
    }

    if (of_chosen) {
        if(!bootargs_ptr) {
            printk(KERN_ERR "ftmaging try to get bootargs failed !\n");
        }
        else {
            if (strstr(bootargs_ptr, "oplus_ftm_mode=ftmaging")) {
                printk(KERN_ERR "ftmaging check success!\n");
                return true;
            }
            else {
                printk(KERN_ERR "not ftmaging!\n");
            }
        }
    }
    else {
        printk(KERN_ERR "failed to get /chosen!\n");
    }

    return false;
}
EXPORT_SYMBOL(check_ftmaging_from_bootargs);
/*Tongyang.Xu@BSP.Stability add for ftm agingtest 2023/5/30 end*/

module_init(oplus_bootargs_init);
module_exit(oplus_bootargs_exit);
MODULE_LICENSE("GPL v2");
