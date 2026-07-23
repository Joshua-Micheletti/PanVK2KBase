/*
 * kbase_test.c - minimal standalone probe for /dev/mali0 (kbase).
 *
 * Goal: confirm you can open the device, negotiate a UK version, and read
 * back GPU properties, WITHOUT any Mesa involved. Get this printing sane
 * numbers for your G615-MC2 before writing a single line of pan_kmod code.
 *
 * Build:
 *   gcc -O0 -g -Wall -o kbase_test kbase_test.c
 *
 * Run (inside your proot, as whatever user can open /dev/mali0):
 *   ./kbase_test
 *
 * IMPORTANT: swap mali_kbase_ioctl_skeleton.h for your device's real
 * extracted header before trusting any of this output. Command numbers,
 * struct layouts and even the version-check semantics can differ for
 * 44.10 vs. what's sketched here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include "mali_kbase_ioctl_skeleton.h"

#define MALI_DEVICE_PATH "/dev/mali0"

static void dump_hex(const unsigned char *buf, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		printf("%02x ", buf[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
	if (len % 16)
		printf("\n");
}

int main(void)
{
	int fd;
	int ret;

	printf("== kbase probe ==\n");
	printf("opening %s\n", MALI_DEVICE_PATH);

	fd = open(MALI_DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open failed: %s\n", strerror(errno));
		fprintf(stderr,
			"If this is EACCES/ENOENT: check `ls -l %s` from "
			"outside proot too - this is the first real blocker, "
			"solve it before anything else.\n", MALI_DEVICE_PATH);
		return 1;
	}
	printf("open OK, fd=%d\n", fd);

	/* Step 1: version check. This tells kbase (and you) which UK
	 * interface generation you're speaking. Expect to see 44.10 echoed
	 * back, or kbase will reject everything after this. */
	struct kbase_ioctl_version_check ver = {
		.major = 1, /* VERIFY: some kbase versions want you to send
			     * the highest version you support and it replies
			     * with what it actually supports; others want 0.
			     * Check your header's comment on this ioctl. */
		.minor = 0,
	};

	ret = ioctl(fd, KBASE_IOCTL_VERSION_CHECK, &ver);
	if (ret < 0) {
		fprintf(stderr, "VERSION_CHECK failed: %s (errno %d)\n",
			strerror(errno), errno);
		fprintf(stderr,
			"A failure here almost always means either the ioctl "
			"number or the struct layout doesn't match this "
			"kernel's actual kbase build. Re-check against the "
			"real header.\n");
		close(fd);
		return 1;
	}
	printf("VERSION_CHECK OK: major=%u minor=%u\n", ver.major, ver.minor);
	printf("  (expected 44.10 for your target - if this doesn't match, "
	       "stop and figure out why before continuing)\n");

	/* Step 2: set flags. Establishes context type. */
	struct kbase_ioctl_set_flags flags = {
		.create_flags = 0, /* VERIFY: correct flag value for your version */
	};
	ret = ioctl(fd, KBASE_IOCTL_SET_FLAGS, &flags);
	if (ret < 0) {
		fprintf(stderr, "SET_FLAGS failed: %s (errno %d)\n",
			strerror(errno), errno);
		close(fd);
		return 1;
	}
	printf("SET_FLAGS OK\n");

	/* Step 3: query GPU properties. Two-step: ask for size (buffer=NULL,
	 * size=0 -> kbase returns required size as the return value or in
	 * .size depending on version), then allocate and fetch. */
	struct kbase_ioctl_get_gpuprops probe = {
		.buffer = 0,
		.size = 0,
		.flags = 0,
	};
	ret = ioctl(fd, KBASE_IOCTL_GET_GPUPROPS, &probe);
	if (ret < 0) {
		fprintf(stderr, "GET_GPUPROPS (size probe) failed: %s\n",
			strerror(errno));
		close(fd);
		return 1;
	}

	/* Some kbase versions return the size as the ioctl's return value,
	 * others fill probe.size - VERIFY which applies here. Trying the
	 * return-value convention first: */
	size_t props_size = (size_t)ret;
	if (props_size == 0)
		props_size = probe.size;

	if (props_size == 0) {
		fprintf(stderr,
			"couldn't determine gpuprops size from either "
			"convention - check your header for this version's "
			"semantics\n");
		close(fd);
		return 1;
	}

	printf("GET_GPUPROPS reports size=%zu bytes\n", props_size);

	unsigned char *props_buf = calloc(1, props_size);
	if (!props_buf) {
		perror("calloc");
		close(fd);
		return 1;
	}

	struct kbase_ioctl_get_gpuprops fetch = {
		.buffer = (__u64)(uintptr_t)props_buf,
		.size = (__u32)props_size,
		.flags = 0,
	};
	ret = ioctl(fd, KBASE_IOCTL_GET_GPUPROPS, &fetch);
	if (ret < 0) {
		fprintf(stderr, "GET_GPUPROPS (fetch) failed: %s\n",
			strerror(errno));
		free(props_buf);
		close(fd);
		return 1;
	}

	printf("GET_GPUPROPS fetch OK, raw bytes:\n");
	dump_hex(props_buf, props_size);
	printf(
		"\nThese are raw serialized gpu_raw_gpu_props/gpu_props "
		"records - decoding them into GPU ID, shader core mask, L2 "
		"cache size etc. needs the property-record struct from your "
		"real header (mali_base_kernel.h / mali_kbase_gpuprops.h). "
		"For a first pass, just confirm the byte pattern looks "
		"non-garbage and consistent across runs - that alone tells "
		"you the ioctl round-trip is working.\n");

	free(props_buf);
	close(fd);

	printf("\n== probe complete, device is talking ==\n");
	return 0;
}