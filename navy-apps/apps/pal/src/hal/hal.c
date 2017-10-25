#include "hal.h"
#include <string.h>
#include <stdlib.h>
#include <ndl.h>

#define NR_KEYS 18

#define W 320
#define H 200

static uint32_t systime;
static int key_state[128];

void PAL_KeyPressHandler(int);
void PAL_KeyReleaseHandler(int);

int
PAL_PollEvent(
   SDL_Event *event
)
/*++
  Purpose:

    Poll and process one event.

  Parameters:

    [OUT] event - Events polled from SDL.

  Return value:

    Return value of PAL_PollEvent.

--*/
{
  NDL_Event evt;
  NDL_WaitEvent(&evt);
  
  if (evt.type == NDL_EVENT_TIMER) {
    systime = evt.data;
  }

  if (evt.type == NDL_EVENT_KEYUP || evt.type == NDL_EVENT_KEYDOWN) {
    int key = -1, kd = evt.type == NDL_EVENT_KEYDOWN;
    switch (evt.data) {
      case NDL_SCANCODE_UP: key = K_UP; break;
      case NDL_SCANCODE_DOWN: key = K_DOWN; break;
      case NDL_SCANCODE_LEFT: key = K_LEFT; break;
      case NDL_SCANCODE_RIGHT: key = K_RIGHT; break;
      case NDL_SCANCODE_RETURN: key = K_RETURN; break;
      case NDL_SCANCODE_SPACE: key = K_SPACE; break;
      case NDL_SCANCODE_ESCAPE: key = K_ESCAPE; break;
      case NDL_SCANCODE_PAGEUP: key = K_PAGEUP; break;
      case NDL_SCANCODE_PAGEDOWN: key = K_PAGEDOWN; break;
      case NDL_SCANCODE_R: key = K_r; break;
      case NDL_SCANCODE_A: key = K_a; break;
      case NDL_SCANCODE_D: key = K_d; break;
      case NDL_SCANCODE_E: key = K_e; break;
      case NDL_SCANCODE_W: key = K_w; break;
      case NDL_SCANCODE_Q: key = K_q; break;
      case NDL_SCANCODE_S: key = K_s; break;
      case NDL_SCANCODE_F: key = K_f; break;
      case NDL_SCANCODE_P: key = K_p; break;
    }
    if (key != -1 && key_state[key] != kd) {
      key_state[key] = kd;
      if (kd) PAL_KeyPressHandler(key);
      else PAL_KeyReleaseHandler(key);
    }
    return true;
  }

  return false;
}

void SDL_WaitUntil(uint32_t tick) {
  while (systime < tick) {
    while (PAL_PollEvent(NULL));
  }
  if (systime > tick) return;
}

uint32_t SDL_GetTicks() {
  return systime;
}

void SDL_Delay(uint32_t ms) {
}

static uint8_t vmem[W * H];
static uint32_t fb[W * H];
static intptr_t VMEM_ADDR = (intptr_t)&vmem[0];

static uint32_t palette[256];

static void redraw() {
  for (int i = 0; i < W; i ++)
    for (int j = 0; j < H; j ++)
      fb[i + j * W] = palette[vmem[i + j * W]];

  NDL_DrawRect(fb, 0, 0, W, H);
  NDL_Render();
}

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, 
    SDL_Surface *dst, SDL_Rect *dstrect) {
  printf("SDL_BlitSurface begin\n");
  assert(dst && src);

  int sx = (srcrect == NULL ? 0 : srcrect->x);
  int sy = (srcrect == NULL ? 0 : srcrect->y);
  int dx = (dstrect == NULL ? 0 : dstrect->x);
  int dy = (dstrect == NULL ? 0 : dstrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);
  if(dst->w - dx < w) { w = dst->w - dx; }
  if(dst->h - dy < h) { h = dst->h - dy; }
  if(dstrect != NULL) {
    dstrect->w = w;
    dstrect->h = h;
  }

  /* copy pixels from position (`sx', `sy') with size
   * `w' X `h' of `src' surface to position (`dx', `dy') of
   * `dst' surface.
   */

  //fprintf(stderr, "(%d, %d) -> (%d, %d), %d x %d\n", sx, sy, dx, dy, w, h);
  for (int i = 0; i < w; i ++)
    for (int j = 0; j < h; j ++) {
      uint8_t idx = src->pixels[(sx + i) + (sy + j) * src->w];
      dst->pixels[(dx + i) + (dy + j) * dst->w] = idx;
    }
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
  printf("SDL_FillRect begin\n");
  assert(dst);
  assert(color <= 0xff);

  int dx = (dstrect == NULL ? 0 : dstrect->x);
  int dy = (dstrect == NULL ? 0 : dstrect->y);
  int w = (dstrect == NULL ? dst->w : dstrect->w);
  int h = (dstrect == NULL ? dst->h : dstrect->h);
  if(dst->w - dx < w) { w = dst->w - dx; }
  if(dst->h - dy < h) { h = dst->h - dy; }
  

  // TODO: color is uint32_t, what about palette?
  for (int i = 0; i < w; i ++)
    for (int j = 0; j < h; j ++) {
      dst->pixels[(dx + i) + (dy + j) * dst->w] = color;
    }

  /* Fill the rectangle area described by `dstrect'
   * in surface `dst' with color `color'. If dstrect is
   * NULL, fill the whole surface.
   */
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, 
    int firstcolor, int ncolors) {
  printf("SDL_SetPalette begin\n");
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  if(s->format->palette->colors == NULL || s->format->palette->ncolors != ncolors) {
    if(s->format->palette->ncolors != ncolors && s->format->palette->colors != NULL) {
      /* If the size of the new palette is different 
       * from the old one, free the old one.
       */
      free(s->format->palette->colors);
    }

    /* Get new memory space to store the new palette. */
    s->format->palette->colors = malloc(sizeof(SDL_Color) * ncolors);
    assert(s->format->palette->colors);
  }

  /* Set the new palette. */
  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
      palette[i] = (r << 16) | (g << 8) | b;
    }
    redraw();
  }
}

/* ======== The following functions are already implemented. ======== */

void SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h) {
  printf("SDL_UpdateRect begin\n");
  assert(screen);
  printf("screen->pitch:%d\n",screen->pitch);
  assert(screen->pitch == W);
  printf("screen->flags:%d\nSDL_HWSURFACE:%d\n",screen->flags,SDL_HWSURFACE);
  // this should always be true in NEMU-PAL
  assert(screen->flags & SDL_HWSURFACE);

  redraw();
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, 
    SDL_Surface *dst, SDL_Rect *dstrect) {
  printf("SDL_SoftStretch begin\n");
  assert(src && dst);
  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    /* No other case occurs in NEMU-PAL. */
    assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  printf("SDL_CreateRGBSurface begin\n");
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s);
  s->format->palette = malloc(sizeof(SDL_Palette));
  assert(s->format->palette);
  s->format->palette->colors = NULL;

  s->format->BitsPerPixel = depth;
  
  s->flags = flags;
  
  printf("s->flags:%d\n",s->flags);
  s->w = width;
  s->h = height;

  s->pitch = (width * depth) >> 3;
  printf("s->pitch:%d\twidth:%d\tdepth:%d\n",s->pitch,width,depth);
  s->pixels = (flags & SDL_HWSURFACE ? (void *)VMEM_ADDR : malloc(s->pitch * height));
  assert(s->pixels);

  return s;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
    printf("SDL_SetVideoMode begin\n");
	return SDL_CreateRGBSurface(flags,  width, height, bpp,
      0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void SDL_FreeSurface(SDL_Surface *s) {
  printf("SDL_FreeSurface begin\n");
  if(s != NULL) {
    if(s->format != NULL) {
      if(s->format->palette != NULL) {
        if(s->format->palette->colors != NULL) {
          free(s->format->palette->colors);
        }
        free(s->format->palette);
      }

      free(s->format);
    }
    
    if(s->pixels != NULL && s->pixels != (void*)VMEM_ADDR) {
      free(s->pixels);
    }

    free(s);
  }
}

void hal_init() {
  NDL_OpenDisplay(W, H);
}
