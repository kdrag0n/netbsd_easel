/*-
 * Copyright (c) 2007 Joerg Sonnenberger <joerg@NetBSD.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <nbcompat.h>

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_ERR_H
#include <err.h>
#endif
#if HAVE_PWD_H
#include <grp.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_TIME_H
#include <time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "lib.h"
#include "create.h"

static void
update_ids(struct memory_file *file)
{
	if (file->owner != NULL) {
		uid_t uid;

		if (uid_from_user(file->owner, &uid) == -1)
			errx(2, "user %s unknown", file->owner);
		file->st.st_uid = uid;
	} else {
		file->owner = user_from_uid(file->st.st_uid, 1);
	}

	if (file->group != NULL) {
		gid_t gid;

		if (gid_from_group(file->group, &gid) == -1)
			errx(2, "group %s unknown", file->group);
		file->group = file->group;
		file->st.st_gid = gid;
	} else {
		file->group = group_from_gid(file->st.st_gid, 1);
	}
}

struct memory_file *
make_memory_file(const char *archive_name, void *data, size_t len,
    const char *owner, const char *group, mode_t mode)
{
	struct memory_file *file;

	if ((file = malloc(sizeof(*file))) == NULL)
		err(2, "malloc failed");

	file->name = archive_name;
	file->owner = owner;
	file->group = group;

	file->data = data;
	file->len = len;

	memset(&file->st, 0, sizeof(file->st));

	file->st.st_atime = file->st.st_ctime = file->st.st_mtime = time(NULL);

	file->st.st_nlink = 1;
	file->st.st_size = len;
	file->st.st_mode = mode | S_IFREG;

	update_ids(file);

	return file;
}

struct memory_file *
load_memory_file(const char *disk_name,
    const char *archive_name, const char *owner, const char *group,
    mode_t mode)
{
	struct memory_file *file;
	int fd;

	if ((file = malloc(sizeof(*file))) == NULL)
		err(2, "malloc failed");

	file->name = archive_name;
	file->owner = owner;
	file->group = group;
	file->mode = mode;

	fd = open(disk_name, O_RDONLY);
	if (fd == -1)
		err(2, "cannot open file %s", disk_name);
	if (fstat(fd, &file->st) == -1)
		err(2, "cannot stat file %s", disk_name);

	update_ids(file);

	if ((file->st.st_mode & S_IFMT) != S_IFREG)
		errx(1, "meta data file %s is not regular file", disk_name);
	if (file->st.st_size > SSIZE_MAX ||
	    (file->data = malloc(file->st.st_size)) == NULL)
		errx(2, "cannot allocate memory for file %s", disk_name);

	if (read(fd, file->data, file->st.st_size) != file->st.st_size)
		err(2, "cannot read file into memory %s", disk_name);

	file->len = file->st.st_size;

	close(fd);

	return file;
}

void
free_memory_file(struct memory_file *file)
{
	if (file != NULL) {
		free(file->data);
		free(file);
	}
}
