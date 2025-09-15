#include "structs_data.h"


void LOG(const char* format, ...)
{
#ifdef DEBUG
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
#endif
}


void cleanup_data(struct _MEDIA_DATA* media_data)
{

	if (media_data)
	{
		if (media_data->buffs.buf1)
		{
			free(media_data->buffs.buf1);
			media_data->buffs.buf1 = NULL;
		}

		if (media_data->buffs.buf2)
		{
			free(media_data->buffs.buf2);
			media_data->buffs.buf2 = NULL;
		}

	}

	if (media_data)
	{

		if (media_data->renderer)
		{
			SDL_DestroyRenderer(media_data->renderer);
			SDL_DestroyTexture(media_data->lvgl_texture);
			media_data->renderer = NULL;
		}


		if (media_data->window)
		{
			SDL_DestroyWindow(media_data->window);
			media_data->window = NULL;
		}
		if (media_data && media_data->mouse->mouse_indev)
		{
			lv_indev_delete(media_data->mouse->mouse_indev);
			media_data->mouse->mouse_indev = NULL;
		}

		if (media_data && media_data->keyboard->keyboard_indev)
		{
			lv_indev_delete(media_data->keyboard->keyboard_indev);
			media_data->keyboard->keyboard_indev = NULL;
		}



		free(media_data);
		media_data = NULL;
	}

	SDL_Quit();

}



int init_data(struct _MEDIA_DATA* media_data)
{
	memset(media_data, 0, sizeof(struct _MEDIA_DATA));

	media_data->mouse = (struct _MOUSE*)calloc(1, sizeof(struct _MOUSE));
	if (!media_data->mouse)
	{
		LOG("failed alloc mouse\n");
		return EXIT_FAILURE;
	}

	media_data->keyboard = (struct _KEYBOARD*)calloc(1, sizeof(struct _KEYBOARD));
	if (!media_data->keyboard)
	{
		free(media_data->mouse);
		LOG("failed alloc keyboard\n");
		return EXIT_FAILURE;
	}

	media_data->butts = (struct _BUTTONS*)calloc(1, sizeof(struct _BUTTONS));
	if (!media_data->butts)
	{
		free(media_data->keyboard);
		free(media_data->mouse);
		LOG("failed alloc butts\n");
		return EXIT_FAILURE;
	}


	media_data->butts->spi_data = (struct _SPI_DATA*)calloc(1, sizeof(struct _SPI_DATA));
	media_data->butts->config_data = (struct _CONFIG_DATA*)calloc(1, sizeof(struct _CONFIG_DATA));
	media_data->butts->i2c_data = (struct _I2C_DATA*)calloc(1, sizeof(struct _I2C_DATA));
	if (!media_data->butts->spi_data || !media_data->butts->config_data || !media_data->butts->i2c_data)
	{
		if (media_data->butts->spi_data) free(media_data->butts->spi_data);
		if (media_data->butts->config_data) free(media_data->butts->config_data);
		if (media_data->butts->i2c_data) free(media_data->butts->i2c_data);
		free(media_data->butts);
		free(media_data->keyboard);
		free(media_data->mouse);
		LOG("failed alloc butts data\n");
		return EXIT_FAILURE;
	}

	media_data->buffs.buf_size = SIZE_W * SIZE_H;
	media_data->buffs.buf1 = calloc(media_data->buffs.buf_size, sizeof(lv_color_t));
	media_data->buffs.buf2 = calloc(media_data->buffs.buf_size, sizeof(lv_color_t));
	if (!media_data->buffs.buf1 || !media_data->buffs.buf2)
	{
		if (media_data->buffs.buf1) free(media_data->buffs.buf1);
		if (media_data->buffs.buf2) free(media_data->buffs.buf2);
		free(media_data->butts->spi_data);
		free(media_data->butts->config_data);
		free(media_data->butts->i2c_data);
		free(media_data->butts);
		free(media_data->keyboard);
		free(media_data->mouse);
		LOG("failed alloc buffers\n");
		return EXIT_FAILURE;
	}


	media_data->shared_value.data = 0;
	media_data->shared_value.offset = 0;

	media_data->cleanup_data = cleanup_data;
	return EXIT_SUCCESS;
}


long cast_str_to_long(const char* str)
{
	if (!str)
	{
		return -1;
	}

	if (strncmp(str, "0x", 2) != 0 && strncmp(str, "0X", 2) != 0)
	{
		//LOG("Error format: expected hex number starting with 0x.\n");
		return -1;
	}

	char* endptr;
	errno = 0;
	long data = strtol(str, &endptr, 16);

	if (errno != 0)
	{
		LOG("Conversion error: %s\n", strerror(errno));
		return -1;
	}

	if (*endptr != '\0')
	{
		//LOG("Error format: invalid characters after hex number.\n");
		return -1;
	}
	//LOG("result cast_str_to_long\nstr: %s\nlong: %lu\n", str, data);
	return data;
}





