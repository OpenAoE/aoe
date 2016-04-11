#include <linux/bio.h>

void try_using_bio_endio(struct bio *bio)
{
	// only older kernels have a second parameter
	bio_endio(bio);
}
