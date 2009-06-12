/* libmpdclient
   (c) 2003-2009 The Music Player Daemon Project
   This project's homepage is: http://www.musicpd.org

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Music Player Daemon nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <mpd/parser.h>
#include <mpd/protocol.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct mpd_parser {
#ifndef NDEBUG
	enum mpd_parser_result result;
#endif

	union {
		bool partial;

		struct {
			enum mpd_ack ack;
			int at;
			const char *message;
		} error;

		struct {
			const char *name, *value;
		} pair;
	} u;
};

struct mpd_parser *
mpd_parser_new(void)
{
	struct mpd_parser *parser = malloc(sizeof(*parser));
	if (parser == NULL)
		return NULL;

#ifndef NDEBUG
	parser->result = MPD_PARSER_MALFORMED;
#endif

	return parser;
}

void
mpd_parser_free(struct mpd_parser *parser)
{
	free(parser);
}

static inline enum mpd_parser_result
set_result(struct mpd_parser *parser, enum mpd_parser_result result)
{
#ifndef NDEBUG
	/* this value exists only in the debug build, and is used by
	   assertions in the "get" functions below */
	parser->result = result;
#else
	/* suppress "unused" warning */
	(void)parser;
#endif

	return result;
}

enum mpd_parser_result
mpd_parser_feed(struct mpd_parser *parser, char *line)
{
	if (strcmp(line, "OK") == 0) {
		parser->u.partial = false;
		return set_result(parser, MPD_PARSER_SUCCESS);
	} else if (strcmp(line, "list_OK") == 0) {
		parser->u.partial = true;
		return set_result(parser, MPD_PARSER_SUCCESS);
	} else if (memcmp(line, "ACK", 3) == 0) {
		char *p, *q;

		parser->u.error.ack = MPD_ACK_ERROR_UNK;
		parser->u.error.at = -1;
		parser->u.error.message = NULL;

		/* parse [ACK@AT] */

		p = strchr(line + 3, '[');
		if (p == NULL)
			return set_result(parser, MPD_PARSER_ERROR);

		parser->u.error.ack = strtol(p + 1, &p, 10);
		if (*p == '@')
			parser->u.error.at = strtol(p + 1, &p, 10);

		q = strchr(p, ']');
		if (q == NULL)
			return set_result(parser, MPD_PARSER_MALFORMED);

		/* skip the {COMMAND} */

		p = q + 1;
		q = strchr(p, '{');
		if (q != NULL) {
			q = strchr(p, '}');
			if (q != NULL)
				p = q + 1;
		}

		/* obtain error message */

		while (*p == ' ')
			++p;

		if (*p != 0)
			parser->u.error.message = p;

		return set_result(parser, MPD_PARSER_ERROR);
	} else {
		/* so this must be a name-value pair */

		char *p;

		p = strchr(line, ':');
		if (p == NULL || p[1] != ' ')
			return set_result(parser, MPD_PARSER_MALFORMED);

		*p = 0;

		parser->u.pair.name = line;
		parser->u.pair.value = p + 2;

		return set_result(parser, MPD_PARSER_PAIR);
	}
}

bool
mpd_parser_is_partial(const struct mpd_parser *parser)
{
	assert(parser->result == MPD_PARSER_SUCCESS);

	return parser->u.partial;
}

enum mpd_ack
mpd_parser_get_ack(const struct mpd_parser *parser)
{
	assert(parser->result == MPD_PARSER_ERROR);

	return parser->u.error.ack;
}

int
mpd_parser_get_at(const struct mpd_parser *parser)
{
	assert(parser->result == MPD_PARSER_ERROR);

	return parser->u.error.at;
}

const char *
mpd_parser_get_message(const struct mpd_parser *parser)
{
	assert(parser->result == MPD_PARSER_ERROR);

	return parser->u.error.message;
}

const char *
mpd_parser_get_name(const struct mpd_parser *parser)
{
	assert(parser->result == MPD_PARSER_PAIR);

	return parser->u.pair.name;
}

const char *
mpd_parser_get_value(const struct mpd_parser *parser)
{
	assert(parser->result == MPD_PARSER_PAIR);

	return parser->u.pair.value;
}