#ifndef __STRUCTS_DATA__
#define __STRUCTS_DATA__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>


#include "lvgl/lvgl.h"
#include <SDL2/SDL.h>
#include "lib_for_user_space/ch36x_lib.h"

#define DEVICE_NAME "/dev/ch36xpci0"

/* parameters of display*/
#define DISP_NAME "Cryptosoft"
#define SIZE_W 1200
#define SIZE_H 800

#ifdef __cplusplus
extern "C"
{
#endif
	extern const lv_font_t roboto_mono;
#ifdef __cplusplus
}
#endif

struct _SHARED_VALUE
{
	uint8_t data;
	size_t offset;
	
	size_t max_size;
};

struct _MOUSE
{
	lv_indev_t* 		mouse_indev;
	lv_indev_t*		encoder_indev;
	int 			encoder_diff;
	bool			encoder_event_pending;

};

struct _KEYBOARD
{
	lv_indev_t*		keyboard_indev;
	bool 			key_event_processed;
	lv_group_t*		group;
	SDL_Keycode      	last_sdl_keycode;
	lv_indev_state_t 	last_key_state;
};

struct _BUFFERS
{
	void* 			buf1;
	void* 			buf2;
	size_t			buf_size;
};


struct _SPI_DATA
{
	// SPI labels
	lv_obj_t*		spi_label_set_offset;
	lv_obj_t*		spi_set_byte_offset;
	lv_obj_t*		spi_label_set_data_byte;
	lv_obj_t*		spi_set_data_byte;

	lv_obj_t*		spi_label_read_byte;
	lv_obj_t*		spi_button_read_byte;
	lv_obj_t*		spi_label_write_byte;
	lv_obj_t*		spi_button_write_byte;
	lv_obj_t*		spi_label_out_read;

	lv_obj_t* 		spi_label_choice_file;
	lv_obj_t*		spi_button_choice_file;
	lv_obj_t*		spi_label_set_offset_for_file;
	lv_obj_t*		spi_set_offset_for_file;

	lv_obj_t*		spi_label_read_in_file;
	lv_obj_t*		spi_button_read_in_file;
	lv_obj_t*		spi_label_write_file;
	lv_obj_t*		spi_button_write_file;
	
	
	lv_obj_t*		spi_button_erase;
	lv_obj_t*		spi_label_erase;
	
	lv_obj_t*		spi_button_change_mode;
	lv_obj_t*		spi_label_change_mode;
	
	lv_obj_t* 		support_label;
	lv_obj_t*		speed_label;
	
	lv_obj_t*		spi_set_max_size;
	lv_obj_t*		spi_max_size_label;
};

struct _CONFIG_DATA
{
	// SPI labels
	lv_obj_t*		config_label_set_offset;
	lv_obj_t*		config_set_byte_offset;
	lv_obj_t*		config_label_set_data_byte;
	lv_obj_t*		config_set_data_byte;

	lv_obj_t*		config_label_read_byte;
	lv_obj_t*		config_button_read_byte;
	lv_obj_t*		config_label_write_byte;
	lv_obj_t*		config_button_write_byte;
	lv_obj_t*		config_label_out_read;

	lv_obj_t* 		config_label_choice_file;
	lv_obj_t*		config_button_choice_file;
	lv_obj_t*		config_label_set_offset_for_file;
	lv_obj_t*		config_set_offset_for_file;

	lv_obj_t*		config_label_read_in_file;
	lv_obj_t*		config_button_read_in_file;
	lv_obj_t*		config_label_write_file;
	lv_obj_t*		config_button_write_file;
	
	lv_obj_t*		config_button_erase;
	lv_obj_t*		config_label_erase;
};

struct _I2C_DATA
{
	// SPI labels
	lv_obj_t*		i2c_label_set_offset;
	lv_obj_t*		i2c_set_byte_offset;
	lv_obj_t*		i2c_label_set_data_byte;
	lv_obj_t*		i2c_set_data_byte;

	lv_obj_t*		i2c_label_read_byte;
	lv_obj_t*		i2c_button_read_byte;
	lv_obj_t*		i2c_label_write_byte;
	lv_obj_t*		i2c_button_write_byte;
	lv_obj_t*		i2c_label_out_read;

	lv_obj_t* 		i2c_label_choice_file;
	lv_obj_t*		i2c_button_choice_file;
	lv_obj_t*		i2c_label_set_offset_for_file;
	lv_obj_t*		i2c_set_offset_for_file;

	lv_obj_t*		i2c_label_read_in_file;
	lv_obj_t*		i2c_button_read_in_file;
	lv_obj_t*		i2c_label_write_file;
	lv_obj_t*		i2c_button_write_file;
	
	lv_obj_t*		i2c_button_erase;
	lv_obj_t*		i2c_label_erase;
};

struct _BUTTONS
{
	lv_obj_t* 		screen;
	struct _SPI_DATA*	spi_data;
	struct _CONFIG_DATA*	config_data;
	struct _I2C_DATA*	i2c_data;
	
	
	lv_obj_t* 		turn_on_driver;
	lv_obj_t* 		turn_on_driver_label;
	
	lv_obj_t*		label_chip_info;
	lv_obj_t*		button_get_chip_info;
	lv_obj_t*		label_get_chip_info;

};


struct _MEDIA_DATA
{
	lv_display_t* 		disp;
	lv_fs_drv_t* 		disp_drv;

	struct _BUFFERS		buffs;
	struct _MOUSE* 		mouse;
	struct _KEYBOARD* 	keyboard;
	struct _BUTTONS*	butts;

	struct _SHARED_VALUE    shared_value;
	
	
	SDL_Window*		window;
	SDL_Event		event;
	SDL_Renderer* 		renderer;
	SDL_Texture*		lvgl_texture;

	char 			file_path[64];

	void 			(*cleanup_data)(struct _MEDIA_DATA* );
	
};

extern void LOG(const char*, ...);
extern int init_display(struct _MEDIA_DATA* );
extern void cleanup_data(struct _MEDIA_DATA* );
extern int init_data(struct _MEDIA_DATA* );
extern long cast_str_to_long(const char* );



/* callbacks declaration function */
extern void sdl_mouse_read_cb(lv_indev_t* indev, lv_indev_data_t* data);
extern void sdl_keyboard_read_cb(lv_indev_t* indev, lv_indev_data_t* data);
extern void my_flush_cb(lv_display_t * disp, const lv_area_t * area,  unsigned char* color_p);
extern void textarea_event_handler_offset(lv_event_t * e);
extern void textarea_event_handler_2char(lv_event_t * e);
extern void textarea_event_handler_max_size(lv_event_t* e);
extern void spi_button_read_byte_handler(lv_event_t* e);
extern void spi_button_write_byte_handler(lv_event_t* e);
extern void spi_button_erase_handler(lv_event_t* e);
extern void spi_button_change_mode_handler(lv_event_t*);
extern void spi_packet_mode_handler(lv_event_t* e);
extern void skip_callback(lv_event_t* e);
extern void spi_packet_write_handler(lv_event_t* e);
extern void spi_packet_read_handler(lv_event_t* e);
extern void spi_packet_erase(lv_event_t* e);


extern void file_choice_handler(lv_event_t* e);
extern void file_read_handler(lv_event_t* e);
extern void file_write_handler(lv_event_t* e);


extern void config_button_read_byte_handler(lv_event_t* e);
extern void config_button_write_byte_handler(lv_event_t* e);

extern void config_read_file_handler(lv_event_t* e);
extern void config_write_file_handler(lv_event_t* e);
extern void config_button_erase_handler(lv_event_t* e);


extern void i2c_button_read_byte_handler(lv_event_t* e);
extern void i2c_button_write_byte_handler(lv_event_t* e);

extern void i2c_read_file_handler(lv_event_t* e);
extern void i2c_write_file_handler(lv_event_t* e);
extern void i2c_button_erase_handler(lv_event_t* e);

extern void turn_on_driver_handler(lv_event_t* e);
extern void turn_off_driver_handler(lv_event_t* e);
extern void get_chip_info(lv_event_t* e);


#endif //__STRUCTS_DATA__



























