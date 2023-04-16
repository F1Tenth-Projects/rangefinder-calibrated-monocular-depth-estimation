#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <laserarray.h>
#include <laserarray-common/constants.h>

static void display_detections(laserarray *dev);

const char *usage =
"Usage: laserarrayctl enable DEVPATH SENSOR_ID\n"
"Usage: laserarrayctl disable DEVPATH SENSOR_ID\n"
"Usage: laserarrayctl echo DEVPATH\n";

int main(int argc, char **argv)
{
	int rc;
	laserarray *dev;

	if (argc < 3) {
		goto print_usage_and_die;
	}

	if ((rc = laserarray_open(argv[2], &dev)) != 0) {
		fprintf(stderr, "Unable to open device: %s (%s)\n",
		        laserarray_strerror(rc), laserarray_error_name(rc));
		exit(1);
	}

	if (strcmp(argv[1], "enable") == 0) {
		int id;
		if (argc < 4) {
			goto close_print_usage_and_die;
		}
		id = atoi(argv[3]);
		if ((rc = laserarray_enable_sensor(dev, id)) != 0) {
			fprintf(stderr,
			        "Unable to enable sensor %i: %s (%s)\n",
				id, laserarray_strerror(rc),
				laserarray_error_name(rc));
			exit(1);
		}
	} else if (strcmp(argv[1], "disable") == 0) {
		int id;
		if (argc < 4) {
			goto close_print_usage_and_die;
		}
		id = atoi(argv[3]);
		if ((rc = laserarray_disable_sensor(dev, id)) != 0) {
			fprintf(stderr,
			        "Unable to disable sensor %i: %s (%s)\n",
				id, laserarray_strerror(rc),
				laserarray_error_name(rc));
			exit(1);
		}
	} else if (strcmp(argv[1], "echo") == 0) {
		while (1) {
			display_detections(dev);
			usleep(100000);
		}
	}


	laserarray_close(dev);

	return 0;

close_print_usage_and_die:
	laserarray_close(dev);
print_usage_and_die:
	fprintf(stderr, "%s", usage);
	exit(1);
}

static void display_detections(laserarray *dev)
{
	int i;

	for (i = 0; i < LASERARRAY_NUM_SENSORS; i++) {
		int rc;
		uint16_t ranges[LASERARRAY_MAX_DETECTIONS];
		int64_t timestamp;

		rc = laserarray_get_detections(dev, i, ranges, &timestamp);
		if (rc < 0) {
			if (rc == -LASERARRAY_ENODATA) {
				continue;
			} else {
				fprintf(stderr,
				      "Unable to get detections %i: %s (%s)\n",
				       0, laserarray_strerror(rc),
				       laserarray_error_name(rc));
			}
		} else {
			int64_t sec;
			int64_t usec;

			sec = timestamp / 1000000;
			usec = timestamp  - sec * 1000000;

			printf("% 10li.%06li: Sensor %i:", sec, usec, i);
			for (int i = 0; i < rc; i++) {
				printf(" % 6i", ranges[i]);
			}
			puts("");
			fflush(stdout);
		}
	}


}
