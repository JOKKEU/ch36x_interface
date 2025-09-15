#include "structs_data.h"



int main(int argc, char** argv)
{
	struct _MEDIA_DATA* media_data;
	media_data = (struct _MEDIA_DATA*)malloc(sizeof(struct _MEDIA_DATA));
	if (!media_data)
	{
		LOG("failed alloc media_data\n");
		return EXIT_FAILURE;
	}

	if (init_data(media_data) != EXIT_SUCCESS)
	{
		LOG("failed in init_data func\n");
		return EXIT_FAILURE;
	}

	if (init_display(media_data) != EXIT_SUCCESS)
	{
		LOG("failed init display\n");

		goto cleanup;
	}

	int running = true;
	uint32_t last_tick = SDL_GetTicks();
	while (running)
	{;
		bool key_event_this_iteration = false;

		while (SDL_PollEvent(&media_data->event))
		{


			if (media_data->event.type == SDL_QUIT)
			{
				running = false;
			}

			else if (media_data->event.type == SDL_KEYDOWN)
			{

				media_data->keyboard->last_sdl_keycode = media_data->event.key.keysym.sym;
				media_data->keyboard->last_key_state = LV_INDEV_STATE_PRESSED;
				media_data->keyboard->key_event_processed = false;
				//LOG("SDL_KEYDOWN: sym=0x%x (%s), state=PRESSED, processed=false\n",
				    //media_data->event.key.keysym.sym, SDL_GetKeyName(media_data->event.key.keysym.sym));
			}
			else if (media_data->event.type == SDL_KEYUP)
			{

				media_data->keyboard->last_sdl_keycode = media_data->event.key.keysym.sym;
				media_data->keyboard->last_key_state = LV_INDEV_STATE_RELEASED;
				media_data->keyboard->key_event_processed = false;
				//LOG("SDL_KEYUP: sym=0x%x (%s), state=RELEASED, processed=false\n",
				    //media_data->event.key.keysym.sym, SDL_GetKeyName(media_data->event.key.keysym.sym));
			}
		}

		uint32_t time_elapsed = SDL_GetTicks() - last_tick;
		last_tick = SDL_GetTicks();
		lv_tick_inc(time_elapsed);
		lv_timer_handler(); // <- segfault
		SDL_SetRenderDrawColor(media_data->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(media_data->renderer);


		if (media_data->lvgl_texture)
		{
			SDL_RenderCopy(media_data->renderer, media_data->lvgl_texture, NULL, NULL);
		}

		SDL_RenderPresent(media_data->renderer);
		SDL_Delay(5);
		//LOG("1\n");
	}

	media_data->cleanup_data(media_data);
	return EXIT_SUCCESS;

cleanup:
	if (media_data)
	{
		media_data->cleanup_data(media_data);
	}

	return EXIT_FAILURE;

}
















