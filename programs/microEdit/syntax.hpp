#pragma once

#include <stdint.h>

enum colors {
	black,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
};

typedef struct syntax_section {
	bool active;
	uint32_t sect_start_offset;
	uint32_t sect_end_offset;
	uint8_t color;
	char skip_next;
} syntax_section_t;

typedef struct syntax_word {
	uint32_t word_offset;
	uint8_t color;
} syntax_word_t;


typedef struct syntax_header {
	uint32_t magic;


	syntax_section_t single_line_comment;
	syntax_section_t multi_line_comment;

	syntax_section_t string;
	syntax_section_t single_char;

	uint32_t brackets_start;
	uint32_t brackets_end;
	bool match_brackets;


	uint32_t num_words;
	syntax_word_t words[];
} syntax_header_t;

syntax_header_t* load_syntax(const char* file);
uint8_t* highlight(char* input, int len, syntax_header_t* syntax);