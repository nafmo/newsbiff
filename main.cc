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

#include <String.h>
#include <stdlib.h>
#include "news.h"

int main(int argc, char **argv)
{
	CNewsServer	server(String("news.mdh.se"));
	String newsrc = String(getenv("HOME")) + "/.newsrc";
	int a = server.CheckLastRead(newsrc);
	cout << newsrc << endl;
	cout << a;

	if (argc > 1)
		cout << " totalt, varav: " << endl;
	for (int i = 1; i < argc; i ++)
		cout << argv[i] << ": " << server.QueryUnread(String(argv[i])) << endl;
}
