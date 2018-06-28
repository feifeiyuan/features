#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/device.h>

static struct kobject *k_obj = NULL;
static char node0_buffer[20] = {0};
static char node1_buffer[20] = {0};
static char node2_buffer[20] = {0};
static char node3_buffer[20] = {0};

static ssize_t node0_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char * buf){
    return sprintf(buf, "%s", node0_buffer);
}

static ssize_t node0_buffer_store(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t count){
    sprintf(node0_buffer, "%s", buf);
    return count;
}

static ssize_t node1_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char * buf){
    return sprintf(buf, "%s", node1_buffer);
}

static ssize_t node1_buffer_store(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t count){
    sprintf(node1_buffer, "%s", buf);
    return count;
}

static ssize_t node2_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char * buf){
    return sprintf(buf, "%s", node2_buffer);
}

static ssize_t node2_buffer_store(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t count){
    sprintf(node2_buffer, "%s", buf);
    return count;
}

static ssize_t node3_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char * buf){
    return sprintf(buf, "%s", node3_buffer);
}

static ssize_t node3_buffer_store(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t count){
    sprintf(node3_buffer, "%s", buf);
    return count;
}

static struct kobj_attribute  node0_buffer_attribute=__ATTR(node0_buffer, S_IWUSR|S_IRUGO, node0_buffer_show, node0_buffer_store);
static struct kobj_attribute  node1_buffer_attribute=__ATTR(node1_buffer, S_IWUSR|S_IRUGO, node1_buffer_show, node1_buffer_store);
static struct kobj_attribute  node2_buffer_attribute=__ATTR(node2_buffer, S_IWUSR|S_IRUGO, node2_buffer_show, node2_buffer_store);
static struct kobj_attribute  node3_buffer_attribute=__ATTR(node3_buffer, S_IWUSR|S_IRUGO, node3_buffer_show, node3_buffer_store);

static struct attribute *node_attributes[] = {
    &node0_buffer_attribute.attr,
    &node1_buffer_attribute.attr,
    &node2_buffer_attribute.attr,
    &node3_buffer_attribute.attr,
    NULL
};

static const struct attribute_group node_attr_group = {
    .attrs = node_attributes,
};   

static int __init node_init(void){
    if ((k_obj = kobject_create_and_add("my_node", NULL)) == NULL ) {
        printk("my_node sys node create error \n");
        goto out;
    }

    if(sysfs_create_group(k_obj, &node_attr_group) ) {
        printk("sysfs_create_group failed\n");
        goto out2;
    }

    return 0;
out2:
    kobject_put(k_obj);
out:
    return -1;
}

static void __exit node_exit(void){
    if (k_obj) {
        sysfs_remove_group(k_obj, &node_attr_group);
        kobject_put(k_obj);
    }
    return;
}

module_init(cpu_power_init);
module_exit(cpu_power_exit);

MODULE_LICENSE("GPL");
