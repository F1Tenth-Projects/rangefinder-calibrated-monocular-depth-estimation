#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <laserarray.h>
#include <laserarray-common/constants.h>

#define ACTION_ENABLE   1
#define ACTION_DISABLE  2
#define ACTION_ECHO     3

struct parameters {
	int action;
	int sensor_id;
	const char *devfile;
	float echo_period;
};

static void parse_cli(int argc, char **argv, struct parameters *params);
static void display_detections(laserarray *dev);
static void print_usage_and_die(void);

const char *usage =
"Usage: laserarrayctl [OPTIONS] enable SENSOR_ID\n"
"   or: laserarrayctl [OPTIONS] disable SENSOR_ID\n"
"   or: laserarrayctl [OPTIONS] echo\n"
"\n"
"  -d, --device=FILE   use the device at DEVPATH (default: /dev/laserarray)\n"
"  -p, --period=SEC    set the echo update period to be every SEC seconds\n"
"      --help          print this help message\n";

int main(int argc, char **argv)
{
	int rc;
	struct parameters cli;
	laserarray *dev;

	parse_cli(argc, argv, &cli);

	if ((rc = laserarray_open(cli.devfile, &dev)) != 0) {
		fprintf(stderr, "error: unable to open device: %s (%s)\n",
		        laserarray_strerror(rc), laserarray_error_name(rc));
		exit(1);
	}

	switch (cli.action) {
	case ACTION_ENABLE:
		if ((rc = laserarray_enable_sensor(dev, cli.sensor_id)) != 0) {
			fprintf(stderr,
			        "error: could not enable sensor %i: %s (%s)\n",
				cli.sensor_id, laserarray_strerror(rc),
				laserarray_error_name(rc));
			exit(1);
		}
		break;
	case ACTION_DISABLE:
		if ((rc = laserarray_disable_sensor(dev, cli.sensor_id)) != 0) {
			fprintf(stderr,
			        "error: could not disable sensor %i: %s (%s)\n",
				cli.sensor_id, laserarray_strerror(rc),
				laserarray_error_name(rc));
			exit(1);
		}
		break;
	case ACTION_ECHO:
		while (1) {
			display_detections(dev);
			usleep((unsigned long) (cli.echo_period * 1e6));
		}
		break;
	default:
		break;
	}

	laserarray_close(dev);

	return 0;
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
			printf("\n");
			fflush(stdout);
		}
	}


}

static void parse_cli(int argc, char **argv, struct parameters *params)
{
	int c;
	const char *cmd;
	const struct option opts[] = {
		{"device", required_argument, NULL, 'd'},
		{"period", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	params->devfile = "/dev/laserarray";
	params->echo_period = 0.100;
	while ((c = getopt_long(argc, argv, "d:p:", opts, NULL)) != -1) {
		switch (c) {
		case 'd':
			params->devfile = optarg;
			break;
		case 'p':
			params->echo_period = atof(optarg);
			break;
		case 'h':
			printf("%s", usage);
			exit(0);
			break;
		default:
			exit(1);
			break;
		}
	};

	if (argc - optind == 0) {
		print_usage_and_die();
	}

	cmd = argv[optind];
	if (strcmp(cmd, "enable") == 0) {
		params->action = ACTION_ENABLE;
		if (argc - optind == 2) {
			params->sensor_id = atoi(argv[optind + 1]);
		} else {
			print_usage_and_die();
		}
	} else if (strcmp(cmd, "disable") == 0) {
		params->action = ACTION_DISABLE;
		if (argc - optind == 2) {
			params->sensor_id = atoi(argv[optind + 1]);
		} else {
			print_usage_and_die();
		}
	} else if (strcmp(cmd, "echo") == 0) {
		params->action = ACTION_ECHO;
	} else {
		print_usage_and_die();
	}
}

static void print_usage_and_die(void)
{
	fprintf(stderr, "%s", usage);
	exit(1);
}
