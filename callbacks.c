#include "structs_data.h"


void sdl_mouse_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
	(void)indev;
	int mouse_x, mouse_y;
	uint32_t buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	data->point.x = mouse_x;
	data->point.y = mouse_y;

	if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		data->state = LV_INDEV_STATE_PRESSED;
	}
	else
	{
		data->state = LV_INDEV_STATE_RELEASED;

	}
}


static lv_key_t sdl_keycode_to_lvgl(SDL_Keycode sdl_key, SDL_Keymod sdl_mod)
{
	bool is_shift = (sdl_mod & KMOD_SHIFT);


	switch (sdl_key)
	{
		case SDLK_RETURN:
		case SDLK_KP_ENTER:     return LV_KEY_ENTER;
		case SDLK_BACKSPACE:    return LV_KEY_BACKSPACE;
		case SDLK_ESCAPE:       return LV_KEY_ESC;
		case SDLK_DELETE:       return LV_KEY_DEL;
		case SDLK_UP:           return LV_KEY_UP;
		case SDLK_DOWN:         return LV_KEY_DOWN;
		case SDLK_LEFT:         return LV_KEY_LEFT;
		case SDLK_RIGHT:        return LV_KEY_RIGHT;
		case SDLK_HOME:         return LV_KEY_HOME;
		case SDLK_END:          return LV_KEY_END;
		case SDLK_PAGEDOWN:     return LV_KEY_NEXT;
		case SDLK_PAGEUP:       return LV_KEY_PREV;
		case SDLK_TAB:          return is_shift ? LV_KEY_PREV : LV_KEY_NEXT;


		default: break;
	}


	if (sdl_key >= SDLK_SPACE && sdl_key <= SDLK_z)
	{
		if (sdl_key >= SDLK_a && sdl_key <= SDLK_z)
		{
			if (is_shift || (sdl_mod & KMOD_CAPS))
			{
				return (lv_key_t)(sdl_key - SDLK_a + 'A');
			}
			else
			{
				return (lv_key_t)(sdl_key);
			}
		}

		else if (sdl_key >= SDLK_0 && sdl_key <= SDLK_9)
		{
			if (is_shift)
			{
				switch (sdl_key)
				{
					case SDLK_0: return ')';
					case SDLK_1: return '!';
					case SDLK_2: return '@';
					case SDLK_3: return '#';
					case SDLK_4: return '$';
					case SDLK_5: return '%';
					case SDLK_6: return '^';
					case SDLK_7: return '&';
					case SDLK_8: return '*';
					case SDLK_9: return '(';
					default: break;
				}
			}
			else
			{
				return (lv_key_t)(sdl_key);
			}
		}

		else if (is_shift)
		{

			switch (sdl_key)
			{
				case SDLK_BACKQUOTE:    return '~';
				case SDLK_MINUS:        return '_';
				case SDLK_EQUALS:       return '+';
				case SDLK_LEFTBRACKET:  return '{';
				case SDLK_RIGHTBRACKET: return '}';
				case SDLK_BACKSLASH:    return '|';
				case SDLK_SEMICOLON:    return ':';
				case SDLK_QUOTE:        return '"';
				case SDLK_COMMA:        return '<';
				case SDLK_PERIOD:       return '>';
				case SDLK_SLASH:        return '?';
				default: break;

			}
		}
		else
		{

			switch (sdl_key)
			{
				case SDLK_SPACE:        return ' ';
				case SDLK_BACKQUOTE:    return '`';
				case SDLK_MINUS:        return '-';
				case SDLK_EQUALS:       return '=';
				case SDLK_LEFTBRACKET:  return '[';
				case SDLK_RIGHTBRACKET: return ']';
				case SDLK_BACKSLASH:    return '\\';
				case SDLK_SEMICOLON:    return ';';
				case SDLK_QUOTE:        return '\'';
				case SDLK_COMMA:        return ',';
				case SDLK_PERIOD:       return '.';
				case SDLK_SLASH:        return '/';
				default: break;
			}
		}
	}


	return 0;
}


void sdl_keyboard_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{

	(void)indev;

	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_indev_get_user_data(indev);
	if (!media_data->keyboard->key_event_processed)
	{

		SDL_Keymod current_mods = SDL_GetModState();
		data->key = sdl_keycode_to_lvgl(media_data->keyboard->last_sdl_keycode, current_mods);
		data->state = media_data->keyboard->last_key_state;

		media_data->keyboard->key_event_processed = true;

		//LOG("sdl_keyboard_read_cb: SENT Event to LVGL -> key=0x%x ('%c'), state=%s\n",
		    //(unsigned int)data->key,
		    //(data->key >= 32 && data->key <= 126) ? (char)data->key : '?',
		    //(data->state == LV_INDEV_STATE_PRESSED) ? "PRESSED" : "RELEASED");
	}
	else
	{
		SDL_Keymod current_mods = SDL_GetModState();
		data->key = sdl_keycode_to_lvgl(media_data->keyboard->last_sdl_keycode, current_mods);
		data->state = LV_INDEV_STATE_RELEASED;

		if (media_data->keyboard->last_key_state == LV_INDEV_STATE_RELEASED)
		{
			data->key = 0;
		}
	}

	data->continue_reading = false;
}


void my_flush_cb(lv_display_t * disp, const lv_area_t * area,  unsigned char* color_p)
{

	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_display_get_user_data(disp);
	if (!media_data) return;

	SDL_Rect rect = {
		.x = (int)area->x1,
		.y = (int)area->y1,
		.w = (int)lv_area_get_width(area),
		.h = (int)lv_area_get_height(area)
	};


	const int pitch = lv_area_get_width(area) * (LV_COLOR_DEPTH / 8);
	SDL_UpdateTexture(media_data->lvgl_texture, &rect, color_p, pitch);

	lv_display_flush_ready(disp);
}

void textarea_event_handler_max_size(lv_event_t * e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	lv_obj_t * ta = lv_event_get_target(e);
	lv_event_code_t code = lv_event_get_code(e);
	lv_group_t * group = lv_obj_get_group(ta);

	const char* text = lv_textarea_get_text(ta);
	if ((media_data->shared_value.max_size = (size_t)cast_str_to_long(text)) == -1)
	{
		return;
	}
}	


void textarea_event_handler_offset(lv_event_t * e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	lv_obj_t * ta = lv_event_get_target(e);
	lv_event_code_t code = lv_event_get_code(e);
	lv_group_t * group = lv_obj_get_group(ta);

	const char* text = lv_textarea_get_text(ta);
	if ((media_data->shared_value.offset = (size_t)cast_str_to_long(text)) == -1)
	{
		return;
	}
}

void textarea_event_handler_2char(lv_event_t * e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	lv_obj_t * ta = lv_event_get_target(e);
	lv_event_code_t code = lv_event_get_code(e);
	lv_group_t * group = lv_obj_get_group(ta);
	
	if (code == LV_EVENT_VALUE_CHANGED)
	{
		const char* text = lv_textarea_get_text(ta);
		if (strlen(text) > 4)
		{
			char out_text[5];
			strncpy(out_text, text, 5);
			out_text[4] = '\0';
			lv_textarea_set_text(ta, out_text);
			lv_textarea_set_cursor_pos(ta, LV_TEXTAREA_CURSOR_LAST);
			if ((media_data->shared_value.data = (size_t)cast_str_to_long(text)) == -1)
			{
				LOG("invalid data in textarea_event_handler_2char\n");
				return;
			}
		}
	}

}


void spi_button_read_byte_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	LOG("start spi_button_read_byte_handler\n");
	int fd_dev;
	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}
	
	if (media_data->shared_value.offset > media_data->shared_value.max_size)
	{
		LOG("offset > size spi\n");
		return;
	}

	if ((ch36x_flash_read(fd_dev, media_data->shared_value.offset, &media_data->shared_value.data, 1)) != EXIT_SUCCESS)
	{
		LOG("Failed to flash read\n");
		ch36x_close(fd_dev);
		return;
	}


	ch36x_close(fd_dev);
	char out_text[3];
	snprintf(out_text, 3, "%02x", media_data->shared_value.data);
	lv_textarea_set_placeholder_text(media_data->butts->spi_data->spi_set_data_byte, out_text);
	return;

}

void spi_button_write_byte_handler(lv_event_t* e)
{
	LOG("Call spi_button_write_byte_handler\n");
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd_dev;
	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}
	
	if (media_data->shared_value.offset > media_data->shared_value.max_size)
	{
		LOG("offset > size spi\n");
		return;
	}
	
	LOG("Flash lock...\n");
	ch36x_flash_lock(fd_dev, 0);

	LOG("Erasing flash...\n");
	if (ch36x_flash_erase(fd_dev, media_data->shared_value.offset, 1) != EXIT_SUCCESS)
	{
		LOG("Failed to flash erase\n");
		ch36x_close(fd_dev);
		return;
	}

	if (ch36x_flash_write(fd_dev, media_data->shared_value.offset, &media_data->shared_value.data, 1) != EXIT_SUCCESS)
	{
		LOG("Failed to flash write\n");
		ch36x_close(fd_dev);
		return;
	}

	ch36x_close(fd_dev);
	LOG("Writted success!\n");
	return;

}


static void ok_btn_event_cb(lv_event_t* e);
static void input_ta_event_cb(lv_event_t* e);
static void process_input(lv_obj_t* input_ta, lv_obj_t* cont, struct _MEDIA_DATA* media_data);

void file_choice_handler(lv_event_t* e)
{
	LOG("clicked file_choice_handler\n");
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	static lv_style_t style_btn;
	lv_obj_t* cont = lv_obj_create(lv_scr_act());
	lv_obj_set_size(cont, 400, 180);
	lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);


	lv_obj_t* input_ta = lv_textarea_create(cont);
	lv_obj_set_width(input_ta, lv_pct(100));
	lv_textarea_set_one_line(input_ta, true);
	lv_textarea_set_placeholder_text(input_ta, "Enter file path...");


	lv_obj_t* btn = lv_btn_create(cont);
	lv_obj_t* btn_label = lv_label_create(btn);
	lv_label_set_text(btn_label, "OK");
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(btn, &style_btn, 0);


	lv_group_t* g = media_data->keyboard->group;
	lv_group_add_obj(g, input_ta);
	lv_group_add_obj(g, btn);
	lv_group_focus_obj(input_ta);

	struct
	{
		lv_obj_t* cont;
		struct _MEDIA_DATA* media_data;
	} *user_data = malloc(sizeof(*(user_data)));
	user_data->cont = cont;
	user_data->media_data = media_data;

	lv_obj_add_event_cb(btn, ok_btn_event_cb, LV_EVENT_CLICKED, user_data);
	lv_obj_add_event_cb(input_ta, input_ta_event_cb, LV_EVENT_READY, user_data);
}

static void input_ta_event_cb(lv_event_t* e)
{
	struct
	{
		lv_obj_t* cont;
		struct _MEDIA_DATA* media_data;
	} *user_data = (void*)lv_event_get_user_data(e);

	lv_obj_t* cont = user_data->cont;
	lv_obj_t* input_ta = lv_obj_get_child(cont, 0);

	process_input(input_ta, cont, user_data->media_data);
}

static void ok_btn_event_cb(lv_event_t* e)
{

	struct
	{
		lv_obj_t* cont;
		struct _MEDIA_DATA* media_data;
	} *user_data = (void*)lv_event_get_user_data(e);

	lv_obj_t* cont = user_data->cont;
	lv_obj_t* input_ta = lv_obj_get_child(cont, 0);

	process_input(input_ta, cont, user_data->media_data);
	free(user_data);
}

static void process_input(lv_obj_t* input_ta, lv_obj_t* cont, struct _MEDIA_DATA* media_data)
{
	const char* path = lv_textarea_get_text(input_ta);
	LOG("Selected file: %s\n", path);

	if (strlen(path) >= 64)
	{
		lv_obj_del(cont);
		return;
	}

	snprintf(media_data->file_path, 64, "%s", path);
	lv_obj_del(cont);
	return;
}


void file_read_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);

	clock_t start_total, end_total;
	size_t total_bytes_readed = 0;

	int fd_dev;
	FILE* file = fopen(media_data->file_path, "a");
	if (!file)
	{
		LOG("failed open file\n");
		return;
	}

	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		fclose(file);
		return;
	}

	size_t chunk_size = 4096;
	uint8_t* buffer = (uint8_t*)malloc(chunk_size);
	if (!buffer)
	{
		LOG("failed alloc buffer\n");
		goto cleanup_read;
	}

	double total_seconds;
	double total_speed;
	size_t remainder_bytes = chunk_size * 2;
	if (media_data->shared_value.offset > media_data->shared_value.max_size)
	{
		LOG("offset > size spi\n");
		return;
	}
	else if (media_data->shared_value.max_size > chunk_size * 2 + media_data->shared_value.offset)
	{
		remainder_bytes = chunk_size * 2;
	}
	else if (media_data->shared_value.max_size <= chunk_size * 2 + media_data->shared_value.offset)
	{
		remainder_bytes = (chunk_size * 2 + media_data->shared_value.offset) - media_data->shared_value.max_size;
	}
	
	LOG("remainder_bytes: %lu\n", remainder_bytes);
	while(remainder_bytes > 0)
	{
		size_t bytes_to_process = (remainder_bytes > chunk_size) ? chunk_size : remainder_bytes;
		start_total = clock();
		if (!ch36x_flash_read(fd_dev, media_data->shared_value.offset, buffer, bytes_to_process))
		{
			LOG("failed flash read\n");
			goto cleanup_read;
		}
		end_total = clock();


		size_t bytes_write = fwrite(buffer, sizeof(uint8_t), bytes_to_process, file);
		if (bytes_write != bytes_to_process) {LOG("bytes loss in fwrite\n"); goto cleanup_read;}
		remainder_bytes -= bytes_write;
		total_bytes_readed += bytes_write;
		total_seconds += (double)(end_total - start_total) / CLOCKS_PER_SEC;
		total_speed += (bytes_write / 1024.0) / (total_seconds ? total_seconds : 0.001);
		LOG("bytes read %lu bytes and remainder %lu bytes\n", bytes_write, remainder_bytes);
	}

	double avg_speed = (total_bytes_readed / 1024.0) / (total_seconds ? total_seconds : 0.001);
	char out[65];

	snprintf(out, 65, "Total: %zu bytes, Time: %.2f sec, Avg Speed: %.2f KB/s\n",
		 total_bytes_readed, total_seconds, avg_speed);

	lv_label_set_text(media_data->butts->spi_data->speed_label, out);


cleanup_read:
	free(buffer);
	fclose(file);
	ch36x_close(fd_dev);
	return;


}


void file_write_handler(lv_event_t* e) 
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd_dev;
	clock_t start_total, end_total;
	size_t total_bytes_written = 0;
	size_t remainder_bytes;

	FILE* file = fopen(media_data->file_path, "rb");
	if (!file)
	{
		LOG("Failed to open file\n");
		return;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (file_size <= 0)
	{
		LOG("File is empty or invalid size\n");
		fclose(file);
		return;
	}

	const size_t chunk_size = 4096;
	uint8_t* buffer = (uint8_t*)malloc(chunk_size);
	if (!buffer) 
	{
		LOG("Failed to allocate buffer\n");
		fclose(file);
		return;
	}

	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0) 
	{
		LOG("Failed to open device\n");
		free(buffer);
		fclose(file);
		return;
	}
	
	
	if (media_data->shared_value.offset > media_data->shared_value.max_size)
	{
		LOG("offset > size spi\n");
		return;
	}
	else if (media_data->shared_value.max_size > file_size + media_data->shared_value.offset)
	{
		remainder_bytes = file_size;
	}
	else if (media_data->shared_value.max_size <= file_size + media_data->shared_value.offset)
	{
		remainder_bytes = (file_size + media_data->shared_value.offset) - media_data->shared_value.max_size;
	}
	
	LOG("remainder_bytes: %lu\n", remainder_bytes);

	LOG("Flash lock...\n");
	ch36x_flash_lock(fd_dev, 0);

	LOG("Erasing flash...\n");
	if (ch36x_flash_erase(fd_dev, media_data->shared_value.offset, file_size) != EXIT_SUCCESS)
	{
		LOG("Failed to erase flash\n");
		goto cleanup;
	}

	LOG("Writing data...\n");
	double total_seconds;
	double total_speed;

	while (remainder_bytes > 0) 
	{
	
		size_t bytes_to_process = (remainder_bytes > chunk_size) ? chunk_size : remainder_bytes;
		size_t bytes_read = fread(buffer, 1, bytes_to_process, file);

		if (bytes_read != bytes_to_process)
		{
			LOG("Read error: expected %zu, got %zu bytes\n", bytes_to_process, bytes_read);
			goto cleanup;
		}

		start_total = clock();
		if (ch36x_flash_write(fd_dev, media_data->shared_value.offset, buffer, bytes_read) != EXIT_SUCCESS)
		{
			LOG("Failed to write flash\n");
			goto cleanup;
		}
		end_total = clock();

		total_seconds += (double)(end_total - start_total) / CLOCKS_PER_SEC;
		total_speed += (bytes_read / 1024.0) / (total_seconds ? total_seconds : 0.001);


		total_bytes_written += bytes_read;
		media_data->shared_value.offset += bytes_read;
		remainder_bytes -= bytes_read;
	}

	double avg_speed = (total_bytes_written / 1024.0) / (total_seconds ? total_seconds : 0.001);
	char out[65];

	snprintf(out, 65, "Total: %zu bytes, Time: %.2f sec, Avg Speed: %.2f KB/s\n",
	    total_bytes_written, total_seconds, avg_speed);

	lv_label_set_text(media_data->butts->spi_data->speed_label, out);

	cleanup:
	free(buffer);
	fclose(file);
	ch36x_close(fd_dev);
}




void config_button_read_byte_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	
	int fd = -1;
	uint8_t max_size_config = 255;
	LOG("data: %02x\n", media_data->shared_value.data);

	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size config mem)", media_data->shared_value.offset, max_size_config);
		goto cleanup;
	}

	fd = ch36x_open(DEVICE_NAME);
	if (fd < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		goto cleanup;
	}
 
	if (ch36x_read_config_byte(fd, media_data->shared_value.offset, &media_data->shared_value.data) != EXIT_SUCCESS)
	{
		LOG("Failed read device | offset: %02x.\n", media_data->shared_value.offset);
		goto cleanup;
	}

	char out_text[3];
	snprintf(out_text, 3, "%02x", media_data->shared_value.data);
	LOG("out_text: %s\ndata: %02x\n", out_text, media_data->shared_value.data);
	lv_textarea_set_placeholder_text(media_data->butts->config_data->config_set_data_byte, out_text);

cleanup:
	if (fd >= 0) ch36x_close(fd);
	return;
}

void config_button_write_byte_handler(lv_event_t* e)
{
	LOG("Start config_button_write_byte_handler\n");
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd = -1;
	uint8_t max_size_config = 255;
	LOG("data: %02x\n", media_data->shared_value.data);

	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size config mem)", media_data->shared_value.offset, max_size_config);
		goto cleanup;
	}
	
	fd = ch36x_open(DEVICE_NAME);
	if (fd < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		goto cleanup;
	}

	if (ch36x_write_config_byte(fd, media_data->shared_value.offset, media_data->shared_value.data) != EXIT_SUCCESS)
	{
		LOG("Failed write device | offset: %02x.\n", media_data->shared_value.offset);
		goto cleanup;
	}
	LOG("Writted %02x on %02x addr\n", media_data->shared_value.data, media_data->shared_value.offset);

cleanup:
	if (fd >= 0) ch36x_close(fd);
	return;
}


void config_read_file_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	FILE* file = NULL;
	uint8_t* buffer = NULL;
	int fd = -1;
	uint8_t max_size_config = 255;
	uint8_t bytes_to_read = max_size_config;

	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size config mem)", media_data->shared_value.offset, max_size_config);
		goto cleanup;
	}

	file = fopen(media_data->file_path, "a");
	if (!file)
	{
		LOG("Не удалось открыть файл для записи\n");
		goto cleanup;
	}

	buffer = (uint8_t*)malloc(bytes_to_read);
	if (!buffer)
	{
		LOG("Не удалось выделить память для буфера\n");
		goto cleanup;
	}

	fd = ch36x_open(DEVICE_NAME);
	if (fd < 0)
	{
		LOG("Не удалось открыть устройство ch36x (возможно, нужно загрузить модуль ядра)\n");
		goto cleanup;
	}

	LOG("Чтение %ld байт из чипа...\n", bytes_to_read);
	for (long i = 0; i < bytes_to_read; ++i)
	{
		if (ch36x_read_config_byte(fd, (uint8_t)(i), &buffer[i]) != EXIT_SUCCESS)
		{
			LOG("Ошибка при чтении байта со смещения %ld.\n", i);
			goto cleanup;
		}
	}

	if (fwrite(buffer, 1, bytes_to_read, file) != (size_t)bytes_to_read)
	{
		LOG("Ошибка: Не удалось записать %ld байт в файл.\n", bytes_to_read);
		goto cleanup;
	}


cleanup:
	if (file) fclose(file);
	if (buffer) free(buffer);
	if (fd >= 0) ch36x_close(fd);
	return;
}

void config_write_file_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	FILE* file = NULL;
	uint8_t* buffer = NULL;
	int fd = -1;
	uint8_t max_size_config = 255;


	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size config mem)", media_data->shared_value.offset, max_size_config);
		goto cleanup;
	}

	file = fopen(media_data->file_path, "rb");
	if (!file)
	{
		LOG("Не удалось открыть файл для чтения\n");
		goto cleanup;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (file_size <= 0)
	{
		LOG("Ошибка: Файл пуст или не удалось определить его размер.\n");
		goto cleanup;
	}

	long bytes_to_write = file_size;
	long available_space = max_size_config - media_data->shared_value.offset;


	if (bytes_to_write > available_space)
	{
		LOG("Предупреждение: Размер файла (%ld байт) больше доступного места в чипе (%ld байт).\n", bytes_to_write, available_space);
		LOG("Будут записаны только первые %ld байт.\n", available_space);
		bytes_to_write = available_space;
	}


	buffer = (uint8_t*)malloc(bytes_to_write);
	if (!buffer)
	{
		LOG("Не удалось выделить память для буфера\n");
		goto cleanup;
	}


	if (fread(buffer, sizeof(uint8_t), bytes_to_write, file) != (size_t)bytes_to_write)
	{
		LOG("Ошибка: Не удалось прочитать %ld байт из файла.\n", bytes_to_write);
		goto cleanup;
	}

	fd = ch36x_open(DEVICE_NAME);
	if (fd < 0)
	{
		LOG("Не удалось открыть устройство ch36x (возможно, нужно загрузить модуль ядра)\n");
		goto cleanup;
	}

	LOG("Запись %ld байт в чип...\n", bytes_to_write);
	for (long i = 0; i < bytes_to_write; ++i)
	{
		if (ch36x_write_config_byte(fd, (uint8_t)(media_data->shared_value.offset + i), buffer[i]) != EXIT_SUCCESS)
		{
			LOG("Ошибка при записи байта на смещении %ld.\n", media_data->shared_value.offset + i);
			goto cleanup;
		}
	}

cleanup:
	if (file) fclose(file);
	if (buffer) free(buffer);
	if (fd >= 0) ch36x_close(fd);
	return;
}



void i2c_button_read_byte_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	lv_obj_t * ta = lv_event_get_target(e);
	lv_event_code_t code = lv_event_get_code(e);
	lv_group_t * group = lv_obj_get_group(ta);
	
	int fd;
	uint8_t max_addr = 255;
	uint8_t i2c_addr = 0;

	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size i2c mem)", media_data->shared_value.offset, max_addr);
		return;
	}


	if ((fd = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}

	if (ch36x_i2c_read(fd, i2c_addr, (uint8_t)media_data->shared_value.offset, &media_data->shared_value.data, sizeof(uint8_t)) != EXIT_SUCCESS)
	{
		LOG("Failed read data!\noffset: %02x\naddr_i2c: %02x\ndata: %02x\n", (uint8_t)media_data->shared_value.offset, i2c_addr, media_data->shared_value.data);
		return;
	}

	char out_text[3];
	snprintf(out_text, 3, "%02x", media_data->shared_value.data);
	lv_textarea_set_placeholder_text(media_data->butts->i2c_data->i2c_set_data_byte, out_text);
	ch36x_close(fd);
	return;

}


void i2c_button_write_byte_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd;
	uint8_t max_addr = 255;
	uint8_t i2c_addr = 0x0;

	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size i2c mem)", media_data->shared_value.offset, max_addr);
		return;
	}


	if ((fd = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}

	if (ch36x_i2c_write(fd, i2c_addr, (uint8_t)media_data->shared_value.offset, &media_data->shared_value.data, sizeof(uint8_t)) != EXIT_SUCCESS)
	{
		LOG("Failed write data!\noffset: %02x\naddr_i2c: %02x\ndata: %02x\n", (uint8_t)media_data->shared_value.offset, i2c_addr, media_data->shared_value.data);
		return;
	}

	ch36x_close(fd);
	return;
}

void i2c_read_file_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd;
	uint8_t max_addr = 255;
	uint8_t i2c_addr = 0x0;

	FILE* file = fopen(media_data->file_path, "w");
	if (!file)
	{
		LOG("Не удалось открыть файл для записи\n");
		goto cleanup;
	}

	if ((fd = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		goto cleanup;
	}

	uint8_t bytes_to_read = max_addr;
	uint8_t* buffer = (uint8_t*)malloc(bytes_to_read);
	if (!buffer)
	{
		LOG("Не удалось выделить память для буфера");
		goto cleanup;
	}


	if (ch36x_i2c_read(fd, i2c_addr, (uint8_t)0, buffer, bytes_to_read) != EXIT_SUCCESS)
	{
		LOG("Failed write data!\noffset: %02x\naddr_i2c: %02x\nbuffer: %p\n", (uint8_t)0, i2c_addr, buffer);
		goto cleanup;
	}


	if (fwrite(buffer, sizeof(uint8_t), bytes_to_read, file) != (uint8_t)bytes_to_read)
	{
		LOG("Ошибка: Не удалось записать %ld байт из файла.\n", bytes_to_read);
		goto cleanup;
	}

cleanup:
	if (file) fclose(file);
	if (buffer) free(buffer);
	if (fd >= 0) ch36x_close(fd);
	return;

}


void i2c_write_file_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd;
	uint8_t max_addr = 255;
	uint8_t i2c_addr = 0x0;

	if (media_data->shared_value.offset > 255)
	{
		LOG("offset: %lu > %02d (size i2c mem)", media_data->shared_value.offset, max_addr);
		goto cleanup;
	}


	FILE* file = fopen(media_data->file_path, "r");
	if (!file)
	{
		LOG("Не удалось открыть файл для чтения\n");
		goto cleanup;
	}

	if ((fd = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		goto cleanup;
	}

	uint8_t bytes_to_write = max_addr;
	uint8_t* buffer = (uint8_t*)malloc(bytes_to_write);
	if (!buffer)
	{
		LOG("Не удалось выделить память для буфера");
		goto cleanup;
	}

	if (fread(buffer, sizeof(uint8_t), bytes_to_write, file) != (uint8_t)bytes_to_write)
	{
		LOG("Ошибка: Не удалось записать %ld байт из файла.\n", bytes_to_write);
		goto cleanup;
	}


	if (ch36x_i2c_read(fd, i2c_addr, (uint8_t)media_data->shared_value.offset, buffer, bytes_to_write) != EXIT_SUCCESS)
	{
		LOG("Failed write data!\noffset: %02x\naddr_i2c: %02x\nbuffer: %p\n", (uint8_t)media_data->shared_value.offset, i2c_addr, buffer);
		goto cleanup;
	}


cleanup:
	if (file) fclose(file);
	if (buffer) free(buffer);
	if (fd >= 0) ch36x_close(fd);
	return;
}


void turn_on_driver_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	system("sudo insmod ch36x.ko");
	lv_label_set_text(media_data->butts->turn_on_driver_label, "Loaded");
	LOG("Module loaded\n");
	return;
}

void turn_off_driver_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	system("sudo rmmod ch36x.ko");
	lv_label_set_text(media_data->butts->turn_on_driver_label, "TURN ON");
	lv_obj_remove_flag(media_data->butts->button_get_chip_info, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->label_chip_info, LV_OBJ_FLAG_HIDDEN);
	LOG("Module unloaded\n");
	return;
}


void spi_button_erase_handler(lv_event_t* e) 
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	size_t size = 64 * 1024;
	int fd_dev;
	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}

	LOG("Flash lock...\n");
	ch36x_flash_lock(fd_dev, 0);

	LOG("Erasing flash...\n");
	if (ch36x_flash_erase(fd_dev, media_data->shared_value.offset, size) != EXIT_SUCCESS)
	{
		LOG("Failed to flash erase\n");
		ch36x_close(fd_dev);
		return;
	}
	LOG("End erasing\n");
	ch36x_close(fd_dev);
	return;
}


void config_button_erase_handler(lv_event_t* e) 
{
	LOG("Start config_button_erase_handler\n");
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd_dev;
	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}
	
	size_t size = 256;
	for (size_t index = 0; index < size; ++index)
	{
		//LOG("%lu\n", index);
		if (!ch36x_write_config_byte(fd_dev, index, 0xff))
		{
			LOG("Ошибка при записи байта на смещении %ld.\n", index);
			ch36x_close(fd_dev);
			return;
		}
	}
	
	ch36x_close(fd_dev);
	return;
}
void i2c_button_erase_handler(lv_event_t* e)
{
	LOG("Start i2c_button_erase_handler\n");
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	int fd_dev;
	uint8_t i2c_addr = 0x50;
	if ((fd_dev = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		return;
	}
	
	size_t size = 256;
	uint8_t ch[1] = {0xff};
	for (size_t index = 0; index < size; ++index)
	{
		if (!ch36x_i2c_write(fd_dev, i2c_addr, index, ch, size))
		{
			LOG("Failed write data!\noffset: %02x\naddr_i2c: %02x\nbuffer: %p\n", index, i2c_addr, &ch);
			ch36x_close(fd_dev);
			return;
		}
	}
	
	ch36x_close(fd_dev);
	return;
}

static void set_style(lv_style_t* style, lv_obj_t* btn)
{
	//lv_obj_remove_style_all(btn);
	lv_obj_add_style(btn , style, 0);
}



void spi_packet_mode_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	lv_label_set_text(media_data->butts->spi_data->spi_label_change_mode, "BASE MODE");

	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_change_mode, spi_packet_mode_handler);
	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_write_byte, spi_packet_write_handler);
	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_read_byte, spi_packet_read_handler);
	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_erase, spi_packet_erase);

	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_change_mode, spi_button_change_mode_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_write_byte, spi_button_write_byte_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_read_byte, spi_button_read_byte_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_erase, spi_button_erase_handler, LV_EVENT_CLICKED, (void*)media_data);

	/*
	lv_obj_remove_flag(media_data->butts->spi_data->spi_button_choice_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->spi_label_choice_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->spi_button_write_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->spi_label_write_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->spi_label_read_in_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->spi_button_read_in_file, LV_OBJ_FLAG_HIDDEN);

	lv_obj_remove_flag(media_data->butts->spi_data->spi_label_set_offset_for_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->spi_set_offset_for_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->spi_data->speed_label, LV_OBJ_FLAG_HIDDEN);

	lv_obj_add_flag(media_data->butts->spi_data->support_label, LV_OBJ_FLAG_HIDDEN);
	*/

	size_t color = 0x007F00;
	static lv_style_t green_color;
	lv_style_init(&green_color);
	lv_style_set_bg_color(&green_color, lv_color_hex(color));
	lv_style_set_border_width(&green_color, 2);
	lv_style_set_border_color(&green_color, lv_color_black());

	set_style(&green_color, media_data->butts->spi_data->spi_button_read_byte);
	set_style(&green_color, media_data->butts->spi_data->spi_button_write_byte);
	set_style(&green_color, media_data->butts->spi_data->spi_button_erase);

	set_style(&green_color, media_data->butts->spi_data->spi_button_choice_file);
	set_style(&green_color, media_data->butts->spi_data->spi_button_read_in_file);
	set_style(&green_color, media_data->butts->spi_data->spi_button_write_file);

}

void spi_button_change_mode_handler(lv_event_t* e)
{
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	lv_label_set_text(media_data->butts->spi_data->spi_label_change_mode, "PACKET MODE");

	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_change_mode, spi_button_change_mode_handler);
	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_write_byte, spi_button_write_byte_handler);
	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_read_byte, spi_button_read_byte_handler);
	lv_obj_remove_event_cb(media_data->butts->spi_data->spi_button_erase, spi_button_erase_handler);

	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_change_mode, spi_packet_mode_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_write_byte, spi_packet_write_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_read_byte, spi_packet_read_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_erase, spi_packet_erase, LV_EVENT_CLICKED, (void*)media_data);

	/*
	lv_obj_add_flag(media_data->butts->spi_data->spi_button_choice_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_label_choice_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_button_write_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_label_write_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_button_read_in_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_label_read_in_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_label_set_offset_for_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->spi_set_offset_for_file, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(media_data->butts->spi_data->speed_label, LV_OBJ_FLAG_HIDDEN);


	lv_obj_remove_flag(media_data->butts->spi_data->support_label, LV_OBJ_FLAG_HIDDEN);
	*/

	size_t color = 0x43699C;
	static lv_style_t orange_color;
	lv_style_init(&orange_color);
	lv_style_set_bg_color(&orange_color, lv_color_hex(color));
	lv_style_set_border_width(&orange_color, 2);
	lv_style_set_border_color(&orange_color, lv_color_black());

	set_style(&orange_color, media_data->butts->spi_data->spi_button_read_byte);
	set_style(&orange_color, media_data->butts->spi_data->spi_button_write_byte);
	set_style(&orange_color, media_data->butts->spi_data->spi_button_erase);

	set_style(&orange_color, media_data->butts->spi_data->spi_button_choice_file);
	set_style(&orange_color, media_data->butts->spi_data->spi_button_read_in_file);
	set_style(&orange_color, media_data->butts->spi_data->spi_button_write_file);
}


void skip_callback(lv_event_t* e)
{
	LOG("Callback skip\n");
}

void spi_packet_write_handler(lv_event_t* e)
{
	LOG("Call spi_packet_write_handler\n");
}

void spi_packet_read_handler(lv_event_t* e)
{
	LOG("Call spi_packet_read_handler\n");
}

void spi_packet_erase(lv_event_t* e)
{
	LOG("Call spi_packet_erase\n");
}



void get_chip_info(lv_event_t* e) 
{
	LOG("Call get_chip_info\n");
	struct _MEDIA_DATA* media_data = (struct _MEDIA_DATA*)lv_event_get_user_data(e);
	
	int fd = -1;
	if ((fd = ch36x_open(DEVICE_NAME)) < 0)
	{
		LOG("failed device open, maybe not insert module. try sudo insmod ch36x.ko \n");
		goto cleanup;
	}
	lv_obj_add_flag(media_data->butts->button_get_chip_info, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(media_data->butts->label_chip_info, LV_OBJ_FLAG_HIDDEN);
	
	enum CHIP_TYPE type;
	
	if (ch36x_get_chiptype(fd, &type) != EXIT_SUCCESS)
	{
		LOG("failed get chip type\n");
		goto cleanup;
	}
	
	char version[30];
	if (ch36x_get_version(fd, version) != EXIT_SUCCESS)
	{
		LOG("failed get chip version\n");
		goto cleanup;
	}
	
	int irq_number;
	
	if (ch36x_get_irq(fd, &irq_number) != EXIT_SUCCESS)
	{
		LOG("failed get irq number\n");
		goto cleanup;
	}
	
	
	void* ioaddr = NULL;
	if (ch36x_get_ioaddr(fd, ioaddr) != EXIT_SUCCESS)
	{
		LOG("failed get ioaddr\n");
		goto cleanup;
	}
	
	void* memaddr = NULL;
	if (ch36x_get_memaddr(fd, memaddr) != EXIT_SUCCESS)
	{
		LOG("failed get memaddr\n");
		goto cleanup;
	}
	
	char out[512];
	switch (type)
	{
		case CHIP_CH365:
			snprintf(out, 512, "CHIP INFO:\nVendor id: %04x        |    Device id: %04x\nChip type: %s    |    Version: %s\nIrq number: %d         |     I/O address: %p\nMemory address: %p",
			CH365_VID, CH365_DID, "CH365", version, irq_number, ioaddr, memaddr);
			lv_label_set_text(media_data->butts->label_chip_info, out);
			break;
		case CHIP_CH367:
			snprintf(out, 512, "CHIP INFO:\nVendor id: %04x        |    Device id: %04x\nChip type: %s    |    Version: %s\nIrq number: %d         |     I/O address: %p\nMemory address: %p",
			CH367_VID, CH367_DID_SID_LOW, "CH367", version, irq_number, ioaddr, memaddr);
			lv_label_set_text(media_data->butts->label_chip_info, out);
			break;
		case CHIP_CH368:
			snprintf(out, 512, "CHIP INFO:\nVendor id: %04x        |    Device id: %04x\nChip type: %s    |    Version: %s\nIrq number: %d         |     I/O address: %p\nMemory address: %p",
			CH368_VID, CH368_DID, "CH368", version, irq_number, ioaddr, memaddr);
			lv_label_set_text(media_data->butts->label_chip_info, out);
			break;				
		
	}
	
	ch36x_close(fd);
	return;	
cleanup:
	ch36x_close(fd);
	return;	
}





























































