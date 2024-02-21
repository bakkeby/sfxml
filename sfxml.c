/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX(A,B) ((A) > (B) ? (A) : (B))
#define TAGS(A,B) (A << 4 | B)

enum {
	NONE,
	OPENING,
	CLOSING,
	EMPTY
} tag;

void
usage(void)
{
	fprintf(stderr,
		"Usage: sfxml                                                                  \n"
		"                                                                              \n"
		"Reads XML data from stdin, formats and indents it and writes the result to    \n"
		"stdout.                                                                       \n"
		"                                                                              \n"
		"Contrary to generally recommended tools like xmllint --format this program    \n"
		"does not perform any form of validation to ensure that the input is valid XML.\n"
		"Formatting is done on a best effort basis following basic rules.              \n"
		"                                                                              \n"
		"The benefit of this is that the output will be consistent even for incomplete \n"
		"data and the tool will not refuse to do the job if it doesn't understand it.  \n"
		"                                                                              \n"
		"This is intended as simple tool to make condensed (e.g. single line) XML like \n"
		"data structures more readable by adding line breaks and indentation after     \n"
		"opening and closing tags while ignoring escaped characters as well as leaving \n"
		"text encapsulated in double or single quotes as-is.                           \n"
		"                                                                              \n"
		"Example usage:                                                                \n"
		"   $ sfxml < file.xml                                                         \n"
		"   $ echo \"<html><head/><body><div>A</div><div>B</div></body>\" | sfxml      \n"
		"\n"
	);
	exit(0);
}

void
printindent(int level)
{
	int l;
	char indent = 0x09; // tab character
	for (l = 0; l < level; ++l) {
		putchar(indent);
	}
}

void
format_xml(void)
{
	int i;
	int level = 0;
	int escape_next = 0;
	int inside_double_quote = 0;
	int inside_single_quote = 0;
	int prev_was_left_angle_bracket = 0;
	int prev_was_right_angle_bracket = 0;
	int prev_was_forward_slash = 0;
	int prev_tag_type = NONE;
	int next_tag_type = NONE;
	int skip_next_spaces = 1;
	int buffer_size = 100;
	char buffer[buffer_size];

	/* The below code should add newlines and indentation
	 * according to the following rules.
	 *
	 * o----------------------------------------o
	 * | Tag combination      | newline | level |
	 * |----------------------|---------|-------|
	 * | <opening><opening>   |    Y    |  ++   |
	 * | <opening>0</closing> |    N    |       |
	 * | </closing><opening>  |    Y    |       |
	 * | </closing></closing> |    Y    |  --   |
	 * | <empty/><opening>    |    Y    |       |
	 * | <empty/></closing>   |    Y    |  --   |
	 * o----------------------------------------o
     */
	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		for (i = 0; i < buffer_size; i++) {
			if (buffer[i] == 0) {
				break;
			}

			if (escape_next) {
				escape_next = 0;
				goto next;
			}

			if (skip_next_spaces) {
				if (buffer[i] == 0x20 || buffer[i] == 0x09) {
					continue;
				}
				skip_next_spaces = 0;
			}

			if (buffer[i] == '\\') {
				escape_next = 1;
				goto next;
			}

			if (inside_double_quote && buffer[i] != '"') {
				goto next;
			}

			if (inside_single_quote && buffer[i] != '\'') {
				goto next;
			}

			if (buffer[i] == '\n') {
				skip_next_spaces = 1;
				continue;
			}

			if (buffer[i] == '"') {
				if (!inside_single_quote && prev_tag_type) {
					inside_double_quote = !inside_double_quote;
				}
				goto next;
			}

			if (buffer[i] == '\'') {
				if (!inside_double_quote && prev_tag_type) {
					inside_single_quote = !inside_single_quote;
				}
				goto next;
			}

			if (buffer[i] == '<') {
				prev_was_left_angle_bracket = 1;
				next_tag_type = OPENING;
				continue;
			}

			if (buffer[i] == '>') {
				prev_was_right_angle_bracket = 1;
				if (prev_was_forward_slash) {
					prev_tag_type = EMPTY;
				}
				prev_was_forward_slash = 0;
				goto next;
			}

			if (prev_was_right_angle_bracket) {
				if (buffer[i] == '/') {
					next_tag_type = CLOSING;
				}
				if (prev_was_left_angle_bracket) {
					switch TAGS(prev_tag_type,next_tag_type) {
					case TAGS(OPENING,OPENING):
						putchar('\n');
						printindent(++level);
						break;
					case TAGS(OPENING,CLOSING):
						break;
					case TAGS(CLOSING,OPENING):
						putchar('\n');
						printindent(level);
						break;
					case TAGS(CLOSING,CLOSING):
						putchar('\n');
						level = MAX(level - 1, 0);
						printindent(level);
						break;
					case TAGS(EMPTY,OPENING):
						putchar('\n');
						printindent(level);
						break;
					case TAGS(EMPTY,CLOSING):
						putchar('\n');
						level = MAX(level - 1, 0);
						printindent(level);
						break;
					default:
						break;
					}
					putchar('<');
				}

				prev_was_left_angle_bracket = 0;
				prev_was_right_angle_bracket = 0;
				prev_tag_type = next_tag_type;
				next_tag_type = NONE;
				goto next;
			}

			if (prev_was_left_angle_bracket) {
				if (buffer[i] == '/') {
					next_tag_type = CLOSING;
				}

				/* Case: <opening>0</closing> - do nothing */
				putchar('<');
				prev_was_left_angle_bracket = 0;
				prev_tag_type = next_tag_type;
				next_tag_type = NONE;
				goto next;
			}

			if (buffer[i] == '/') {
				prev_was_forward_slash = 1;
				goto next;
			}

			prev_was_forward_slash = 0;
			prev_was_left_angle_bracket = 0;
			prev_was_right_angle_bracket = 0;
next:
			putchar(buffer[i]);
 		}
	}
}

int
main(int argc, char *argv[])
{
	if (isatty(STDIN_FILENO)) {
		usage();
	}

	format_xml();
	exit(0);
}
