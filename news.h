// Routines for interacting with a news server
// Copyright © 1998 Peter Karlsson
//
// $Id$
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef __NEWS_H
#define __NEWS_H
#include <String.h>

// Port number
#define NNTP 119

class CNewsServer
{
public:
	// Constructor and destructor
	CNewsServer(String servername);
	~CNewsServer();

	// Method for checking numer of unread (total)
	int CheckLastRead(String newsrcfile);

protected:
	// Internal methods
	int NntpResponse(void);

	// Internal structure used
	struct lastread_t
	{
		String				group;
		int					lastread;
		struct lastread_t	*next;
	};

	// Internal data
	String		servername;
	int			socket;
	bool		used;
	lastread_t	*list_p;
};
#endif
