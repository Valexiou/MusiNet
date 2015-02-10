/**
 * @file vidbridge.c Video bridge
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include "vidbridge.h"


static struct vidisp *vidisp;
static struct vidsrc *vidsrc;

struct hash *ht_src;
struct hash *ht_disp;


static int module_init(void)
{
	int err;

	err  = hash_alloc(&ht_src, 32);
	err |= hash_alloc(&ht_disp, 32);
	if (err)
		return err;

	err = vidisp_register(&vidisp, "vidbridge", vidbridge_disp_alloc,
			      NULL, vidbridge_disp_display, 0);
	if (err)
		return err;

	err = vidsrc_register(&vidsrc, "vidbridge", vidbridge_src_alloc, NULL);
	if (err)
		return err;

	return err;
}


static int module_close(void)
{
	vidsrc = mem_deref(vidsrc);
	vidisp = mem_deref(vidisp);

	ht_src  = mem_deref(ht_src);
	ht_disp = mem_deref(ht_disp);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(vidbridge) = {
	"vidbridge",
	"video",
	module_init,
	module_close,
};
