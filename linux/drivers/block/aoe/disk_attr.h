/* This struct is not defined in kernels 2.6.10 and earlier.
 * It is defined here for compatibility with those kernels.
 */

struct disk_attribute {
	struct attribute attr;
	ssize_t (*show)(struct gendisk *, char *);
};
