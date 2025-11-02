#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD "/usr/bin/fd"
#define MAX_LINES 100000
#define BUFFER_SIZE 1024

char **getoutput(char *option, char *target, char *path, int *line_count) {
  // Allocate memory for the result array
  char **result = malloc(MAX_LINES * sizeof(char *));
  if (!result) {
    return NULL;
  }

  // Build the command string safely
  char full_cmd[BUFFER_SIZE];
  snprintf(full_cmd, sizeof(full_cmd), "%s", CMD);

  if (option && strlen(option) > 0) {
    strncat(full_cmd, " ", sizeof(full_cmd) - strlen(full_cmd) - 1);
    strncat(full_cmd, option, sizeof(full_cmd) - strlen(full_cmd) - 1);
  }

  // If target is empty, use "." to match all files
  if (target && strlen(target) > 0) {
    strncat(full_cmd, " ", sizeof(full_cmd) - strlen(full_cmd) - 1);
    strncat(full_cmd, target, sizeof(full_cmd) - strlen(full_cmd) - 1);
  } else {
    strncat(full_cmd, " .", sizeof(full_cmd) - strlen(full_cmd) - 1);
  }

  strncat(full_cmd, " ", sizeof(full_cmd) - strlen(full_cmd) - 1);
  strncat(full_cmd, path, sizeof(full_cmd) - strlen(full_cmd) - 1);

  // Execute the command
  FILE *pipe = popen(full_cmd, "r");
  if (!pipe) {
    free(result);
    return NULL;
  }

  // Read output line by line
  int count = 0;
  char buffer[BUFFER_SIZE];

  while (fgets(buffer, sizeof(buffer), pipe) && count < MAX_LINES) {
    // Remove newline character if present
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    // Allocate memory for the line and copy it
    result[count] = malloc(strlen(buffer) + 1);
    if (!result[count]) {
      // Cleanup on allocation failure
      for (int i = 0; i < count; i++) {
        free(result[i]);
      }
      free(result);
      pclose(pipe);
      return NULL;
    }
    strcpy(result[count], buffer);
    count++;
  }

  pclose(pipe);
  *line_count = count;
  return result;
}

void free_output(char **output, int line_count) {
  if (output) {
    for (int i = 0; i < line_count; i++) {
      free(output[i]);
    }
    free(output);
  }
}

int main(int argc, char *argv[]) {
  int line_count = 0;
  char **output = getoutput("", "", "/home/sanae", &line_count);

  if (!output) {
    fprintf(stderr, "Failed to execute command or allocate memory\n");
    return EXIT_FAILURE;
  }

  // Print all output lines
  for (int i = 0; i < line_count; i++) {
    printf("%s\n", output[i]);
  }

  // Free allocated memory
  free_output(output, line_count);

  return EXIT_SUCCESS;
}
