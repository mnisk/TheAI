/*
	Copyright 1999-2001, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

/*
 * handles mime type information for dosfs
 * gets/sets mime information in vnode
 */


#define MIME_STRING_TYPE 'MIMS'

#include "system_dependencies.h"

#ifndef FS_SHELL
#include <file_systems/mime_ext_table.h>
#endif

#include "dosfs.h"
#include "attr.h"


int32 kBeOSTypeCookie = 0x1234;

#define DPRINTF(a,b) if (debug_attr > (a)) dprintf b

status_t set_mime_type(vnode *node, const char *filename)
{
#ifdef FS_SHELL
	return B_ERROR;
#else
	return set_mime(&node->mime, filename);
#endif
}


status_t
dosfs_open_attrdir(fs_volume *_vol, fs_vnode *_node, void **_cookie)
{
	DPRINTF(0, ("dosfs_open_attrdir called\n"));

	if ((*_cookie = malloc(sizeof(uint32))) == NULL) {
		return ENOMEM;
	}
	*(int32 *)(*_cookie) = 0;

	return 0;
}


status_t
dosfs_close_attrdir(fs_volume *_vol, fs_vnode *_node, void *_cookie)
{
	DPRINTF(0, ("dosfs_close_attrdir called\n"));

	*(int32 *)_cookie = 1;

	return 0;
}


status_t
dosfs_free_attrdir_cookie(fs_volume *_vol, fs_vnode *_node, void *_cookie)
{
	DPRINTF(0, ("dosfs_free_attrcookie called\n"));

	if (_cookie == NULL) {
		dprintf("error: dosfs_free_attrcookie called with null cookie\n");
		return EINVAL;
	}

	*(int32 *)_cookie = 0x87654321;
	free(_cookie);

	return 0;
}


status_t
dosfs_rewind_attrdir(fs_volume *_vol, fs_vnode *_node, void *_cookie)
{
	DPRINTF(0, ("dosfs_rewind_attrdir called\n"));

	if (_cookie == NULL) {
		dprintf("error: dosfs_rewind_attrcookie called with null cookie\n");
		return EINVAL;
	}

	*(uint32 *)_cookie = 0;
	return 0;
}


status_t
dosfs_read_attrdir(fs_volume *_vol, fs_vnode *_node, void *_cookie,
	struct dirent *entry, size_t bufsize, uint32 *num)
{
	nspace *vol = (nspace *)_vol->private_volume;
	vnode *node = (vnode *)_node->private_node;
	int32 *cookie = (int32 *)_cookie;

	DPRINTF(0, ("dosfs_read_attrdir called\n"));

	*num = 0;

	RecursiveLocker lock(vol->vlock);

	if ((*cookie == 0) && (node->mime)) {
		*num = 1;

		entry->d_ino = node->vnid;
		entry->d_dev = vol->id;
		strcpy(entry->d_name, "BEOS:TYPE");
		entry->d_reclen = offsetof(struct dirent, d_name) + strlen(entry->d_name) + 1;
	}

	*cookie = 1;

	return 0;
}


status_t
dosfs_open_attr(fs_volume *_vol, fs_vnode *_node, const char *name,
	int openMode, void **_cookie)
{
	nspace *vol = (nspace *)_vol->private_volume;
	vnode *node = (vnode *)_node->private_node;

	if (strcmp(name, "BEOS:TYPE"))
		return ENOENT;

	RecursiveLocker lock(vol->vlock);

	if (node->mime == NULL) {
		return ENOENT;
	}

	*_cookie = &kBeOSTypeCookie;
	return B_OK;
}


status_t
dosfs_close_attr(fs_volume *_vol, fs_vnode *_node, void *cookie)
{
	return B_OK;
}


status_t
dosfs_free_attr_cookie(fs_volume *_vol, fs_vnode *_node, void *cookie)
{
	return B_OK;
}


status_t
dosfs_read_attr_stat(fs_volume *_vol, fs_vnode *_node, void *_cookie,
	struct stat *stat)
{
	nspace *vol = (nspace *)_vol->private_volume;
	vnode *node = (vnode *)_node->private_node;

	DPRINTF(0, ("dosfs_read_attr_stat\n"));

	if (_cookie != &kBeOSTypeCookie)
		return ENOENT;

	RecursiveLocker lock(vol->vlock);

	if (node->mime == NULL) {
		return ENOENT;
	}

	stat->st_type = MIME_STRING_TYPE;
	stat->st_size = strlen(node->mime) + 1;

	return 0;
}


status_t
dosfs_read_attr(fs_volume *_vol, fs_vnode *_node, void *_cookie, off_t pos,
	void *buffer, size_t *_length)
{
	nspace *vol = (nspace *)_vol->private_volume;
	vnode *node = (vnode *)_node->private_node;
	ssize_t length;

	DPRINTF(0, ("dosfs_read_attr\n"));

	if (_cookie != &kBeOSTypeCookie)
		return ENOENT;

	RecursiveLocker lock(vol->vlock);

	if (node->mime == NULL)
		return ENOENT;

	if ((pos < 0) || (pos > (off_t)strlen(node->mime)))
		return EINVAL;

	length = user_strlcpy((char*)buffer, node->mime + pos, *_length);
	if (length < B_OK)
		return B_BAD_ADDRESS;

	if ((size_t)length < *_length)
		*_length = length + 1;

	return 0;
}


// suck up application attempts to set mime types; this hides an unsightly
// error message printed out by zip
status_t
dosfs_write_attr(fs_volume *_vol, fs_vnode *_node, void *_cookie,
	off_t pos, const void *buffer, size_t *_length)
{
	DPRINTF(0, ("dosfs_write_attr\n"));

	*_length = 0;

	if (_cookie != &kBeOSTypeCookie)
		return ENOSYS;

	return B_OK;
}
