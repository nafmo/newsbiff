// Routines for interacting with TCP streams
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include "tcp.h"

// Establish a TCP connection to the given address and port
// returns the socket number (file handle) for the connection,
// or -1 in case of error
int ConnectTCP(const char *address, int port)
{
	struct hostent					*host;
	union
	{
		struct sockaddr adr;
		struct sockaddr_in in;
	}								addr;
	int								sock;

	if (!address || address[0] == 0) return -1;

	host = gethostbyname(address);
	addr.in.sin_port = htons((unsigned short) port);
	addr.in.sin_addr.s_addr = *(unsigned long *) host->h_addr_list[0];
	addr.in.sin_family = AF_INET;

	// Open socket
	if (-1 == (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
		return -1;

	// Connect socket
	if (-1 == connect(sock, &addr.adr, sizeof(addr)))
		return -1;

	return sock;
}

// Disconnect the established TCP connection
void DisconnectTCP(int socket)
{
	shutdown(socket, 2);
	close(socket);
}

// Reads a line of text (terminated by newline or EOF) from a stream
int ReadLine(int socket, void *buf, size_t maxlen)
{
	char	tmp, *p;
	int		err, len;

	len = 0;
	p = (char *) buf;

	do
	{
		err = read(socket, &tmp, 1);
		if (err < 0) return err;
		if (err > 0 && len < maxlen && tmp != '\r')
		{
			*(p ++) = tmp;
			len ++;
		}
	} while (err != 0 && tmp != '\n');

	if ('\n' == tmp) p --; // Strip newline
	
	*p = 0;
	return len;
}
