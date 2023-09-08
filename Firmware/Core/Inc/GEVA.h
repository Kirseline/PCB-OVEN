#ifndef __VIDEO_BUFFER__
#define __VIDEO_BUFFER__

#include <stdint.h>
#include <malloc.h>
    
#define X 0
#define Y 1
#define _A 0
#define _B 1
#define _C 2
#define _D 3

enum color {
    CLEAR_C = 0,
    SET_C = 1
};

enum mode {
    NORMAL = 0,
    INVERTED = 1
};

enum orientation {
    LANDSCAPE,              //normal orientation
    LANDSCAPE_INVERTED,
    PORTRAIT,
    PORTRAIT_INVERTED
};

//video buffer structure
typedef struct video_buffer {
    uint8_t size_x;
    uint8_t size_y;

    uint8_t* vid_buf;

    uint8_t byte_col_cnt;
    uint16_t byte_buffer_cnt;

    enum mode mode;

    enum orientation orient;
    uint8_t origin[2];      //reference origin;  origin[] = x, y
    int8_t r_matrix[4];     //rotation matrix |a b|  r_matrix[] = a, b, c, d
                            //                |c d|
}video_buffer;

typedef struct vbc_file {

    struct {
        uint8_t id;
        uint8_t width;
        uint8_t height;
        uint8_t byte_cnt_h;
        uint8_t byte_cnt_l;
    }header;

    uint8_t* body;
}vbc_file;

enum fonts_size {
    SMALL,
    MEDIUM,
    LARGE
};

/* ==========================================CHART VISUALIZATION STRUCT=========================================== */

enum chart_type {
    BAR,
    COL,
    LINE,
    XY,
    SCATTER
};

typedef struct data_container {

    uint8_t o_x, o_y;						//origin coordinates
    enum chart_type type;
    uint8_t dim_x, dim_y_pos, dim_y_neg;	//chart dimension
    float max, min, max_x;					//data max, min value, max_x only used for XY type
    float data;								//chart data
    uint8_t x_division;                     //axis mark divison
    
    struct {             //struct only used for XY type
        uint8_t n_ent;                      //number of entries 
        float* x;                           //make sure x,y contains the same number of elements
        float* y;
    }xy_entries;
    
}data_container;

/* ================================================BASIC FUNCTION================================================= */

uint8_t init_buf(video_buffer* buf, uint8_t size_x, uint8_t size_y);
void set_mode(enum mode mode, video_buffer* buf);
void set_orientation(enum orientation o, video_buffer* buf);
uint8_t clr_buffer(video_buffer* buf);
uint8_t clear_sector(uint8_t pos_x, uint8_t pos_y, uint8_t width, uint8_t height, video_buffer* buf);

uint8_t put_pixel(uint8_t pos_x, uint8_t pos_y, enum color color, video_buffer* buf);
uint8_t put_line(uint8_t pos_x1, uint8_t pos_y1, uint8_t pos_x2, uint8_t pos_y2, video_buffer* buf);

uint8_t set_font(enum fonts_size ff);
uint8_t put_char(uint8_t pos_x, uint8_t pos_y, uint8_t chr, enum fonts_size ff, video_buffer* buf);
uint8_t put_string(uint8_t pos_x, uint8_t pos_y, uint8_t* chr, enum fonts_size ff, video_buffer* buf);

uint8_t put_vbc(vbc_file file, uint8_t x, uint8_t y, video_buffer* buf);

/* ================================================SHAPES========================================================= */
uint8_t put_circle(uint8_t o_x, uint8_t o_y, uint8_t r, video_buffer* buf);
uint8_t put_rect(uint8_t o_x, uint8_t o_y, uint8_t width, uint8_t height, video_buffer* buf);
uint8_t put_triangle(uint8_t a_x, uint8_t a_y, uint8_t b_x, uint8_t b_y, uint8_t c_x, uint8_t c_y, video_buffer* buf);

/* ========================================DATA VISUALIZATION===================================================== */
uint8_t update_chart(float new_data, data_container* c, video_buffer* buf);

#endif