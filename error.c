#include "error.h"
#include "parser.h"

void error(int error_num) {
	fprintf(output, "Error: %s", errors[error_num]);
	fclose(input);
	fclose(output);
	exit(3434);
}
