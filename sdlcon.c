/** SDL Interface for FVM **/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <SDL.h>
#include <sdlcon.h>
int screen_x = 0;
int screen_y = 0;
uint32_t keycode;
GL_EVENT_t event;
GL_SURFACE_t* screen;
GL_SURFACE_t* bmpfont;
SDL_Color COLOR = { 255, 255, 255, 60 };
void FVM_SDL_init(GL_WIDTH_t width, GL_HEIGHT_t height, GL_DEPTH_t color)
{
	//! Init everything.
	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(width, height, color, SDL_SWSURFACE);
}
GL_SURFACE_t* FVM_SDL_loadbmp(const char* FILENAME)
{
	GL_SURFACE_t* Image = SDL_LoadBMP(FILENAME);
	GL_SURFACE_t* OptimizedImage = NULL;
	if (Image != NULL)
	{
		OptimizedImage = SDL_DisplayFormat(Image);
		SDL_FreeSurface(Image);
		return OptimizedImage;
	}
	return NULL;
}
void FVM_SDL_updatedisplay(GL_SURFACE_t* SURFACE)
{
	 SDL_BlitSurface(SURFACE, NULL, screen, NULL); 
	 SDL_Flip(screen);
}
void FVM_SDL_delay(uint32_t time)
{
	SDL_Delay(time);
}
void FVM_SDL_blitsurface(int x, int y, GL_SURFACE_t* source, GL_SURFACE_t* dest)
{
	GL_RECT_t NewRect;
	NewRect.x = x;
	NewRect.y = y;
	SDL_BlitSurface(source,NULL,dest, &NewRect);
}
void FVM_SDL_setwincaption(const char* caption)
{
	SDL_WM_SetCaption(caption, NULL);
}
void FVM_SDL_putentry(GL_SURFACE_t* font, GL_SURFACE_t* dest, int x, int y, unsigned char c)
{
        static SDL_Rect font_rect, dest_rect;
       	int indice = c - 32;
	if (c >= 'a' && c <= 'z')
	{
		char n = toupper(c);
		c = n;
		indice = c - 32;
	}
	else if(c < 32 || c > 109) {
		return;
	}
        font_rect.x = indice *  (font->w / 77);
        font_rect.y = 0;
        font_rect.w = font->w / 77;
        font_rect.h = font->h;
        dest_rect.x = x;
        dest_rect.y = y;
        dest_rect.w = font->w / 77;
        dest_rect.h = font->h;

        SDL_BlitSurface(font, &font_rect, dest, &dest_rect);
}
void FVM_SDL_putchar(GL_SURFACE_t* font, GL_SURFACE_t* dest, unsigned char c)
{	
	if (c == '\n')
	{
		screen_x = 0;
		screen_y += 20;
	}
	else {
		FVM_SDL_putentry(font, dest, screen_x, screen_y, c);
		screen_x += 10;
	}
	if (screen_x >= GL_MAX_X)
	{
		screen_y += 20;
		screen_x = 0;
	}
	// Are we at the end of the screen?
	if (screen_y >= GL_MAX_Y)
	{
		// Yes. Scroll
		// This basically involves copying a part of the screen on the screen itself
		SDL_Rect temp, temp2;
		temp.x = 0; // X co-ordinate 
		temp.y = FONT_HEIGHT; // Y-cord First Row (In scrolling, the 0th row is replaced by the 1st row)
		temp.h = GL_MAX_Y; // ...And copy till the last line
		temp.w = GL_MAX_X; // Width should be the screen
		// COPY!
		SDL_BlitSurface(screen, &temp, screen, NULL);
		//! Clear last line
		temp2.x = 0;
		temp2.y = GL_MAX_Y - FONT_HEIGHT;
		temp2.h = FONT_HEIGHT;
		temp2.w = GL_MAX_X;	
		SDL_FillRect(screen, &temp2, 0x000000);
		// Set X to 0
		screen_x = 0;
		// Set Y to last row 
		screen_y = GL_MAX_Y - FONT_HEIGHT;
	}
}
void FVM_SDL_putstring(GL_SURFACE_t* font, GL_SURFACE_t* dest, const char* string)
{
	unsigned int i = 0;
	while( i <= strlen(string))
	{
		FVM_SDL_putchar(font, dest, string[i]);
		i++;
	}
}
void FVM_SDL_putstringat(GL_SURFACE_t* font, GL_SURFACE_t* dest, int x, int y, const char* string)
{
	unsigned int i = 0;
	while (i <= strlen(string))
	{
		FVM_SDL_putentry(font, dest, x, y, string[i]);
		i++;
	}
}
void SDL_printf(GL_SURFACE_t* font, GL_SURFACE_t*  dest, const char *fmt, ...) {
	if (strlen(fmt) > 512)
	{
		return;
	}
       	char buf[512];
	buf[512] = '\0'; // Null terminate
        va_list pvar;
        va_start(pvar, fmt);

        vsnprintf(buf, 511, fmt, pvar);
        FVM_SDL_putstring(font, dest, buf);
}

