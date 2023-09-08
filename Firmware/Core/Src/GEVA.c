#include <string.h>
#include "GEVA.h"
#include "GEVA_font.h"

/*
* TO-DO : better error handling
*/

//internal function
uint8_t byte_col_cnt(uint8_t size_x);
uint16_t buffer_byte_cnt(video_buffer* buf);
uint16_t get_buffer_index(uint8_t pos_x, uint8_t pos_y, uint8_t row_byte_cnt);
uint8_t* normalize_pos(uint8_t pos_x, uint8_t pos_y, video_buffer* buf);
uint16_t* rotate_char(uint16_t *chr, video_buffer* buf);
uint8_t calculate_y(uint8_t x, uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
uint8_t modulo(int8_t n);
uint16_t reverse_byte(uint16_t b);

uint8_t update_LINE(data_container* c, video_buffer* buf);
uint8_t update_COL(data_container* c, video_buffer* buf);
uint8_t update_BAR(data_container* c, video_buffer* buf);
uint8_t update_SCATTER(data_container* c, video_buffer* buf);
uint8_t update_XY(data_container* c, video_buffer* buf);

/*Initialize buffer memory
* return error code
* error code list:
* 1 error allocating memory
*/
uint8_t init_buf(video_buffer *buf, uint8_t size_x, uint8_t size_y) {
    uint8_t error = 0;
    buf->size_x = size_x;
    buf->size_y = size_y;
    buf->byte_col_cnt = byte_col_cnt(size_x);
    buf->byte_buffer_cnt = buffer_byte_cnt(buf);
    buf->vid_buf = (uint8_t *)malloc(buf->byte_buffer_cnt);
    
    /*initialize default pointer values*/
    buf->origin[0] = 0;
    buf->origin[1] = 0;
    buf->r_matrix[0] = 0;
    buf->r_matrix[1] = 0;
    buf->r_matrix[2] = 0;
    buf->r_matrix[3] = 0;

    clr_buffer(buf);
    set_mode(NORMAL, buf);
    set_orientation(LANDSCAPE, buf);
    return error;
}

/* Set buffer mode between NORMAL and INVERTED and adjust buffer accordingly*/
void set_mode(enum mode mode, video_buffer *buf) {
    
    buf->mode = mode;

    if (mode) {
        for (uint16_t i = 0; i < buf->byte_buffer_cnt; i++) {
            buf->vid_buf[i] = ~buf->vid_buf[i];
        }
    }
}

/* Adjust rotation matrix according to orientation */
void set_orientation(enum orientation o, video_buffer* buf) {
    
    buf->orient = o;
    switch (o) {

    case LANDSCAPE:
        buf->origin[X] = 0;
        buf->origin[Y] = 0;
        buf->r_matrix[_A] = 1;
        buf->r_matrix[_B] = 0;
        buf->r_matrix[_C] = 0;
        buf->r_matrix[_D] = 1;

        break;
    case PORTRAIT:
        buf->origin[X] = buf->size_x-1;
        buf->origin[Y] = 0;
        buf->r_matrix[_A] = 0;
        buf->r_matrix[_B] = -1;
        buf->r_matrix[_C] = 1;
        buf->r_matrix[_D] = 0;
        break;
    case LANDSCAPE_INVERTED:
        buf->origin[X] = buf->size_x-1;
        buf->origin[Y] = buf->size_y-1;
        buf->r_matrix[_A] = -1;
        buf->r_matrix[_B] = 0;
        buf->r_matrix[_C] = 0;
        buf->r_matrix[_D] = -1;
        break;
    case PORTRAIT_INVERTED:
        buf->origin[X] = 0;
        buf->origin[Y] = buf->size_y-1;
        buf->r_matrix[_A] = 0;
        buf->r_matrix[_B] = 1;
        buf->r_matrix[_C] = -1;
        buf->r_matrix[_D] = 0;
        break;
    default:
        buf->origin[X] = 0;
        buf->origin[Y] = 0;
        buf->r_matrix[_A] = 1;
        buf->r_matrix[_B] = 0;
        buf->r_matrix[_C] = 0;
        buf->r_matrix[_D] = 1;
        break;
    }
}

/* ===========================================GRAPHIC FUNCTIONS==================================================== */

/*Reset buffer to default value according to MODE*/

uint8_t clr_buffer(video_buffer* buf) {
    
    uint8_t def = (buf->mode == NORMAL) ? 0x00 : 0xff;
    uint8_t error = 1;
    if (buf->vid_buf != NULL) {
        memset(buf->vid_buf, def, buf->byte_buffer_cnt);
        error = 0;
    }
    return error;
}

/* Draw pixel black or white in a specific location on the screen
 * return error code
 * error code list:
 * 4 point out of scope
 */
uint8_t put_pixel(uint8_t pos_x, uint8_t pos_y, enum color color, video_buffer* buf) {

    uint8_t error = 0;
    uint8_t *n_pos = normalize_pos(pos_x, pos_y, buf);

    if ((n_pos[Y] < 0) || (n_pos[Y] > buf->size_y - 1) || (n_pos[X] < 0) || (n_pos[X] > buf->size_x - 1))
        error = 4;
    else {
        if (buf->mode & color)
            color = CLEAR_C;
        else if (buf->mode & ~color)
            color = SET_C;

        if (!color)
            buf->vid_buf[get_buffer_index(n_pos[X], (buf->size_y - n_pos[Y] - 1), buf->byte_col_cnt)] |= 0x80 >> (n_pos[X] % 8);
        else
            buf->vid_buf[get_buffer_index(n_pos[X], (buf->size_y - n_pos[Y] - 1), buf->byte_col_cnt)] &= ~(0x80 >> (n_pos[X] % 8));
    }
    free(n_pos);
    return error;
}


/* Draw line between to points
 * return error code
 * error code list:
 * 4 point out of scope
*/
uint8_t put_line(uint8_t pos_x1, uint8_t pos_y1, uint8_t pos_x2, uint8_t pos_y2, video_buffer* buf) {

    uint8_t error = 0;
    uint8_t firstx = 0;
    uint8_t lastx = 0;
    uint8_t firsty = 0;
    uint8_t lasty = 0;

    if (pos_x1 == pos_x2) { //vertical line

        uint8_t line_lenght = 0;

        if (pos_y1 > pos_y2) {
            line_lenght = (pos_y1 - pos_y2) + 1;
            firsty = pos_y2;
        }
        else {
            line_lenght = (pos_y2 - pos_y1) + 1;
            firsty = pos_y1;
        }
        for (uint8_t i = 0; i < line_lenght; i++) {
            error = put_pixel(pos_x1, firsty + i, SET_C, buf);
        }
    }
    else {
        if (pos_x1 > pos_x2) {
            firstx = pos_x2;
            lastx = pos_x1;
            firsty = pos_y2;
            lasty = pos_y1;
        }
        else {
            firstx = pos_x1;
            lastx = pos_x2;
            firsty = pos_y1;
            lasty = pos_y2;
        }

        int8_t slope_addendum = (firsty > lasty) ? -1 : 1;

        uint8_t y = firsty;
        uint8_t nexty = 0;
        int8_t y_todrw = 0;
        uint8_t fill_y = 0;

        for (uint8_t x = firstx; x < lastx + 1; x++) {

            y = calculate_y(x, firstx, lastx, firsty, lasty);
            nexty = calculate_y(x + 1, firstx, lastx, firsty, lasty);
            fill_y = 0;

            do { 
               y_todrw = y + (slope_addendum * fill_y);
              
               if (y_todrw < 0)
                   y_todrw = 0;

               error = put_pixel(x, y_todrw, SET_C, buf);
               fill_y++;

               if (y_todrw == pos_y2 && pos_y1 != pos_y2)
                   return error;

            } while ( fill_y < modulo(y - nexty));
        }
    }
    return error;
}


/* Clear part of the display buffer memory
 * return error code
 * error code list:
 * 
 * 2 dimension mismatch
 * 
 * 4 starting point out of scope
*/
uint8_t clear_sector(uint8_t pos_x, uint8_t pos_y, uint8_t width, uint8_t height, video_buffer* buf) {

    uint8_t error = 0;
    uint8_t startx_ptr = 0; // pos_x;
    uint8_t starty_ptr = 0; // pos_y;
    uint8_t endx_ptr = 0;   //pos_x + width;
    uint8_t endy_ptr = 0;   // pos_y + height;
    uint8_t n_width = 0;    //normalized width
    uint8_t* n_pos_ptr = normalize_pos(pos_x, pos_y, buf);
    
    switch (buf->orient) {

    case LANDSCAPE:
        n_width = width;
        startx_ptr = n_pos_ptr[X];
        starty_ptr = n_pos_ptr[Y];
        endx_ptr = startx_ptr + width;
        endy_ptr = starty_ptr + height;
        break;

    case LANDSCAPE_INVERTED:
        n_width = width;
        startx_ptr = n_pos_ptr[X] - width + 1;
        starty_ptr = n_pos_ptr[Y] - height + 1;
        endx_ptr = n_pos_ptr[X] + 1;
        endy_ptr = n_pos_ptr[Y] + 1;
        break;

    case PORTRAIT:
        n_width = height;
        startx_ptr = n_pos_ptr[X] - height + 1;
        starty_ptr = n_pos_ptr[Y];
        endx_ptr = n_pos_ptr[X] + 1;
        endy_ptr = starty_ptr + width;
        break;

    case PORTRAIT_INVERTED:
        startx_ptr = n_pos_ptr[X];
        starty_ptr = n_pos_ptr[Y] - width + 1;
        endx_ptr = startx_ptr + height;
        endy_ptr = n_pos_ptr[Y] + 1;
        break;

    default:
        break;
    }

    if (pos_x >= buf->size_x || pos_x < 0 || pos_y >= buf->size_y || pos_y < 0)
        return 4;

    if (pos_y + height > buf->size_y || pos_x + width > buf->size_x)
        error = 2;

    else {

        uint16_t start_byte_ptr = get_buffer_index(startx_ptr, buf->size_y - 1 - starty_ptr, buf->byte_col_cnt);
        uint16_t buffer_byte_ptr = start_byte_ptr;
        int16_t initial_shift = modulo(endx_ptr - startx_ptr);
        int16_t to_shift = initial_shift;

        for (uint8_t row_clrd = 0; row_clrd < modulo(endy_ptr - starty_ptr); row_clrd++)
        {
            buffer_byte_ptr = start_byte_ptr - (buf->byte_col_cnt * row_clrd);
            to_shift = initial_shift;

            if ( (startx_ptr % 8) + n_width > 7) {  // when multiple bytes are needed
                to_shift -= (8 - (startx_ptr % 8));
                
                if(buf->mode == NORMAL)
                    buf->vid_buf[buffer_byte_ptr] &= 0xff << (8 - (startx_ptr % 8));
                else 
                    buf->vid_buf[buffer_byte_ptr] |= 0xff >> (startx_ptr % 8);
                
                buffer_byte_ptr++;
                
                while (to_shift > 8) {
                    if(buf->mode == NORMAL)
                        buf->vid_buf[buffer_byte_ptr] = 0x00;
                    else 
                        buf->vid_buf[buffer_byte_ptr] = 0xff;
                        
                    buffer_byte_ptr++;
                    to_shift -= 8;
                }

                if(buf->mode == NORMAL)
                    buf->vid_buf[buffer_byte_ptr] &= 0xff >> to_shift;
                else 
                    buf->vid_buf[buffer_byte_ptr] |= 0xff << (8 - (endx_ptr % 8));
            }
            else { // can do clear operation on a single byte because of sector dimensions
                if(buf->mode == NORMAL)
                    buf->vid_buf[buffer_byte_ptr] &= ~((0xff >> startx_ptr % 8) & (0xff << (8 - (endx_ptr % 8))));
                else 
                    buf->vid_buf[buffer_byte_ptr] |= (0xff >> startx_ptr % 8) & (0xff << (8 - (endx_ptr % 8)));
            }
        }
    }
    free(n_pos_ptr);
    return error;
}

/*put character on screen
 * return error code
 * error code list:
 * 
 * 2 dimension mismatch
 * 
 * 4 starting point out of scope
 * note: each letter needs a square of 7x7 pixels clear on the display (because of rotation) befor drawing the letter
*/
uint8_t put_char(uint8_t pos_x, uint8_t pos_y, uint8_t chr, enum fonts_size ff, video_buffer* buf) {
    
    uint8_t error = 0;
    uint8_t* n_pos; 
    uint16_t* c;

    switch(ff){ //font selection
        case SMALL:
            font_height = C_HEIGHT_S;
            font_width = C_WIDTH_S;
            c = font_s[chr];
        break;
        case MEDIUM:
            font_height = C_HEIGHT_M;
            font_width = C_WIDTH_M;
            c = font_m[chr];
        break; 
        
        case LARGE:
        break; 

        default:
            font_height = C_HEIGHT_S;
            font_width = C_WIDTH_S;
            c = font_s[chr];
        break;
    }
/*
    switch (buf->orient) {
        case LANDSCAPE:
            n_pos = normalize_pos(pos_x, pos_y, buf); 
           break;

        case LANDSCAPE_INVERTED:
            n_pos = normalize_pos(pos_x + font_width - 1, pos_y + font_height - 1, buf);
               break;

        case PORTRAIT:
            n_pos = normalize_pos(pos_x, pos_y + font_height, buf);
            break;

        case PORTRAIT_INVERTED:
            n_pos = normalize_pos(pos_x + font_height - 1, pos_y, buf);
            break;

        default:
            n_pos = normalize_pos(pos_x, pos_y, buf);
            break;
        }
*/
    if (n_pos[X] >= buf->size_x || n_pos[X] < 0 || n_pos[Y] >= buf->size_y || n_pos[Y] < 0)
        error = 4;
    else if (n_pos[Y] + font_height > buf->size_y || n_pos[X] + font_width > buf->size_x)
        error = 2;
    else {
        c = rotate_char(c, buf);
            
        uint16_t start_byte_ptr = get_buffer_index(n_pos[X], buf->size_y - 1 - n_pos[Y], buf->byte_col_cnt);
        uint16_t buffer_byte_ptr = start_byte_ptr;
        uint16_t temp = 0;
        uint8_t shifter_a = n_pos[X] % 8;
    
        for (uint8_t i = 0; i < font_height; i++) {
            buffer_byte_ptr = start_byte_ptr - (buf->byte_col_cnt * i);
            temp = c[font_height - 1 - i] >> shifter_a;
            buf->vid_buf[buffer_byte_ptr] |= (uint8_t)(temp >> 8);
            buf->vid_buf[buffer_byte_ptr + 1] |= (uint8_t)temp; 
            
            if (shifter_a != 0){
                    temp = c[font_height - i - 1];
                    buf->vid_buf[buffer_byte_ptr + 2] |= (uint8_t)temp << (8 - shifter_a);
                }
        }
    } 
    
   // free(n_pos);
    return error;
}

/*put character string on screen
 * return error code
 * error code list:
 * 1 empty string
 * 2 dimension mismatch
 *
 * 4 starting point out of scope
*/
uint8_t put_string(uint8_t pos_x, uint8_t pos_y, uint8_t* chr, enum fonts_size ff, video_buffer* buf) {
    
    uint8_t error = 1;
    uint8_t i = 0;
    uint8_t y_offset = pos_y;
    uint8_t x_offset = pos_x;    

    if (chr != NULL) {
        
        while(i < strlen(chr)) {

            if (chr[i] == '\r')
                x_offset = pos_x;
            else if (chr[i] == '\n')
                y_offset = y_offset - 1 - font_height;
            else {
                error = put_char(x_offset, y_offset, chr[i], ff, buf);
                if (error != 0)
                    break;
                x_offset += font_width + 1;
            
            }
            i++;
        }
    }
    return error;
}

/* Load vbc file smaller than display size to buffer memory
 * return error code
 * error code list:
 * 1 wrong file type
 * 2 dimension mismatch
 * 3 body bytes count mismatch
 * 4 starting point out of scope
 * note: starting position (x,y)  referenced to LANDSCAPE and rappresents lower left corner of the vbc
*/
uint8_t put_vbc(vbc_file file, uint8_t x, uint8_t y, video_buffer* buf) {
    uint8_t error = 0;
    uint16_t vbc_body_byte = (((uint16_t)file.header.byte_cnt_h) << 8 | file.header.byte_cnt_l) - 5;

    if (x > buf->size_x || x < 0 || y > buf->size_y || y < 0)
        return 4;

    if (file.header.id != 0xA7)
        error = 1;
    else if (y + file.header.height > buf->size_y || x + file.header.width > buf->size_x)
        error = 2;
    else if (vbc_body_byte > buf->byte_buffer_cnt - get_buffer_index(x, y, buf->byte_col_cnt))
        error = 3;
    else if (vbc_body_byte == buf->byte_buffer_cnt) {
        buf->vid_buf = file.body;
    }
    else {
        uint8_t byte_width = (file.header.width % 8 == 0) ? file.header.width / 8 : file.header.width / 8 + 1;
        uint16_t start_byte_ptr = get_buffer_index(x, buf->size_y - 1 - y, buf->byte_col_cnt);
        uint16_t buffer_byte_ptr = start_byte_ptr;

        for (uint8_t row = 0; row < file.header.height - 1; row++) {
            buffer_byte_ptr = start_byte_ptr - (buf->byte_col_cnt * row);
            memmove(&buf->vid_buf[buffer_byte_ptr], &file.body[((file.header.height - 1) * byte_width) - row * byte_width], byte_width);//row * byte_width], byte_width);
        }
    }
    return error;
}


/* ================================================SHAPES========================================================= */

/* Draw circle at specific location on the screen with specified radius
 * return error code
 * error code list:
 * 4 point out of scope
*/
uint8_t put_circle(uint8_t o_x, uint8_t o_y, uint8_t r, video_buffer* buf) {

    uint8_t error = 0;
    uint16_t s = 0;
    uint16_t rsqr = r * r;
    
    for (int16_t y = -r; y <= r; y++) {
        for (int16_t x = -r; x <= r; x++) {
            s = x * x + y * y;
            if (s >= rsqr - r && s <= rsqr + r) {
                error = put_pixel(o_x + x, o_y + y, SET_C, buf);
               // if (error)
                //    return error;
            }
        }
    }
    return error;
}

/* Draw rectangular shape at specific location on the screen with specified height and width
 * return error code
 * error code list:
 * 4 point out of scope
 * note: the starting point of the shape is in the lower left corner
*/
uint8_t put_rect(uint8_t o_x,uint8_t o_y, uint8_t width, uint8_t height, video_buffer* buf){
    uint8_t error = 0;
    error = put_line(o_x, o_y, o_x + width, o_y, buf);
    error = put_line(o_x + width, o_y, o_x + width, o_y + height, buf);
    error = put_line(o_x, o_y + height, o_x + width, o_y + height, buf);
    error = put_line(o_x, o_y, o_x, o_y + height, buf);
    return error;
}

/* Draw triangle shape with specified vertices
 * return error code
 * error code list:
 * 4 point out of scope
*/
uint8_t put_triangle(uint8_t a_x, uint8_t a_y, uint8_t b_x, uint8_t b_y, uint8_t c_x, uint8_t c_y, video_buffer* buf) {
    uint8_t error = 0;
    error = put_line(a_x, a_y, b_x, b_y, buf);
    error = put_line(a_x, a_y, c_x, c_y, buf);
    error = put_line(b_x, b_y, c_x, c_y, buf);
    return error;
}

/* ========================================DATA VISUALIZATION===================================================== */

uint8_t update_chart(float new_data, data_container* c, video_buffer* buf){
    uint8_t error = 0;
    c->data = new_data;

    switch (c->type) {
    case COL:
        error = update_COL(c,buf);
        break;
    case XY: //when calling update_XY new data is ignored: changing of data_container.xy_entry struct is needed
        error = update_XY(c, buf);
        break;
    case BAR:
        error = update_BAR(c, buf);
        break;
    case LINE:
        error = update_LINE(c,buf);
        break;
    case SCATTER:
        error = update_SCATTER(c, buf);
        break;
    default:
        break;

    }
    return error;
}

/* ===========================================INTERNAL FUNCTIONS=================================================== */

/* Calculate the number of columns in number of bytes needed for the video buffer*/
uint8_t byte_col_cnt(uint8_t size_x) {

    uint8_t col_n = (size_x / 8);

    if (size_x % 8)
        col_n++;

    return col_n;
}

/* Calculate the total number of bytes needed for the video buffer*/
uint16_t buffer_byte_cnt(video_buffer* buf) {
    
    return buf->size_y * buf->byte_col_cnt;
}

/* Set the video buffer index to address for  x & y position in the video matrix  
 * Sanity check before calling
*/
uint16_t get_buffer_index(uint8_t pos_x, uint8_t pos_y, uint8_t col_byte_cnt){

    return (col_byte_cnt * pos_y) + (uint8_t)(pos_x / 8);

}

/* Normalize orientation operation.  
 * Project coordinates in LANDSCAPE coordinates
*/
uint8_t * normalize_pos(uint8_t pos_x, uint8_t pos_y, video_buffer* buf) {

    uint8_t *pos = (uint8_t*)malloc(2);
    pos[0] = buf->origin[X] + (pos_x * buf->r_matrix[_A]) + (pos_y * buf->r_matrix[_B]);
    pos[1] = buf->origin[Y] + (pos_x * buf->r_matrix[_C]) + (pos_y * buf->r_matrix[_D]);
    
    return pos;
}

/* 
 * Generates new normalized character from font.h character 
*/
uint16_t* rotate_char(uint16_t *chr, video_buffer* buf) {
    
    uint16_t *new_chr = (uint16_t *)malloc(font_height);
    
    for (uint8_t i = 0; i < font_height; i++)
            new_chr[i]=0;
            
    uint16_t* nc_ptr = new_chr;
    uint16_t bit = 0;
    
    switch (buf->orient) {
   
    case LANDSCAPE :
        nc_ptr = chr;
        break;
    
    case LANDSCAPE_INVERTED:

        for (uint8_t i = 0; i < font_height; i++) {
            new_chr[i] = reverse_byte(chr[font_height - 1 - i] >> (16 - font_width)) ;
        }
        break;

    case PORTRAIT:

        for (uint8_t i = 0; i < font_height; i++) {
            for (uint8_t j = 0; j < font_height; j++) {
                bit = (chr[font_height - 1 - i] & (0x8000 >> j)) << j;
                new_chr[font_height - 1  - j] |= bit >> (font_height - i);    
                bit = 0;
            }
        }
        break;
   
    case PORTRAIT_INVERTED:

        for (uint8_t i = 0; i < font_height; i++) {
            for (uint8_t j = 0; j < font_height; j++) {
                bit = (chr[font_height - 1 - i] & (0x8000 >> j)) << j;
                new_chr[j] |= (bit >> i);
                bit = 0;
            }
        }
        break;
    
    default:
        nc_ptr = chr;
        break;
    }
    return nc_ptr;
}

uint16_t reverse_byte(uint16_t b) {

    b = ((b >> 1) & 0x5555) | ((b & 0x5555) << 1);
    b = ((b >> 2) & 0x3333) | ((b & 0x3333) << 2);
    b = ((b >> 4) & 0x0f0f) | ((b & 0x0f0f) << 4);
    b = ((b >> 8) & 0x00ff) | ((b & 0x00ff) << 8);

    return b;
}


uint8_t modulo(int8_t n) {
    return (n > 0) ? n : -n;
}

uint8_t calculate_y(uint8_t x, uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1) {
    return ((x - x0) * (y1 - y0) / (x1 - x0)) + y0;
}

/* Draw only positive column chart
 * return error code
 * error code list:
 * 1 data out of range
 * 2 dimension mismatch
 * 3 chart not defined
 * 4 starting point out of scope
*/
uint8_t put_pos_COL(data_container* c, video_buffer* buf) {

    uint8_t error = 1;

    if (c->max == 0 || c->dim_x == 0 || c->dim_y_pos == 0 || c->max <= c->min)
        error = 3;

    else {
        error = clear_sector(c->o_x, c->o_y, c->dim_x, c->dim_y_pos, buf); //limits check within clear_sector

        if (c->o_y + c->data >= 0) {
            if (c->data > 0 && c->data <= c->max) {
                uint8_t dy = c->data * c->dim_y_pos / c->max;

                for (uint8_t i = 0; i < c->dim_x; i++)
                    error = put_line(c->o_x + i, c->o_y, c->o_x + i, c->o_y + dy - 1, buf);
            }
        }
        else
            error = 2;
    }

    return error;
}

/* Draw only positive column chart
 * return error code
 * error code list:
 * 1 data out of range
 * 2 dimension mismatch
 * 3 chart not defined
 * 4 starting point out of scope
*/
uint8_t put_neg_COL(data_container* c, video_buffer* buf) {

    uint8_t error = 1;

    if (c->dim_x == 0 || c->max <= c->min)
        error = 3;

    else {
        error = clear_sector(c->o_x, c->o_y - c->dim_y_neg, c->dim_x, c->dim_y_neg, buf); //limits check within clear_sector

        if (c->o_y + c->data <= 0) {

            if (c->data < 0 && c->data >= c->min) {
                uint8_t dy = c->data * c->dim_y_neg / c->min;

                for (uint8_t i = 0; i < c->dim_x; i++)
                    error = put_line(c->o_x + i, c->o_y-dy+1, c->o_x + i, c->o_y, buf);
            }
        }
        else
            error = 2;
    }
    return error;
}

uint8_t update_COL(data_container* c, video_buffer* buf) {
    uint8_t error = 0;
    error = put_pos_COL(c, buf);
    error = put_neg_COL(c, buf);
    return error;
}

/* Draw positive bar chart
 * return error code
 * error code list:
 * 1 data out of range
 * 2 dimension mismatch
 * 3 chart not defined
 * 4 starting point out of scope
*/
uint8_t update_BAR(data_container* c, video_buffer* buf) {

    uint8_t error = 1;

    if (c->max == 0 || c->dim_x == 0 || c->dim_y_pos == 0 || c->max <= c->min)
        error = 3;

    else {
        error = clear_sector(c->o_x, c->o_y, c->dim_x, c->dim_y_pos, buf); //limits check within clear_sector

        if (c->o_y + c->data >= 0) {
            if (c->data > 0 && c->data <= c->max) {
                uint8_t dx = c->data * c->dim_x / c->max;

                for (uint8_t i = 0; i < c->dim_y_pos; i++)
                    error = put_line(c->o_x, c->o_y + i, c->o_x + dx - 1, c->o_y + i, buf);
            }
        }
        else
            error = 2;
    }

    return error;
}

/* Draw line like chart
 * return error code
 * error code list:
 * 1 data out of range
 * 2 dimension mismatch
 * 3 chart not defined
 * 4 starting point out of scope
 * beware: no check on data coherence nor axis limits
 * note: change data before calling update
*/
uint8_t update_LINE(data_container* c, video_buffer* buf) {

    uint8_t error = 1;
    int16_t value = (c->data * c->dim_y_pos / c->max);
    static uint8_t old_data;
    static uint8_t dx;
    dx = c->dim_x / c->x_division; //limit is dim_x
    static uint8_t x;

    static uint8_t x_cnt = 0;

    if (c->dim_x == 0 || c->max <= c->min)
        error = 3;

    else {
        if (x_cnt > c->x_division)
            x_cnt = 0;

        if (x_cnt == 0) {
            // cancel chart area only at first entry
            //draw axis only at first entry
            put_line(c->o_x, c->o_y, c->o_x + c->dim_x, c->o_y, buf);
            put_line(c->o_x, c->o_y, c->o_x, c->o_y + c->dim_y_pos, buf);
            put_line(c->o_x, c->o_y, c->o_x, c->o_y - c->dim_y_neg, buf); //
            
            for (uint8_t i = 0; i <= c->x_division; i++) //draw x data markers
                put_pixel(c->o_x + dx * i, c->o_y - 1,SET_C, buf);
            
            old_data = c->o_y;
            error = clear_sector(c->o_x + 1, c->o_y - c->dim_y_neg, c->dim_x, c->dim_y_neg, buf); //limits check within clear_sector
            error = clear_sector(c->o_x + 1, c->o_y + 1, c->dim_x, c->dim_y_pos, buf); //limits check within clear_sector
            //first entry
            error = put_line(c->o_x, (uint8_t)old_data, c->o_x, (uint8_t)(c->o_y + value), buf);
            old_data = c->o_y + value;
            x = c->o_x;
        }
        else {
            error = put_line(x, (uint8_t)old_data, x + dx, c->o_y + value, buf);
            old_data = (uint8_t)(c->o_y + value);
            x += dx;
        }
        x_cnt++;
    }
    return error;
}

/* Draw scatter like chart
 * return error code
 * error code list:
 * 1 data out of range
 * 2 dimension mismatch
 * 3 chart not defined
 * 4 starting point out of scope
 * beware: no check on data coherence nor axis limits
 * note: change data before calling update
*/
uint8_t update_SCATTER(data_container* c, video_buffer* buf) {
    
    uint8_t error = 1;
    int16_t value = (c->data * c->dim_y_pos / c->max);
    static uint8_t old_data;
    static uint8_t dx;
    dx = c->dim_x / c->x_division; //limit is dim_x
    static uint8_t x;

    static uint8_t x_cnt = 0;

    if (c->dim_x == 0 || c->max <= c->min)
        error = 3;

    else {
        if (x_cnt > c->x_division)
            x_cnt = 0;

        if (x_cnt == 0) {
            // cancel chart area only at first entry
            //draw axis only at first entry
            put_line(c->o_x, c->o_y, c->o_x + c->dim_x, c->o_y, buf);
            put_line(c->o_x, c->o_y, c->o_x, c->o_y + c->dim_y_pos, buf);
            put_line(c->o_x, c->o_y, c->o_x, c->o_y - c->dim_y_neg, buf);
            
                
            for (uint8_t i = 0; i <= c->x_division; i++) //draw x data markers
                put_pixel(c->o_x + dx * i, c->o_y - 1, SET_C, buf);

            old_data = c->o_y;
                
            error = clear_sector(c->o_x + 1, c->o_y - c->dim_y_neg, c->dim_x, c->dim_y_neg, buf); //limits check within clear_sector
            error = clear_sector(c->o_x + 1, c->o_y + 1, c->dim_x, c->dim_y_pos, buf); //limits check within clear_sector
            //first entry
            error = put_line(c->o_x, (uint8_t)old_data, c->o_x, (uint8_t)(c->o_y + value), buf);
            old_data = c->o_y + value;
            x = c->o_x;
        }
        else {
            error = put_pixel(x, (uint8_t)(c->o_y + value), SET_C, buf);
            //old_data = (uint8_t)(c->o_y + value);
            x += dx;
        }
        x_cnt++;
    }
    return error;
}


/* Draw XY type chart where for every x is associate a y value similar to scatter but data are taken from an array and displayed all at once
 * return error code
 * error code list:
 * 1 data out of range
 * 2 dimension mismatch
 * 3 chart not defined
 * 4 starting point out of scope
 * beware: no check on data coherence nor axis limits
 * note: both x and y values get scaled thus the visualization can be messy if x values are to close or there are too many entries
*/
uint8_t update_XY(data_container* c, video_buffer* buf) {

    uint8_t error = 1;
    uint8_t dx = 0;
    int16_t dy = 0;

    if (c->dim_x == 0 || c->max <= c->min)
        error = 3;

    else {
            // cancel chart area only at first entry
            //draw axis only at first entry
            error = clear_sector(c->o_x, c->o_y - c->dim_y_neg, c->dim_x, c->dim_y_neg, buf); //limits check within clear_sector
            error = clear_sector(c->o_x, c->o_y, c->dim_x, c->dim_y_pos, buf); //limits check within clear_sector
            put_line(c->o_x, c->o_y, c->o_x + c->dim_x, c->o_y, buf);
            put_line(c->o_x, c->o_y, c->o_x, c->o_y + c->dim_y_pos, buf);
            put_line(c->o_x, c->o_y, c->o_x, c->o_y - c->dim_y_neg, buf);

            for (uint8_t i = 0; i < c->xy_entries.n_ent; i++){
                dx = c->xy_entries.x[i] * c->dim_x / c->max_x;
                dy = c->xy_entries.y[i] * c->dim_y_pos / c->max;
                put_pixel(c->o_x + dx, c->o_y + dy, SET_C, buf);
            } 
    }
    return error;
}
