/* libmpdclient
   (c) 2003-2008 The Music Player Daemon Project
   This project's homepage is: http://www.musicpd.org

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

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

#ifndef MPD_INTERNAL_H
#define MPD_INTERNAL_H

#include "ierror.h"
#include <mpd/connection.h>
#include <mpd/status.h>
#include "socket.h"

#include <sys/select.h>

/* mpd_connection
 * holds info about connection to mpd
 * use error, and errorStr to detect errors
 */
struct mpd_connection {
	/* use this to check the version of mpd */
	unsigned version[3];

	struct mpd_error_info error;

	/* DON'T TOUCH any of the rest of this stuff */

	struct mpd_async *async;
	struct timeval timeout;

	struct mpd_parser *parser;

	/**
	 * Are we currently receiving the response of a command?
	 */
	bool receiving;

	int listOks;
	int doneListOk;
	int commandList;
	struct mpd_pair *pair;
	char *request;
};

extern const char *const mpdTagItemKeys[];

struct mpd_stats {
	int number_of_artists;
	int number_of_albums;
	int number_of_songs;
	unsigned long uptime;
	unsigned long db_update_time;
	unsigned long play_time;
	unsigned long db_play_time;
};

struct mpd_search_stats {
	int number_of_songs;
	unsigned long play_time;
};

/**
 * Copies the error state from connection->sync to connection->error.
 */
void
mpd_connection_sync_error(struct mpd_connection *connection);

#endif
