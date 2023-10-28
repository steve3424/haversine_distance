#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_PATH_ 1024

typedef struct {
	char path[MAX_PATH_];
	uint64_t path_len;
} Path;

void PrintPath(Path* path) {
	printf("(%lld) %s", path->path_len, path->path);
}

void PushPath(Path* path, char* str) {
	int i = 0;
	while(str[i]) {
		path->path[path->path_len++] = str[i++];
	}
	path->path[path->path_len] = '\0';
}

// NOTE: assumes 'C:\folder\folder\file.c' type path
void GetResourcesDir(Path* path) {
	// Pop file name
	char* this_file_path = __FILE__;
	int i = (int)strlen(this_file_path);
	while(i-- > 0 && this_file_path[i] != '\\') {}

	// Copy this file dir to path
	for(; path->path_len < i; ++path->path_len) {
		path->path[path->path_len] = this_file_path[path->path_len];
	}

	// Copy resources to buffer
	char* relative_path_to_resources_dir = "\\..\\local_resources";
	uint64_t dir_len = strlen(relative_path_to_resources_dir);
	for(i = 0; i < dir_len; ++i) {
		path->path[path->path_len++] = relative_path_to_resources_dir[i];
	}
	path->path[path->path_len] = '\0';
}

void WritePairs(int num_pairs, FILE* file) {
	double x0 = 180.0;
	double y0 = 179.123;
	double x1 = 178.124;
	double y1 = 177.125;
	char coord[128];

	for(int i = 0; i < num_pairs; ++i) {
		int buf_size = sizeof(coord);
		int offset = 0;
		if(i != 0) {
			coord[0] = ',';
			coord[1] = '\n';
			offset += 2;
		}
		// NOTE: assuming floats are at most 21 chars, the max this will write is 112 chars
		snprintf(coord + offset, buf_size - offset, "\t\t{\"x0\":%.16f, \"y0\":%.16f, \"x1\":%.16f, \"y1\":%.16f}", x0, y0, x1, y1);
		fwrite(coord, sizeof(char), strlen(coord), file);
	}
	fwrite("\n", sizeof(char), 1, file);
}

int main() {
	Path json_file_path;
	json_file_path.path_len = 0;
	GetResourcesDir(&json_file_path);
	PushPath(&json_file_path, "\\pairs.json");

	FILE* json_file = fopen(json_file_path.path, "w");
	char* json_out_start = "{\n\t\"pairs\": [\n";
	fwrite(json_out_start, sizeof(char), strlen(json_out_start), json_file);

	WritePairs(2, json_file);

	char* json_out_end = "\t]\n}";
	fwrite(json_out_end, sizeof(char), strlen(json_out_end), json_file);
	fclose(json_file);

	json_file = fopen(json_file_path.path, "r");
	char buffer[64] = {0};
	int chars_to_read = sizeof(buffer) - 1;
	int num_chars_read = 0;
	do {
		num_chars_read = (int)fread(buffer, sizeof(char), chars_to_read, json_file);
		buffer[num_chars_read] = '\0';
		printf("%s", buffer);
	} while(chars_to_read == num_chars_read);

	return 0;
}
