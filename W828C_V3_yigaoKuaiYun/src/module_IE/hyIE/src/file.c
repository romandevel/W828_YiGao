/**
 * Functions to open a stream from a file on the local disk.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

/*
 * Copyright (C) 1999, Tomas Berndtsson <tomas@nocrew.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "file.h"


/**
 * Opens a file and gets the file descriptor for it. This also looks at the
 * extension of the filename, and tries to decide what MIME type should be
 * set for it. This might later be moved into a specialized part that handles
 * all MIME arrends. 
 *
 * @param filename The name of the file to open.
 * @param filename If '-' is specified, stdin is opened for reading.
 * @param headers Simulated HTTP headers to be able to set the MIME type
 * @param headers according to the extension of the filename.
 *
 * @return the file descriptor for the file or a negative value
 * @return if an error occurred, which we hope it will not.
 */
int protocol_file_open(char *filename, struct protocol_http_headers *headers)
{
  int fd;
  char *extension;
  
  if (strcmp(filename, "-"))
  {
	fd = AbstractFileSystem_Open(filename, 0);
    if(fd < 0) 
	{
	  headers->return_code = 404;
	  return fd;
	}
  }

  headers->return_code = 200;

  headers->real_url = (char *)GETBUFFER(7 + strlen(filename) + 16,HYIE_MEM_IDX);
  if(headers->real_url == NULL)
    return fd;
  
  strcpy(headers->real_url, "file://");
  strcat(headers->real_url, filename);
  
  headers->content_type_major = (char *)GETBUFFER(256,HYIE_MEM_IDX);
  if(headers->content_type_major == NULL) {
    headers->content_type_minor = NULL;
    return fd;
  }
  headers->content_type_minor = (char *)GETBUFFER(256,HYIE_MEM_IDX);
  if(headers->content_type_minor == NULL) {
    FREEBUFFER(headers->content_type_major,HYIE_MEM_IDX);
    headers->content_type_major = NULL;
    return fd;
  }

  extension = strrchr(filename, '.');
  if(extension == NULL || !strcasecmp(extension, ".html") 
	  || !strcasecmp(extension, ".htm")||!strcasecmp(extension, ".HTM")) 
  {
    strcpy(headers->content_type_major, "text");
    strcpy(headers->content_type_minor, "html");
  }
  else if(!strcasecmp(extension, ".wml"))
  {
    strcpy(headers->content_type_major, "text");
    strcpy(headers->content_type_minor, "vnd.wap.wml");
  }
  else if(!strcasecmp(extension, ".xml"))
  {
    strcpy(headers->content_type_major, "text");
    strcpy(headers->content_type_minor, "xml");
  }

  else if(!strcasecmp(extension, ".txt")) 
  {
    strcpy(headers->content_type_major, "text");
    strcpy(headers->content_type_minor, "plain");
  }
  else if(!strcasecmp(extension, ".jpg") 
    	||!strcasecmp(extension, ".jpeg") 
		||!strcasecmp(extension, ".jpe")
		||!strcasecmp(extension, ".JPG")) 
  {
    strcpy(headers->content_type_major, "image");
    strcpy(headers->content_type_minor, "jpeg");
  } 
  else if(!strcasecmp(extension, ".png")) 
  {
	strcpy(headers->content_type_major, "image");
    strcpy(headers->content_type_minor, "png");
  } 
  else if(!strcasecmp(extension, ".gif")) 
  {
    strcpy(headers->content_type_major, "image");
    strcpy(headers->content_type_minor, "gif");
  }
  else //the type we unsupport!
  {
    strcpy(headers->content_type_major, "DEBUG");
    strcpy(headers->content_type_minor, "DEBUG");
  }
  
  return fd;
}

/**
* Close a file descriptor
*
* @param fd The file descriptor associated with the file to close. 
* @param fd If it is stdin, we do not close it.
*/
int protocol_file_close(int fd)
{
#ifdef PLATFORM_PC
	fclose(fd);
#else
	AbstractFileSystem_Close( fd );
#endif	
	return HYWB_OK;
}

