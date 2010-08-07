/*
 * Mesa 3-D graphics library
 * Version:  7.9
 *
 * Copyright 2009, VMware, Inc.
 * All Rights Reserved.
 * Copyright (C) 2010 LunarG Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Keith Whitwell <keithw@vmware.com>
 *    Jakob Bornecrantz <wallbraker@gmail.com>
 *    Chia-I Wu <olv@lunarg.com>
 */

#include "pipe/p_format.h"
#include "pipe/p_state.h"
#include "state_tracker/st_api.h"

#include "dri_context.h"
#include "dri_drawable.h"
#include "dri_extensions.h"


/**
 * These are used for GLX_EXT_texture_from_pixmap
 */
static void
dri_set_tex_buffer2(__DRIcontext *pDRICtx, GLint target,
                     GLint format, __DRIdrawable *dPriv)
{
   struct dri_context *ctx = dri_context(pDRICtx);
   struct dri_drawable *drawable = dri_drawable(dPriv);
   struct pipe_resource *pt;

   dri_drawable_validate_att(drawable, ST_ATTACHMENT_FRONT_LEFT);

   pt = drawable->textures[ST_ATTACHMENT_FRONT_LEFT];

   if (pt) {
      enum pipe_format internal_format = pt->format;

      if (format == __DRI_TEXTURE_FORMAT_RGB)  {
         /* only need to cover the formats recognized by dri_fill_st_visual */
         switch (internal_format) {
         case PIPE_FORMAT_B8G8R8A8_UNORM:
            internal_format = PIPE_FORMAT_B8G8R8X8_UNORM;
            break;
         case PIPE_FORMAT_A8R8G8B8_UNORM:
            internal_format = PIPE_FORMAT_X8R8G8B8_UNORM;
            break;
         default:
            break;
         }
      }

      ctx->st->teximage(ctx->st,
            (target == GL_TEXTURE_2D) ? ST_TEXTURE_2D : ST_TEXTURE_RECT,
            0, internal_format, pt, FALSE);
   }
}

static void
dri_set_tex_buffer(__DRIcontext *pDRICtx, GLint target,
                    __DRIdrawable *dPriv)
{
   dri_set_tex_buffer2(pDRICtx, target, __DRI_TEXTURE_FORMAT_RGBA, dPriv);
}

const __DRItexBufferExtension driTexBufferExtension = {
    { __DRI_TEX_BUFFER, 2 /* __DRI_TEX_BUFFER_VERSION */ },
   dri_set_tex_buffer,
   dri_set_tex_buffer2,
};

