//repo: https://github.com/9999years/i-c-the-light
//MIT/expat license
//rebecca turner
//consult ../readme.md

//render code
#include "main.h"

char outfile_base[256];
int convert_immediately;

void saveframe(SDL_Surface *screen)
{
	char filename[256] = "UNINITIALIZED.ppm";
	unsigned long int timeint = time(NULL);
	sprintf(filename, "%s/image%lu_f%d.ppm", outfile_base, timeint, frame);
	printf("printing %s\n", filename);
	int writestatus;
	if(
		(writestatus = writeppm(
			filename,
			screen->w,
			screen->h,
			screen->pixels
		)) != 0
	) {
		switch(writestatus) {
		case PPM_FILE_OPEN_FAILURE:
			fprintf(logfile, "image write error!\n");
			break;
		case PPM_WRITE_ALL_BLACK:
			fprintf(logfile, "image is all black!\n");
			break;
		default:
			fprintf(logfile, "unknown write error!\n");
			break;
		}
		return;
	}
	char shacmd[256];
	sprintf(shacmd, "sha256sum %s", filename);
	FILE *file = popen(shacmd, "r");
	if(file == NULL) {
		fprintf(logfile, "failed to get image hash!\n");
		return;
	}
	//the hash is 64 characters but we need a 0 at the end too
	char sha[68];
	int i;
	char c;
	for(i = 0; (i < 64) && (c != EOF) && (c != 0x20); i++) {
		sha[i] = c = fgetc(file);
	}
	sha[i++] = 0;
	printf("SHA: %s\n", sha);
	fprintf(logfile, "SHA: %s\n", sha);
	pclose(file);

	char hashfilename[256];
	sprintf(hashfilename, "%s/hash/%s", outfile_base, sha);

	if(_access(hashfilename, 0) != -1) {
		//file exists, delete img
		printf("image already rendered, deleting\n");
		if(unlink(filename) != 0) {
			fprintf(logfile, "image delete error!\n");
		}
	} else {
		FILE *hashfile = fopen(hashfilename, "w");
		if(hashfile == NULL)
			fprintf(logfile,
				"hash file write error!\nfilename: %s\n",
				hashfilename
			);
		fclose(hashfile);
	}

	if(convert_immediately) {
		printf("converting ppm to png\n");
		char mogrifycmd[512];
		sprintf(mogrifycmd, "mogrify -format png %s", filename);
		file = popen(mogrifycmd, "r");
		if(file == NULL) {
			fprintf(logfile, "failed to get image hash!\n");
			return;
		}
		pclose(file);
		if(unlink(filename) != 0) {
			fprintf(logfile, "ppm delete error!\n");
			return;
		}
	}
	return;
}

int handleevents(SDL_Surface *screen)
{
	SDL_Event event;
	while((SDL_PollEvent(&event))) {
		switch(event.type) {
		case SDL_KEYDOWN:
			// if escape is pressed, quit
			if((event.key.keysym.sym == SDLK_ESCAPE) || (event.key.keysym.sym == SDLK_q)) {
				return 1;
			} else if(event.key.keysym.sym == SDLK_s) {
				saveframe(screen);
			}
			break;

		case SDL_QUIT:
			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	printf("Hello!\n");
	const time_t unixtime = time(NULL);
	printf( "I C the Light by Rebecca Turner (MIT/Expat)\n"
		"%s\n"
		"See --help\n", ctime(&unixtime)
	);

	//don't want stuff to be the same every time...
	srand(unixtime);

	printf(
		"                  .V.\n"
		"              \\.\\_____/./\n"
		"            \\-.=^ -  '^=././\n"
		"       ===\\-\\(/`  /#\\  *\\)/-/===\n"
		" >=------  <{.~  (#O#)   .}>  ------=<\n"
		"       ===/-/(\\  .\\#/  ~/)\\-\\===\n"
		"           /-/*=-_____-=*\\^\\\n"
		"              -/ /*~*\\^ \\\n"
		"                   ^\n"
		"                                        \n"
		"╭─────────────────────────────────────╮\n"
		"│ ·           ╷         ╷ ·     ╷     │\n"
		"│ ╷   ╭─╮   ┼ ├─╮ ╭─╮   │ ╷ ╭─╮ ├─╮ ┼ │\n"
		"│ │   │     │ │ │ ├─┘   │ │ ╰─╯ │ │ │ │\n"
		"│ ╵   ╰─╯   ╰ ╵ ╵ ╰─╯   ╵ ╵ ╰─╮ ╵ ╵ ╰ │\n"
		"╰───────────────────────────╰─╯───────╯\n"
		"                                       \n"
	);

	if(searchargs(argc, argv, "--help") != -1) {
		printf(
"I C the Light help:\n\n"

"-w or --window: Request a window. (Does nothing if I C the Light was compiled\n"
"    without SDL.)\n\n"

"-r or --resolution: Specifies resolution via next argument, in wxh format.\n"
"    e.g. ./icthelight -r 500x500\n\n"

"-s or --scale: Specifies resolution scaling. (If you know you want a smooth\n"
"    500x500 image, use -r 500x500 -s 2 to render at 1000x1000.)\n\n"

"-o or --output: Output directory (where images are written).\n"
"    ../output/ by default, must have a hash/ subdirectory.\n\n"

"-c or --convert: Convert the .ppm output to .png after saving. Might take up\n"
"    a little bit of time, requires mogrify to be in your PATH\n\n"

"--help: Shows this text and then quits.\n\n"

"../readme.md, becca.ooo/i-c-the-light, or github.com/9999years/i-c-the-light\n"
"might have more information.\n"
		);
		return 0;
	}

	frame = 0;

	initializelogfile();

	//The window we'll be rendering to
	SDL_Window *window = NULL;

	//The surface contained by the window
	SDL_Surface *screen = NULL;

	//screen w/h
	int width, height;

	int inx =
		searchargs(argc, argv, "-r") &
		searchargs(argc, argv, "--resolution");
	if(inx != -1) {
		//should allow up to 5-digits each side
		char resolution_input[16];
		//put str into memory
		strcpy(resolution_input, argv[inx + 1]);
		if(resolution_input == NULL) {
			printf("couldn't get a resolution! try again with a "
				"better argument.\n");
			return -1;
		}
		//get the string after the x
		//(so in 100x100, we get "x100")
		char *second = strchr(resolution_input, 'x');
		if(second == NULL) {
			//no x, assume square res
			height = width = strtod(resolution_input, NULL);
		} else {
			//add one byte to strip off the leading x, get a number
			height = strtod(second + sizeof(char), NULL);
			//set the 'x' to \0 to make c think that's the end of
			//the string
			second = '\0';
			//get the first number
			width = strtod(resolution_input, NULL);
		}
	} else {
		//compile-time defaults
		width = SCREEN_WIDTH;
		height = SCREEN_WIDTH;
	}

	inx =
		searchargs(argc, argv, "-s") &
		searchargs(argc, argv, "--scale");
	if(inx != -1) {
		//god help you if 8 chars is too little????
		char scale_input[16];
		strcpy(scale_input, argv[inx + 1]);
		if(scale_input == NULL) {
			printf("couldn't get a scale! try again with a "
				"better argument.\n");
			return -1;
		}
		float scale = strtof(scale_input, NULL);
		width = (float)width * scale;
		height = (float)height * scale;
	}

	if(
		(width < 1) ||
		(height < 1)
	) {
		printf(
			"Bad resolution! You tried to render at %dx%d.\n"
			"Cya!\n",
			width, height
		);
		return -1;
	}

	inx =
		searchargs(argc, argv, "-o") &
		searchargs(argc, argv, "--output");
	if(inx != -1) {
		strcpy(outfile_base, argv[inx + 1]);
	} else {
		strcpy(outfile_base, "../output/");
	}

	inx =
		searchargs(argc, argv, "-c") &
		searchargs(argc, argv, "--convert");
	if(inx != -1) {
		convert_immediately = 1;
	} else {
		convert_immediately = 0;
	}

	//set up the window
	if(
		(searchargs(argc, argv, "-w") != -1) ||
		(searchargs(argc, argv, "--window") != -1)
	) {
		//Initialize SDL
		if(SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		} else {
			//Create window
			window = SDL_CreateWindow(
				"I C the Light",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				width, height,
				SDL_WINDOW_SHOWN
			);
			if(window == NULL) {
				printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			} else {
				//Get window surface
				screen = SDL_GetWindowSurface(window);
			}
		}
	} else {
		unsigned int rmask, gmask, bmask, amask;
#define SCREEN_BIT_DEPTH 32
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
#endif
		screen = SDL_CreateRGBSurface(
			0,
			width, height,
			SCREEN_BIT_DEPTH, rmask, gmask, bmask, amask
		);
	}

	if(screen == NULL) {
		printf("Render surface undefined, quitting!\n");
		return -1;
	}

	if(window != NULL) {
		SDL_FillRect(screen, NULL, 0x000000);
		SDL_UpdateWindowSurface(window);
	}

	int quit = 0;
	clock_t start, end;
	double total;
	do {
		start = clock();
		//printf("frame: %d\n", frame);

		//poll for events, and handle the ones we care about.
		//this returns 1 if we need to quit
		quit = handleevents(screen);

		// render
		//if(frame == 0) {
			render(screen, frame);
			saveframe(screen);
		//}

		//Update the surface
		if(window != NULL) {
			SDL_UpdateWindowSurface(window);
		}

		end = clock();
		//if(frame%30 == 0) {
			////if(frame == 0)
				////saveframe(screen);
		//}
		total = (double)(end - start) / CLOCKS_PER_SEC;
		printf("%.4f FPS = %.4f SPF\n", 1 / total, total);
		frame++;
		//if(frame > FRAMES_IN_ROTATION + 1) {
			//quit = 1;
		//}
	} while(!quit);

	printf("I feel free!\n");

	//Destroy window
	if(window != NULL) {
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();

	if(screen != NULL) {
		free(screen);
	}

	printf("Goodbye!\n");

	return 0;
}
