#include <confread.h>

int main(int argc, char **argv)
{

	struct confread_file *configFile;
	int i = 0;
	struct confread_section *thisSect = 0;
	int j = 0;
	struct confread_pair *thisPair = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
		return -1;
	}

	if (!(configFile = confread_open(argv[1]))) {
		fprintf(stderr, "Config open failed\n");
		return -1;
	}

	thisSect = configFile->sections;

	while (thisSect) {

		printf("[%s]\n", thisSect->name);

		thisPair = thisSect->pairs;

		while (thisPair) {

			printf("%s = %s\n", thisPair->key, thisPair->value);
			thisPair = thisPair->next;

		}

		printf("\n");
		thisSect = thisSect->next;

	}

	confread_close(&configFile);

	return 0;
}
