#include "syntax.hpp"

#include <stddef.h>
#include <stdint.h>
#include <libm/cstrTools.h>
#include <libm/memStuff.h>
#include <libm/heap/heap.h>
#include <libm/syscallManager.h>

bool look_forward_compare(int maxl, int cur, char* input, char* search) {
	int slen = StrLen(search);

	for (int i = 0; i < slen; i++) {
		if (cur + i > maxl) {
			return false;
		}
		if (input[cur + i] != search[i]) {
			return false;
		}
	}
	return true;
}

int highlight_section(char* input, int len, int cur, char* str, syntax_section_t* section, uint8_t* output) {
	if (!section->active) {
		return cur;
	}
	if (look_forward_compare(len, cur, input, &str[section->sect_start_offset])) {
		int start = cur;
		int end_len = StrLen(&str[section->sect_end_offset]);

		cur += StrLen(&str[section->sect_start_offset]);

		while (!look_forward_compare(len, cur, input, &str[section->sect_end_offset])) {
			if (cur > len) {
				break;
			}
			if (input[cur] == section->skip_next) {
				cur++;
			}
			cur++;
		}
		//printf("section %d - %d\n", start, cur);
		_memset(&output[start], section->color, (cur - start) + end_len);

		cur += end_len;
	}

	return cur;
}

bool contains_char(char* str, char c) {
	int len = StrLen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == c) {
			return true;
		}
	}
	return false;
}

uint8_t bracket_matching_colors[] = {
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan
};

#define isupper(c) (c >= 'A' && c <= 'Z')
#define islower(c) (c >= 'a' && c <= 'z')
#define isalpha(c) (islower(c) || isupper(c))

uint8_t* highlight(char* input, int len, syntax_header_t* syntax) {
	uint32_t str_offset = sizeof(syntax_header_t) + sizeof(syntax_word_t) * syntax->num_words;
	char* str = &((char*)syntax)[str_offset];
	syntax_word_t* words = (syntax_word_t*)&syntax[1];

	uint8_t* output = (uint8_t*) _Malloc(len);
	_memset(output, white, len);

	int curr_bracket_idx = 0;

	for (int i = 0; i < len; i++) {
		i = highlight_section(input, len, i, str, &syntax->string, output);
		i = highlight_section(input, len, i, str, &syntax->single_char, output);
		i = highlight_section(input, len, i, str, &syntax->single_line_comment, output);
		i = highlight_section(input, len, i, str, &syntax->multi_line_comment, output);

		if (i == 0 || (!isalpha(input[i - 1]) && input[i - 1] != '_')) {
			//i++;

			for (int j = 0; j < syntax->num_words; j++) {
				if (look_forward_compare(len, i, input, &str[words[j].word_offset])) {
					int word_len = StrLen(&str[words[j].word_offset]);

					if (isalpha(input[i + word_len]) || input[i + word_len] == '_') {
						continue;
					}

					//printf("word %d - %d\n", i, i + word_len);
				
					_memset(&output[i], words[j].color, word_len);
					i += word_len;
				}
			}
		}

		if (syntax->match_brackets) {
			if (contains_char(&str[syntax->brackets_start], input[i])) {
				curr_bracket_idx++;
				output[i] = bracket_matching_colors[curr_bracket_idx % sizeof(bracket_matching_colors)];
			}

			if (contains_char(&str[syntax->brackets_end], input[i])) {
				output[i] = bracket_matching_colors[curr_bracket_idx % sizeof(bracket_matching_colors)];
				curr_bracket_idx--;
			}
		}
	}

	return output;
}

syntax_header_t* load_syntax(const char* file) {
    syntax_header_t* syntax;
    uint64_t byteCount;
	if (!fsReadFile(file, (void**) &syntax, &byteCount)) {
		return NULL;
	}

	return syntax;
}