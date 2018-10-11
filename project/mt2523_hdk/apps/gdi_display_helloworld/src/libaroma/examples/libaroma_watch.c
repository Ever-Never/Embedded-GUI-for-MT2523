/* libaroma header */

#ifndef __libaroma_watch_c__
#define __libaroma_watch_c__

#include <aroma.h>


void init_libaroma()
{
	libaroma_config()->fb_shm_name[0] = 0;
	libaroma_start();

	/* clean display */
	libaroma_canvas_blank(libaroma_fb()->canvas);
	libaroma_sync();

	/* load font - id=0 */
	libaroma_font(0, libaroma_stream("file:///fonts/Roboto-Regular.ttf"));
	libaroma_font(1, libaroma_stream("file:///fonts/DroidSansFallback.ttf"));
} /* End of init_libaroma */



int main(int argc, char **argv){

	init_libaroma();

	libaroma_end();

	return 0;
} /* End of main */


#endif

