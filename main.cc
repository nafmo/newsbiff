// Newsbiff, a program for checking if there are unread news messages
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

#include <string>
#include <stdlib.h>
#include <getopt.h>
#include "news.h"

int main(int argc, char **argv)
{
	int option;
	string servername = getenv("NNTPSERVER") ? string(getenv("NNTPSERVER"))
	                                         : "news.mdh.se";
	string newsrc = string(getenv("HOME")) + "/.newsrc";

	while (-1 != (option = getopt(argc, argv, "s:n:")))
	{
		switch (option)
		{
			case 's':
				servername = optarg;
				break;

			case 'n':
				newsrc = optarg;
				break;
		}
	}

	CNewsServer	server(servername);
	int a = server.CheckLastRead(newsrc);
	cout << a;

	if (argc > optind)
		cout << " totalt, varav: " << endl;
	for (int i = optind; i < argc; i ++)
		cout << argv[i] << ": " << server.QueryUnread(string(argv[i])) << endl;
cout << endl;
}
