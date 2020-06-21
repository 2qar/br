#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TMP_PATH "/tmp/br"
#define MAX_FILENAME_LEN 1000

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "usage: br [FILE]...\n");
		exit(1);
	}

	FILE *f = fopen(TMP_PATH, "w");
	for (int i = 1; i < argc; ++i) {
		fprintf(f, "%s\n", argv[i]);
	}
	fclose(f);

	char *editor;
	if (!(editor = getenv("EDITOR")))
		editor = "vi";

	char cmd[100];
	sprintf(cmd, "%s %s", editor, TMP_PATH);
	system(cmd);

	f = fopen(TMP_PATH, "r");
	int n = 0;
	char tmp[MAX_FILENAME_LEN];
	while (fgets(tmp, MAX_FILENAME_LEN, f))
		++n;
	if (n != argc -1) {
		fprintf(stderr, "br: you added or deleted lines. dont do that\n");
		fclose(f);
		exit(1);
	}

	rewind(f);

	int i = 1;
	char *name = malloc(sizeof(char) * MAX_FILENAME_LEN);
	size_t max_len = MAX_FILENAME_LEN;
	while ((n = getline(&name, &max_len, f)) > 0) {
		name[n-1] = 0;
		if (strncmp(argv[i], name, MAX_FILENAME_LEN) != 0) {
			printf("%s -> %s\n", argv[i], name);
			if (rename(argv[i], name) < 0) {
				perror("rename");
			}
		}
		i += 1;
	}
	free(name);

	remove(TMP_PATH);
}
