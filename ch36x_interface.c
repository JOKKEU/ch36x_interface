#include "structs_data.h"

static uint8_t get_average_color(lv_color_t color);

int init_display(struct _MEDIA_DATA* media_data)
{
	if (!media_data)
	{
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		LOG("SDL Initialization failed: %s\n", SDL_GetError());
		goto exit;
	}

	media_data->window = SDL_CreateWindow(
		DISP_NAME,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SIZE_W,
		SIZE_H,
		SDL_WINDOW_SHOWN
	);

	media_data->renderer = SDL_CreateRenderer(media_data->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!media_data->renderer)
	{
		LOG("error create renderer: %s\n", SDL_GetError());
		goto exit;
	}

	Uint32 sdl_pixel_format;

	if (LV_COLOR_DEPTH == 32)
	{
		sdl_pixel_format = SDL_PIXELFORMAT_ARGB8888;
	}
	else if (LV_COLOR_DEPTH == 16)
	{
		sdl_pixel_format = SDL_PIXELFORMAT_RGB565;
	}
	else if (LV_COLOR_DEPTH == 24)
	{
		sdl_pixel_format = SDL_PIXELFORMAT_RGB24;
	}
	else
	{
		LOG("Unsupported LV_COLOR_DEPTH: %d\n", LV_COLOR_DEPTH);
		goto exit;
	}

	media_data->lvgl_texture = SDL_CreateTexture(
		media_data->renderer,
		sdl_pixel_format,
		SDL_TEXTUREACCESS_STREAMING,
		SIZE_W,
		SIZE_H
	);

	if (!media_data->lvgl_texture)
	{
		LOG("Failed to create LVGL texture: %s\n", SDL_GetError());
		goto exit;
	}

	lv_init();

	media_data->disp = lv_display_create(SIZE_W, SIZE_H);
	if (!media_data->disp)
	{
		LOG("Failed to create LVGL display\n");
		goto exit;
	}



	LOG("Initializing LVGL input device...\n");
	media_data->mouse->mouse_indev = lv_indev_create();
	if (!media_data->mouse->mouse_indev)
	{
		LOG("Failed to create input device\n");
		goto exit;
	}


	lv_indev_set_type(media_data->mouse->mouse_indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(media_data->mouse->mouse_indev, sdl_mouse_read_cb);
	lv_indev_set_display(media_data->mouse->mouse_indev, media_data->disp);
	LOG("Mouse input device initialized.\n");


	media_data->keyboard->keyboard_indev = lv_indev_create();
	if (!media_data->keyboard->keyboard_indev)
	{
		LOG("Failed to create input device\n");
		goto exit;
	}

	lv_indev_set_type(media_data->keyboard->keyboard_indev, LV_INDEV_TYPE_KEYPAD);

	lv_indev_set_read_cb(media_data->keyboard->keyboard_indev, sdl_keyboard_read_cb);
	lv_indev_set_display(media_data->keyboard->keyboard_indev, media_data->disp);


	lv_indev_set_user_data(media_data->keyboard->keyboard_indev, (void*)media_data);

	media_data->keyboard->group = lv_group_create();
	if (!media_data->keyboard->group)
	{
		LOG("failed to create input group\n");
		goto exit;
	}

	lv_group_set_default(media_data->keyboard->group);
	lv_indev_set_group(media_data->keyboard->keyboard_indev, media_data->keyboard->group);
	LOG("Keyboard input device initialized.\n");


	lv_display_set_buffers(media_data->disp,
			       media_data->buffs.buf1,
			       media_data->buffs.buf2,
			       media_data->buffs.buf_size * sizeof(lv_color_t),
			       LV_DISPLAY_RENDER_MODE_PARTIAL
			       );

	lv_display_set_flush_cb(media_data->disp, my_flush_cb);
	lv_display_set_user_data(media_data->disp, (void*)media_data);

	media_data->butts->screen = lv_screen_active();
	static lv_style_t style_btn;


	/* PANEL */
	// ===================================================================================
	/* MAIN CONTAINER - вертикальное расположение */
	lv_obj_t * main_container = lv_obj_create(lv_scr_act());
	lv_obj_set_size(main_container, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
	lv_obj_set_flex_flow(main_container, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	// Вычисляем высоту для каждой строки (треть экрана минус отступы)
	lv_coord_t row_height = (lv_disp_get_ver_res(NULL) - 20) / 4; // 20 - суммарные отступы

	// ===================================================================================
	/* TOP ROW */
	lv_obj_t * top_row = lv_obj_create(main_container);
	lv_obj_set_size(top_row, lv_pct(100), row_height);
	lv_obj_set_flex_flow(top_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_all(top_row, 5, 0);

	// Левая колонка (50%)
	lv_obj_t * top_left_col = lv_obj_create(top_row);
	lv_obj_set_size(top_left_col, lv_pct(50), lv_pct(100));
	lv_obj_set_style_pad_all(top_left_col, 10, 0);

	// Правая колонка (50%)
	lv_obj_t * top_right_col = lv_obj_create(top_row);
	lv_obj_set_size(top_right_col, lv_pct(50), lv_pct(100));
	lv_obj_set_style_pad_all(top_right_col, 10, 0);

	// ===================================================================================
	/* MID ROW */
	lv_obj_t * mid_row = lv_obj_create(main_container);
	lv_obj_set_size(mid_row, lv_pct(100), row_height);
	lv_obj_set_flex_flow(mid_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_all(mid_row, 5, 0);

	// Левая колонка (50%)
	lv_obj_t * mid_left_col = lv_obj_create(mid_row);
	lv_obj_set_size(mid_left_col, lv_pct(50), lv_pct(100));
	lv_obj_set_style_pad_all(mid_left_col, 10, 0);

	// Правая колонка (50%)
	lv_obj_t * mid_right_col = lv_obj_create(mid_row);
	lv_obj_set_size(mid_right_col, lv_pct(50), lv_pct(100));
	lv_obj_set_style_pad_all(mid_right_col, 10, 0);

	// ===================================================================================
	/* BOT ROW */
	lv_obj_t * bot_row = lv_obj_create(main_container);
	lv_obj_set_size(bot_row, lv_pct(100), row_height);
	lv_obj_set_flex_flow(bot_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_all(bot_row, 5, 0);

	// Левая колонка (50%)
	lv_obj_t * bot_left_col = lv_obj_create(bot_row);
	lv_obj_set_size(bot_left_col, lv_pct(50), lv_pct(100));
	lv_obj_set_style_pad_all(bot_left_col, 10, 0);

	// Правая колонка (50%)
	lv_obj_t * bot_right_col = lv_obj_create(bot_row);
	lv_obj_set_size(bot_right_col, lv_pct(50), lv_pct(100));
	lv_obj_set_style_pad_all(bot_right_col, 10, 0);


	/*
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 */
	/* SPI DATA */

	// offset + data + size
	media_data->butts->spi_data->spi_label_set_offset = lv_label_create(media_data->butts->screen);
	lv_label_set_text( media_data->butts->spi_data->spi_label_set_offset, "SPI Offset:");
	lv_obj_align( media_data->butts->spi_data->spi_label_set_offset, LV_ALIGN_TOP_LEFT, 30, 40);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_set_offset, &roboto_mono, 0);

	media_data->butts->spi_data->spi_set_byte_offset = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->spi_data->spi_set_byte_offset, true);
	lv_textarea_set_placeholder_text(media_data->butts->spi_data->spi_set_byte_offset, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->spi_data->spi_set_byte_offset, 150);
	lv_obj_align_to(media_data->butts->spi_data->spi_set_byte_offset,  media_data->butts->spi_data->spi_label_set_offset, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

	media_data->butts->spi_data->spi_label_set_data_byte = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->spi_data->spi_label_set_data_byte, "SPI Data:");
	lv_obj_align_to(media_data->butts->spi_data->spi_label_set_data_byte,  media_data->butts->spi_data->spi_label_set_offset, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
	
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_set_data_byte, &roboto_mono, 0);
	media_data->butts->spi_data->spi_set_data_byte = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->spi_data->spi_set_data_byte, true);
	lv_textarea_set_placeholder_text(media_data->butts->spi_data->spi_set_data_byte, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->spi_data->spi_set_data_byte, 150);
	lv_obj_align_to(media_data->butts->spi_data->spi_set_data_byte, media_data->butts->spi_data->spi_label_set_data_byte, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	
	
	media_data->butts->spi_data->spi_max_size_label = lv_label_create(media_data->butts->screen);
	lv_label_set_text( media_data->butts->spi_data->spi_max_size_label, "SPI size:");
	lv_obj_align( media_data->butts->spi_data->spi_max_size_label, LV_ALIGN_TOP_LEFT, 30, 140);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_max_size_label, &roboto_mono, 0);

	media_data->butts->spi_data->spi_set_max_size = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->spi_data->spi_set_max_size, true);
	lv_textarea_set_placeholder_text(media_data->butts->spi_data->spi_set_max_size, "0x... | default: 8192");
	lv_obj_set_width(media_data->butts->spi_data->spi_set_max_size, 150);
	lv_obj_align_to(media_data->butts->spi_data->spi_set_max_size,  media_data->butts->spi_data->spi_max_size_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);


	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_set_max_size);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_set_byte_offset);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_set_data_byte);

	lv_obj_add_event_cb(media_data->butts->spi_data->spi_set_byte_offset, textarea_event_handler_offset, LV_EVENT_ALL, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_set_data_byte, textarea_event_handler_2char, LV_EVENT_ALL,(void* )media_data);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_set_max_size, textarea_event_handler_max_size, LV_EVENT_ALL,(void* )media_data);


	// btn write
	media_data->butts->spi_data->spi_button_write_byte = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_write_byte, 80, 30);
	lv_obj_align(media_data->butts->spi_data->spi_button_write_byte, LV_ALIGN_TOP_MID, -200, 30);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_write_byte, spi_button_write_byte_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->spi_data->spi_button_write_byte, &style_btn, 0);

	media_data->butts->spi_data->spi_label_write_byte = lv_label_create(media_data->butts->spi_data->spi_button_write_byte);
	lv_label_set_text( media_data->butts->spi_data->spi_label_write_byte, "WRITE");
	lv_obj_align( media_data->butts->spi_data->spi_label_write_byte, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_write_byte, &roboto_mono, 0);
	lv_obj_center(media_data->butts->spi_data->spi_label_write_byte);


	// btn READ
	media_data->butts->spi_data->spi_button_read_byte = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_read_byte, 80, 30);
	lv_obj_align(media_data->butts->spi_data->spi_button_read_byte, LV_ALIGN_TOP_MID, -200, 80);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_read_byte, spi_button_read_byte_handler, LV_EVENT_CLICKED, (void*)media_data);

	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->spi_data->spi_button_read_byte, &style_btn, 0);

	media_data->butts->spi_data->spi_label_read_byte = lv_label_create(media_data->butts->spi_data->spi_button_read_byte);
	lv_label_set_text(media_data->butts->spi_data->spi_label_read_byte, "READ");
	lv_obj_center(media_data->butts->spi_data->spi_label_read_byte);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_read_byte, &roboto_mono, 0);



	/* FILE OPERATIONS SECTION - Right Panel */

	// Label and button for file selection
	media_data->butts->spi_data->spi_label_choice_file = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->spi_data->spi_label_choice_file, "Select File:");
	lv_obj_align_to(media_data->butts->spi_data->spi_label_choice_file,
			media_data->butts->spi_data->spi_label_set_offset,
			LV_ALIGN_OUT_RIGHT_MID,
			460, 0);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_choice_file, &roboto_mono, 0);

	media_data->butts->spi_data->spi_button_choice_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_choice_file, 120, 30);
	lv_obj_align_to(media_data->butts->spi_data->spi_button_choice_file,
			media_data->butts->spi_data->spi_label_choice_file,
			LV_ALIGN_OUT_RIGHT_MID,
			10, 0);
	lv_obj_add_style(media_data->butts->spi_data->spi_button_choice_file, &style_btn, 0);

	lv_obj_t* file_choice_label = lv_label_create(media_data->butts->spi_data->spi_button_choice_file);
	lv_label_set_text(file_choice_label, "Browse...");
	lv_obj_center(file_choice_label);
	lv_obj_set_style_text_font(file_choice_label, &roboto_mono, 0);

	// FileToSpi button
	media_data->butts->spi_data->spi_button_write_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_write_file, 120, 30);
	lv_obj_align_to(media_data->butts->spi_data->spi_button_write_file,
			media_data->butts->spi_data->spi_button_choice_file,
			LV_ALIGN_OUT_RIGHT_MID,
			40, 0);
	lv_obj_add_style(media_data->butts->spi_data->spi_button_write_file, &style_btn, 0);

	media_data->butts->spi_data->spi_label_write_file = lv_label_create(media_data->butts->spi_data->spi_button_write_file);
	lv_label_set_text(media_data->butts->spi_data->spi_label_write_file, "FileToSpi");
	lv_obj_center(media_data->butts->spi_data->spi_label_write_file);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_write_file, &roboto_mono, 0);

	// Offset for file operations
	media_data->butts->spi_data->spi_label_set_offset_for_file = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->spi_data->spi_label_set_offset_for_file, "File Offset:");
	lv_obj_align_to(media_data->butts->spi_data->spi_label_set_offset_for_file,
			media_data->butts->spi_data->spi_label_choice_file,
			LV_ALIGN_OUT_BOTTOM_LEFT,
			0, 30);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_set_offset_for_file, &roboto_mono, 0);

	media_data->butts->spi_data->spi_set_offset_for_file = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->spi_data->spi_set_offset_for_file, true);
	lv_textarea_set_placeholder_text(media_data->butts->spi_data->spi_set_offset_for_file, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->spi_data->spi_set_offset_for_file, 150);
	lv_obj_align_to(media_data->butts->spi_data->spi_set_offset_for_file,
			media_data->butts->spi_data->spi_label_set_offset_for_file,
			LV_ALIGN_OUT_RIGHT_MID,
			10, 0);

	// SpiToFile button
	media_data->butts->spi_data->spi_button_read_in_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_read_in_file, 120, 30);
	lv_obj_align_to(media_data->butts->spi_data->spi_button_read_in_file,
			media_data->butts->spi_data->spi_set_offset_for_file,
			LV_ALIGN_OUT_RIGHT_MID,
			10, 0);
	lv_obj_add_style(media_data->butts->spi_data->spi_button_read_in_file, &style_btn, 0);

	media_data->butts->spi_data->spi_label_read_in_file = lv_label_create(media_data->butts->spi_data->spi_button_read_in_file);
	lv_label_set_text(media_data->butts->spi_data->spi_label_read_in_file, "SpiToFile");
	lv_obj_center(media_data->butts->spi_data->spi_label_read_in_file);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_read_in_file, &roboto_mono, 0);

	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_set_offset_for_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_button_choice_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_button_read_in_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->spi_data->spi_button_write_file);


	lv_obj_add_event_cb(media_data->butts->spi_data->spi_set_offset_for_file,
			    textarea_event_handler_offset,
			    LV_EVENT_ALL,
			    (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_choice_file,
			    file_choice_handler,
			    LV_EVENT_CLICKED,
			    (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_read_in_file,
			    file_read_handler,
			    LV_EVENT_CLICKED,
			    (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_write_file,
			    file_write_handler,
			    LV_EVENT_CLICKED,
			    (void*)media_data);
			    
			    
			    
	// ERASE
	media_data->butts->spi_data->spi_button_erase = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_erase, 80, 30);
	lv_obj_align(media_data->butts->spi_data->spi_button_erase, LV_ALIGN_TOP_MID, -100, 55);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_erase, spi_button_erase_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->spi_data->spi_button_erase , &style_btn, 0);

	media_data->butts->spi_data->spi_label_erase = lv_label_create(media_data->butts->spi_data->spi_button_erase);
	lv_label_set_text(media_data->butts->spi_data->spi_label_erase, "ERASE");
	lv_obj_align(media_data->butts->spi_data->spi_label_erase, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_erase, &roboto_mono, 0);
	lv_obj_center(media_data->butts->spi_data->spi_label_erase);
	
	
	
	// CHANGE MODE
	media_data->butts->spi_data->spi_button_change_mode = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->spi_data->spi_button_change_mode, 160, 45);
	lv_obj_align(media_data->butts->spi_data->spi_button_change_mode, LV_ALIGN_TOP_MID, -100, 140);
	lv_obj_add_event_cb(media_data->butts->spi_data->spi_button_change_mode, spi_button_change_mode_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->spi_data->spi_button_change_mode , &style_btn, 0);

	media_data->butts->spi_data->spi_label_change_mode = lv_label_create(media_data->butts->spi_data->spi_button_change_mode);
	lv_label_set_text(media_data->butts->spi_data->spi_label_change_mode, "BASE MODE");
	lv_obj_align(media_data->butts->spi_data->spi_label_change_mode, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->spi_data->spi_label_change_mode, &roboto_mono, 0);
	lv_obj_center(media_data->butts->spi_data->spi_label_change_mode);


	// SUPPORT PACKET MODE
	media_data->butts->spi_data->support_label = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->spi_data->support_label, "UNSUPPORTED");
	lv_obj_align(media_data->butts->spi_data->support_label,
			LV_ALIGN_OUT_RIGHT_MID,
			830, 100);
	lv_obj_set_style_text_font(media_data->butts->spi_data->support_label, &roboto_mono, 0);
	lv_obj_add_flag(media_data->butts->spi_data->support_label, LV_OBJ_FLAG_HIDDEN);
	

	// SPEED
	media_data->butts->spi_data->speed_label = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->spi_data->speed_label, "SPI SPEED: START FileToSpi/SpiToFile");
	lv_obj_align(media_data->butts->spi_data->speed_label,
		     LV_ALIGN_OUT_RIGHT_MID,
		     620, 150);
	//lv_obj_set_style_text_font(media_data->butts->spi_data->speed_label, &roboto_mono, 0);


	/*
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 */
	// CONFIG

	// offset + data
	media_data->butts->config_data->config_label_set_offset = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->config_data->config_label_set_offset, "CONFIG Offset:");
	lv_obj_align( media_data->butts->config_data->config_label_set_offset, LV_ALIGN_TOP_LEFT, 30, 260);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_set_offset, &roboto_mono, 0);

	media_data->butts->config_data->config_set_byte_offset = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->config_data->config_set_byte_offset, true);
	lv_textarea_set_placeholder_text(media_data->butts->config_data->config_set_byte_offset, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->config_data->config_set_byte_offset, 130);
	lv_obj_align_to(media_data->butts->config_data->config_set_byte_offset, media_data->butts->config_data->config_label_set_offset, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

	media_data->butts->config_data->config_label_set_data_byte = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->config_data->config_label_set_data_byte , "CONFIG Data:");
	lv_obj_align_to(media_data->butts->config_data->config_label_set_data_byte ,  media_data->butts->config_data->config_label_set_offset, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 40);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_set_data_byte , &roboto_mono, 0);
	media_data->butts->config_data->config_set_data_byte = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->config_data->config_set_data_byte, true);
	lv_textarea_set_placeholder_text(media_data->butts->config_data->config_set_data_byte, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->config_data->config_set_data_byte, 150);
	lv_obj_align_to(media_data->butts->config_data->config_set_data_byte, media_data->butts->config_data->config_label_set_data_byte , LV_ALIGN_OUT_RIGHT_MID, 10, 0);

	lv_group_add_obj(media_data->keyboard->group, media_data->butts->config_data->config_set_byte_offset);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->config_data->config_set_data_byte);

	lv_obj_add_event_cb(media_data->butts->config_data->config_set_byte_offset, textarea_event_handler_offset, LV_EVENT_ALL, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->config_data->config_set_data_byte, textarea_event_handler_2char, LV_EVENT_ALL,(void* )media_data);


	// btn WRITE
	media_data->butts->config_data->config_button_write_byte = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->config_data->config_button_write_byte, 80, 30);
	lv_obj_align(media_data->butts->config_data->config_button_write_byte, LV_ALIGN_TOP_MID, -200, 250);
	lv_obj_add_event_cb(media_data->butts->config_data->config_button_write_byte, config_button_write_byte_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->config_data->config_button_write_byte, &style_btn, 0);

	media_data->butts->config_data->config_label_write_byte = lv_label_create(media_data->butts->config_data->config_button_write_byte);
	lv_label_set_text( media_data->butts->config_data->config_label_write_byte, "WRITE");
	lv_obj_align( media_data->butts->config_data->config_label_write_byte, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_write_byte, &roboto_mono, 0);
	lv_obj_center(media_data->butts->config_data->config_label_write_byte);


	// btn READ
	media_data->butts->config_data->config_button_read_byte = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->config_data->config_button_read_byte, 80, 30);
	lv_obj_align(media_data->butts->config_data->config_button_read_byte, LV_ALIGN_TOP_MID, -200, 300);
	lv_obj_add_event_cb(media_data->butts->config_data->config_button_read_byte, config_button_read_byte_handler, LV_EVENT_CLICKED, (void*)media_data);

	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->config_data->config_button_read_byte, &style_btn, 0);

	media_data->butts->config_data->config_label_read_byte = lv_label_create(media_data->butts->config_data->config_button_read_byte);
	lv_label_set_text(media_data->butts->config_data->config_label_read_byte, "READ");
	lv_obj_center(media_data->butts->config_data->config_label_read_byte);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_read_byte, &roboto_mono, 0);





	/* FILE OPERATIONS SECTION - Right Panel */

	// Label and button for file selection
	media_data->butts->config_data->config_label_choice_file = lv_label_create(media_data->butts->screen);
	lv_label_set_text (media_data->butts->config_data->config_label_choice_file, "Select File:");
	lv_obj_align_to(media_data->butts->config_data->config_label_choice_file,
			media_data->butts->config_data->config_label_set_offset,
			LV_ALIGN_OUT_RIGHT_MID,
			425, 0);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_choice_file, &roboto_mono, 0);

	media_data->butts->config_data->config_button_choice_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->config_data->config_button_choice_file, 120, 30);
	lv_obj_align_to(media_data->butts->config_data->config_button_choice_file,
			media_data->butts->config_data->config_label_choice_file,
			LV_ALIGN_OUT_RIGHT_MID,
			10, 0);
	lv_obj_add_style(media_data->butts->config_data->config_button_choice_file, &style_btn, 0);

	lv_obj_t* config_file_choice_label = lv_label_create(media_data->butts->config_data->config_button_choice_file);
	lv_label_set_text(config_file_choice_label, "Browse...");
	lv_obj_center(config_file_choice_label);
	lv_obj_set_style_text_font(config_file_choice_label, &roboto_mono, 0);

	// FileToSpi button
	media_data->butts->config_data->config_button_write_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->config_data->config_button_write_file, 120, 30);
	lv_obj_align_to(media_data->butts->config_data->config_button_write_file,
			media_data->butts->config_data->config_button_choice_file,
			LV_ALIGN_OUT_RIGHT_MID,
			40, 0);
	lv_obj_add_style(media_data->butts->config_data->config_button_write_file, &style_btn, 0);

	media_data->butts->config_data->config_label_write_file = lv_label_create(media_data->butts->config_data->config_button_write_file);
	lv_label_set_text(media_data->butts->config_data->config_label_write_file , "FileToCFG");
	lv_obj_center(media_data->butts->config_data->config_label_write_file );
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_write_file , &roboto_mono, 0);

	// Offset for file operations
	media_data->butts->config_data->config_label_set_offset_for_file = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->config_data->config_label_set_offset_for_file, "File Offset:");
	lv_obj_align_to(media_data->butts->config_data->config_label_set_offset_for_file,
			media_data->butts->config_data->config_label_choice_file,
			LV_ALIGN_OUT_BOTTOM_LEFT,
			0, 30);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_set_offset_for_file, &roboto_mono, 0);

	media_data->butts->config_data->config_set_offset_for_file = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->config_data->config_set_offset_for_file, true);
	lv_textarea_set_placeholder_text(media_data->butts->config_data->config_set_offset_for_file, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->config_data->config_set_offset_for_file, 150);
	lv_obj_align_to(media_data->butts->config_data->config_set_offset_for_file,
			media_data->butts->config_data->config_label_set_offset_for_file,
			LV_ALIGN_OUT_RIGHT_MID,
			10, 0);

	// SpiToFile button
	media_data->butts->config_data->config_button_read_in_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->config_data->config_button_read_in_file, 120, 30);
	lv_obj_align_to(media_data->butts->config_data->config_button_read_in_file,
			media_data->butts->config_data->config_set_offset_for_file,
		 LV_ALIGN_OUT_RIGHT_MID,
		 10, 0);
	lv_obj_add_style(media_data->butts->config_data->config_button_read_in_file, &style_btn, 0);

	media_data->butts->config_data->config_label_read_in_file = lv_label_create(media_data->butts->config_data->config_button_read_in_file);
	lv_label_set_text(media_data->butts->config_data->config_label_read_in_file, "CFGToFile");
	lv_obj_center(media_data->butts->config_data->config_label_read_in_file);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_read_in_file, &roboto_mono, 0);

	lv_group_add_obj(media_data->keyboard->group, media_data->butts->config_data->config_set_offset_for_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->config_data->config_button_choice_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->config_data->config_button_read_in_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->config_data->config_button_write_file);


	lv_obj_add_event_cb(media_data->butts->config_data->config_set_offset_for_file,
			    textarea_event_handler_offset,
		     LV_EVENT_ALL,
		     (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->config_data->config_button_choice_file,
			    file_choice_handler,
		     LV_EVENT_CLICKED,
		     (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->config_data->config_button_read_in_file,
			    config_read_file_handler,
		     LV_EVENT_CLICKED,
		     (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->config_data->config_button_write_file,
			    config_write_file_handler,
		     LV_EVENT_CLICKED,
		     (void*)media_data);
		     
		     
		     
	// ERASE
	media_data->butts->config_data->config_button_erase = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->config_data->config_button_erase, 80, 30);
	lv_obj_align(media_data->butts->config_data->config_button_erase, LV_ALIGN_TOP_MID, -100, 275);
	lv_obj_add_event_cb(media_data->butts->config_data->config_button_erase, config_button_erase_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->config_data->config_button_erase , &style_btn, 0);

	media_data->butts->config_data->config_label_erase = lv_label_create(media_data->butts->config_data->config_button_erase);
	lv_label_set_text(media_data->butts->config_data->config_label_erase, "ERASE");
	lv_obj_align(media_data->butts->config_data->config_label_erase, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->config_data->config_label_erase, &roboto_mono, 0);
	lv_obj_center(media_data->butts->config_data->config_label_erase);

	/*
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 */

	// I2C


	// offset + data 
	media_data->butts->i2c_data->i2c_label_set_offset = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_set_offset, "I2C Offset:");
	lv_obj_align(media_data->butts->i2c_data->i2c_label_set_offset, LV_ALIGN_TOP_LEFT, 30, 470);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_set_offset, &roboto_mono, 0);

	media_data->butts->i2c_data->i2c_set_byte_offset = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->i2c_data->i2c_set_byte_offset, true);
	lv_textarea_set_placeholder_text(media_data->butts->i2c_data->i2c_set_byte_offset, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->i2c_data->i2c_set_byte_offset, 130);
	lv_obj_align_to(media_data->butts->i2c_data->i2c_set_byte_offset, media_data->butts->i2c_data->i2c_label_set_offset, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

	media_data->butts->i2c_data->i2c_label_set_data_byte = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_set_data_byte , "I2C Data:");
	lv_obj_align_to(media_data->butts->i2c_data->i2c_label_set_data_byte, media_data->butts->i2c_data->i2c_label_set_offset, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 40);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_set_data_byte , &roboto_mono, 0);
	media_data->butts->i2c_data->i2c_set_data_byte = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->i2c_data->i2c_set_data_byte, true);
	lv_textarea_set_placeholder_text(media_data->butts->i2c_data->i2c_set_data_byte, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->i2c_data->i2c_set_data_byte, 150);
	lv_obj_align_to(media_data->butts->i2c_data->i2c_set_data_byte, media_data->butts->i2c_data->i2c_label_set_data_byte, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

	lv_group_add_obj(media_data->keyboard->group, media_data->butts->i2c_data->i2c_set_byte_offset);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->i2c_data->i2c_set_data_byte);

	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_set_byte_offset, textarea_event_handler_offset, LV_EVENT_ALL, (void*)media_data);
	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_set_data_byte, textarea_event_handler_2char, LV_EVENT_ALL,(void* )media_data);


	// btn WRITE
	media_data->butts->i2c_data->i2c_button_write_byte = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->i2c_data->i2c_button_write_byte, 80, 30);
	lv_obj_align(media_data->butts->i2c_data->i2c_button_write_byte, LV_ALIGN_TOP_MID, -200, 460);
	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_button_write_byte, i2c_button_write_byte_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->i2c_data->i2c_button_write_byte, &style_btn, 0);

	media_data->butts->i2c_data->i2c_label_write_byte = lv_label_create(media_data->butts->i2c_data->i2c_button_write_byte);
	lv_label_set_text( media_data->butts->i2c_data->i2c_label_write_byte, "WRITE");
	lv_obj_align( media_data->butts->i2c_data->i2c_label_write_byte, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_write_byte, &roboto_mono, 0);
	lv_obj_center(media_data->butts->i2c_data->i2c_label_write_byte);


	// btn READ
	media_data->butts->i2c_data->i2c_button_read_byte = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->i2c_data->i2c_button_read_byte, 80, 30);
	lv_obj_align(media_data->butts->i2c_data->i2c_button_read_byte, LV_ALIGN_TOP_MID, -200, 510);
	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_button_read_byte, i2c_button_read_byte_handler, LV_EVENT_CLICKED, (void*)media_data);

	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->i2c_data->i2c_button_read_byte, &style_btn, 0);

	media_data->butts->i2c_data->i2c_label_read_byte = lv_label_create(media_data->butts->i2c_data->i2c_button_read_byte);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_read_byte, "READ");
	lv_obj_center(media_data->butts->i2c_data->i2c_label_read_byte);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_read_byte, &roboto_mono, 0);




	/* FILE OPERATIONS SECTION - Right Panel */

	// Label and button for file selection
	media_data->butts->i2c_data->i2c_label_choice_file = lv_label_create(media_data->butts->screen);
	lv_label_set_text (media_data->butts->i2c_data->i2c_label_choice_file, "Select File:");
	lv_obj_align_to(media_data->butts->i2c_data->i2c_label_choice_file,
			media_data->butts->i2c_data->i2c_label_set_offset,
		 LV_ALIGN_OUT_RIGHT_MID,
		 460, 0);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_choice_file, &roboto_mono, 0);

	media_data->butts->i2c_data->i2c_button_choice_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->i2c_data->i2c_button_choice_file, 120, 30);
	lv_obj_align_to(media_data->butts->i2c_data->i2c_button_choice_file,
			media_data->butts->i2c_data->i2c_label_choice_file,
		 LV_ALIGN_OUT_RIGHT_MID,
		 10, 0);
	lv_obj_add_style(media_data->butts->i2c_data->i2c_button_choice_file, &style_btn, 0);

	lv_obj_t* i2c_file_choice_label = lv_label_create(media_data->butts->i2c_data->i2c_button_choice_file);
	lv_label_set_text(i2c_file_choice_label, "Browse...");
	lv_obj_center(i2c_file_choice_label);
	lv_obj_set_style_text_font(i2c_file_choice_label, &roboto_mono, 0);

	// FileToSpi button
	media_data->butts->i2c_data->i2c_button_write_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->i2c_data->i2c_button_write_file, 120, 30);
	lv_obj_align_to(media_data->butts->i2c_data->i2c_button_write_file,
			media_data->butts->i2c_data->i2c_button_choice_file,
		 LV_ALIGN_OUT_RIGHT_MID,
		 40, 0);
	lv_obj_add_style(media_data->butts->i2c_data->i2c_button_write_file, &style_btn, 0);

	media_data->butts->i2c_data->i2c_label_write_file = lv_label_create(media_data->butts->i2c_data->i2c_button_write_file);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_write_file, "FileToI2C");
	lv_obj_center(media_data->butts->i2c_data->i2c_label_write_file);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_write_file, &roboto_mono, 0);

	// Offset for file operations
	media_data->butts->i2c_data->i2c_label_set_offset_for_file = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_set_offset_for_file, "File Offset:");
	lv_obj_align_to(media_data->butts->i2c_data->i2c_label_set_offset_for_file,
			media_data->butts->i2c_data->i2c_label_choice_file,
		 LV_ALIGN_OUT_BOTTOM_LEFT,
		 0, 30);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_set_offset_for_file, &roboto_mono, 0);

	media_data->butts->i2c_data->i2c_set_offset_for_file = lv_textarea_create(media_data->butts->screen);
	lv_textarea_set_one_line(media_data->butts->i2c_data->i2c_set_offset_for_file, true);
	lv_textarea_set_placeholder_text(media_data->butts->i2c_data->i2c_set_offset_for_file, "0x... | default: 0");
	lv_obj_set_width(media_data->butts->i2c_data->i2c_set_offset_for_file, 150);
	lv_obj_align_to(media_data->butts->i2c_data->i2c_set_offset_for_file,
			media_data->butts->i2c_data->i2c_label_set_offset_for_file,
		 LV_ALIGN_OUT_RIGHT_MID,
		 10, 0);

	// SpiToFile button
	media_data->butts->i2c_data->i2c_button_read_in_file = lv_btn_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->i2c_data->i2c_button_read_in_file, 120, 30);
	lv_obj_align_to(media_data->butts->i2c_data->i2c_button_read_in_file,
			media_data->butts->i2c_data->i2c_set_offset_for_file,
		 LV_ALIGN_OUT_RIGHT_MID,
		 10, 0);
	lv_obj_add_style(media_data->butts->i2c_data->i2c_button_read_in_file, &style_btn, 0);

	media_data->butts->i2c_data->i2c_label_read_in_file = lv_label_create(media_data->butts->i2c_data->i2c_button_read_in_file);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_read_in_file, "I2CToFile");
	lv_obj_center(media_data->butts->i2c_data->i2c_label_read_in_file);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_read_in_file, &roboto_mono, 0);

	lv_group_add_obj(media_data->keyboard->group, media_data->butts->i2c_data->i2c_set_offset_for_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->i2c_data->i2c_button_choice_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->i2c_data->i2c_button_read_in_file);
	lv_group_add_obj(media_data->keyboard->group, media_data->butts->i2c_data->i2c_button_write_file);


	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_set_offset_for_file,
			    textarea_event_handler_offset,
		     LV_EVENT_ALL,
		     (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_button_choice_file,
			    file_choice_handler,
		     LV_EVENT_CLICKED,
		     (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_button_read_in_file,
			    i2c_read_file_handler,
		     LV_EVENT_CLICKED,
		     (void*)media_data);

	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_button_write_file,
			    i2c_write_file_handler,
		     LV_EVENT_CLICKED,
		     (void*)media_data);
		     
		     
		     
	// ERASE
	media_data->butts->i2c_data->i2c_button_erase = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->i2c_data->i2c_button_erase, 80, 30);
	lv_obj_align(media_data->butts->i2c_data->i2c_button_erase, LV_ALIGN_TOP_MID, -100, 485);
	lv_obj_add_event_cb(media_data->butts->i2c_data->i2c_button_erase, i2c_button_erase_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->i2c_data->i2c_button_erase , &style_btn, 0);

	media_data->butts->i2c_data->i2c_label_erase = lv_label_create(media_data->butts->i2c_data->i2c_button_erase);
	lv_label_set_text(media_data->butts->i2c_data->i2c_label_erase, "ERASE");
	lv_obj_align(media_data->butts->i2c_data->i2c_label_erase, LV_ALIGN_TOP_LEFT, 20, 50);
	lv_obj_set_style_text_font(media_data->butts->i2c_data->i2c_label_erase, &roboto_mono, 0);
	lv_obj_center(media_data->butts->i2c_data->i2c_label_erase);	     


	/*
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 * ============================================================================================================
	 */
	// TURN ON DRIVER

	media_data->butts->turn_on_driver = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->turn_on_driver , 200, 75);
	lv_obj_align(media_data->butts->turn_on_driver , LV_ALIGN_TOP_MID, -300, 650);
	lv_obj_add_event_cb(media_data->butts->turn_on_driver , turn_on_driver_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->turn_on_driver , &style_btn, 0);


	media_data->butts->turn_on_driver_label = lv_label_create(media_data->butts->turn_on_driver);
	lv_label_set_text(media_data->butts->turn_on_driver_label, "TURN ON");
	lv_obj_center(media_data->butts->turn_on_driver_label);
	lv_obj_set_style_text_font(media_data->butts->turn_on_driver_label, &roboto_mono, 0);


	// TURN OFF DRIVER

	lv_obj_t* turn_off_driver = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(turn_off_driver, 200, 75);
	lv_obj_align(turn_off_driver, LV_ALIGN_TOP_MID, 300, 650);
	lv_obj_add_event_cb(turn_off_driver, turn_off_driver_handler, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(turn_off_driver, &style_btn, 0);


	lv_obj_t* turn_off_driver_label = lv_label_create(turn_off_driver);
	lv_label_set_text(turn_off_driver_label, "TURN OFF");
	lv_obj_center(turn_off_driver_label);
	lv_obj_set_style_text_font(turn_off_driver_label, &roboto_mono, 0);
	
	
	
	// INFO
	media_data->butts->label_chip_info = lv_label_create(media_data->butts->screen);
	lv_label_set_text(media_data->butts->label_chip_info, "CHIP INFO");
	lv_obj_align(media_data->butts->label_chip_info, LV_ALIGN_TOP_MID, 50, 620);
	lv_obj_set_size(media_data->butts->label_chip_info, 400, LV_SIZE_CONTENT);
	//lv_obj_set_style_text_font(media_data->butts->label_chip_info, &roboto_mono, 0);
	lv_obj_add_flag(media_data->butts->label_chip_info, LV_OBJ_FLAG_HIDDEN);

	
	media_data->butts->button_get_chip_info = lv_button_create(media_data->butts->screen);
	lv_obj_set_size(media_data->butts->button_get_chip_info  , 200, 75);
	lv_obj_align(media_data->butts->button_get_chip_info , LV_ALIGN_TOP_MID, 0, 650);
	lv_obj_add_event_cb(media_data->butts->button_get_chip_info  , get_chip_info, LV_EVENT_CLICKED, (void*)media_data);
	lv_style_init(&style_btn);
	lv_style_set_bg_color(&style_btn, lv_color_hex(0x007F00));
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_color(&style_btn, lv_color_black());
	lv_obj_add_style(media_data->butts->button_get_chip_info , &style_btn, 0);


	media_data->butts->label_get_chip_info = lv_label_create(media_data->butts->button_get_chip_info);
	lv_label_set_text(media_data->butts->label_get_chip_info, "GET INFO");
	lv_obj_center(media_data->butts->label_get_chip_info);
	lv_obj_set_style_text_font(media_data->butts->label_get_chip_info, &roboto_mono, 0);
	

	LOG("Display initialized successfully\n");
	return EXIT_SUCCESS;


exit:
	LOG("Display initialization failed\n");
	return EXIT_FAILURE;
}








































