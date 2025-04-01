#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>


int main() {
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	unsigned char *fbp = 0;
	unsigned int x = 0, y = 0;
	long int location = 0;

	// Open the file for reading and writing
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		perror("Error: cannot open framebuffer device");
		exit(1);
	}
	//printf("The framebuffer device was opened successfully.\n");

	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("Error reading fixed information");
		exit(2);
	}

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable information");
		exit(3);
	}

	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	// Figure out the size of the screen in bytes
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);

	if (fbp == MAP_FAILED) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");
	struct timespec ts, delta;
	long start_time,end_time;
	clock_gettime(CLOCK_REALTIME, &ts);
	start_time = ts.tv_nsec;

	int i = 0;
	for(;;) {
		clock_gettime(CLOCK_REALTIME, &ts);
		start_time = ts.tv_nsec;
		// Figure out where in memory to put the pixel
		for (y = 0; y < vinfo.yres / 2; y++) {
			for (x = 0; x < vinfo.xres /2; x++) {
				location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
				*(fbp + location) = (int)((i) % 2 * 0xffff);        // Some blue
				*(fbp + location + 1) = 255;     // A little green
				*(fbp + location + 2) = 255;    // A lot of red
				*(fbp + location + 3) = 0;      // No transparency
		//		printf("%d\n", *(fbp+location));
			}
		}
		clock_gettime(CLOCK_REALTIME, &ts);
		end_time = ts.tv_nsec;
		delta.tv_nsec =  100000*1000 - end_time - start_time;
		nanosleep(&delta, NULL);
		printf("REDRAW i = %d\n", i++);
	}
		munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
