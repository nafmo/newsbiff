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

#include "news.h"
#include "tcp.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static const char *s_modereader = "MODE READER\n";
static const char *s_list = "LIST\n";
static const char *s_quit = "QUIT\n";

// Constructor -- connect to a news server
CNewsServer::CNewsServer(string servername)
{
	socket = ConnectTCP(servername.c_str(), NNTP);
	used = false;
	list_p = NULL;
	
	if (-1 != socket)
	{
		if (200 != NntpResponse())
		{
			DisconnectTCP(socket);
			socket = -1;
		}

		write(socket, s_modereader, strlen(s_modereader));

		if (200 != NntpResponse())
		{
			DisconnectTCP(socket);
			socket = -1;
		}
	}
}

// Destructor -- disconnect
CNewsServer::~CNewsServer()
{
	if (-1 != socket)
	{
		write(socket, s_quit, strlen(s_quit));
		DisconnectTCP(socket);
	}
	
	if (used)
	{
		lastread_t *p, *trav_p = list_p;
		while (trav_p)
		{
	   		p = trav_p->next;
			delete trav_p;
			trav_p = p;
		}
	}
}

// Check number of unread (total)
unsigned int CNewsServer::CheckLastRead(string newsrcfile)
{
	if (-1 == socket) return 0; // Socket not open
	if (used) return totalunread; // We already have the total

	used = true;
	
	// Load the newsrc file
	lastread_t	*trav_p = NULL;
	FILE		*newsrc;
	char		data[1024], *p1, *p2;

	newsrc = fopen(newsrcfile.c_str(), "r");
	if (NULL == newsrc) return 0; // Unable to open newsrc file
	
	while (NULL != fgets(data, sizeof(data), newsrc))
	{
		data[sizeof(data) - 1] = 0; // Be sure to zero terminate string
		
		if (NULL != (p1 = strchr(data, ':')) && *(p1 + 1))
		{
			// This is an active entry
			// The highest number should be on the right
			p2 = data + strlen(data) - 2;
			while (p2 >= data && isdigit(*p2)) p2 --;
			
			if (p2 != data) // If we got to the start, something is wrong
			{
				// Move one step to the right (beginning of numeral)
				p2 ++;
				// Zero terminate newsgroup name
				*p1 = 0;
				// Allocate and fill in lastread entry
				lastread_t	*this_p = new lastread_t;
				this_p->group = data;
				this_p->lastread = atoi(p2);
				this_p->topnumber = 0;
				this_p->next = NULL;
				if (trav_p) trav_p->next = this_p;
				if (!list_p) list_p = this_p;
				trav_p = this_p;
			}
		}
	}
	fclose(newsrc);
	
	// Retrieve a newsgroup list from server
	write(socket, s_list, strlen(s_list));
	
	if (215 != NntpResponse())
	{
		return 0;
	}
	
	char buf[1024];
	int retries = 0;

	unsigned int	unread = 0, len;

	// Retrieve newsgruop list
	while ((len = ReadLine(socket, buf, sizeof(buf))) != 2) // Len2=end
	{
		// The returned data from the news server is on the form:
		// NGNAME HIGHEST NUMBER STATUS
		// str    int     int    char
		char *p1, *p2;
		p1 = strchr(buf, ' ');
		p2 = strchr(p1 + 1, ' ');
		if (p1 && p2)
		{
			*p1 = 0;
			*p2 = 0;
			int topnumber = atoi(p1 + 1);
			
			// Locate in our list of newsgroups
			trav_p = list_p;
			while (NULL != trav_p)
			{
				if (trav_p->group == buf)
				{
					// Match
					if (trav_p->lastread && topnumber > trav_p->lastread)
						unread += topnumber - trav_p->lastread;
					trav_p->topnumber = topnumber;
					trav_p = NULL;
				}
				else
				{
					trav_p = trav_p->next;
				}
			}
		}
	}

	totalunread = unread;
	return unread;
}

// Check the number of unread articles in a group
unsigned int CNewsServer::QueryUnread(string groupname)
{
	if (!used) return 0; // We do not know the number of unread

	// Locate in our list of newsgroups
	lastread_t	*trav_p = list_p;
	while (NULL != trav_p)
	{
		if (trav_p->group == groupname)
		{
			// Match
			if (trav_p->topnumber > trav_p->lastread)
			{
				return trav_p->topnumber - trav_p->lastread;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			trav_p = trav_p->next;
		}
	}

	return 0;
}

// Retrieve response number from NNTP server
int CNewsServer::NntpResponse(void)
{
	char buf[1024];
	int retries = 0;
	while (retries < 5)
	{
		if (0 > ReadLine(socket, buf, sizeof(buf)))
		{
			retries ++;
			sleep(1);
		}
		else
			retries = 10;
	}

	if (10 == retries)
	{
		int response = atoi(buf);
		return response;
	}
	else
		return -1; // Error		
}
