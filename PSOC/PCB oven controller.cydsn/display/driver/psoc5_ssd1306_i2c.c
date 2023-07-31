/*
* Adjust write_buf() function according to your project names
* Kiriaus 15/7/22
*/

#include "psoc5_ssd1306_i2c.h"
#include "project.h"

static uint8 _i2caddr; 

static uint32_t write_buf( uint8_t * buf, uint16_t size ); 

void init_display( uint8_t addr ){
   
    _i2caddr = addr;
    
    uint8_t cmdbuf[] = {
        0x00,
        SSD1306_DISPLAYOFF,
        SSD1306_SETDISPLAYCLOCKDIV,
        0x80,
        SSD1306_SETMULTIPLEX,
        0x3f,
        SSD1306_SETDISPLAYOFFSET,
        0x00,
        SSD1306_SETSTARTLINE | 0x0,
        SSD1306_CHARGEPUMP,
        0x14,
        SSD1306_MEMORYMODE,
        0x00,
        SSD1306_SEGREMAP | 0x01,
        SSD1306_COMSCANDEC,
        SSD1306_SETCOMPINS,
        0x12,
        SSD1306_SETCONTRAST,
        0xcf,
        SSD1306_SETPRECHARGE,
        0xf1,
        SSD1306_SETVCOMDETECT,
        0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY,
        SSD1306_DISPLAYON
    };
    
    write_buf( cmdbuf, sizeof(cmdbuf) ); 
}

void update_display(video_buffer *buf){
    
    uint8_t bit = 0;  
    uint8_t new_col = 0;

    uint8_t SSD1306_buffer[buf->byte_buffer_cnt + 1];
    
    uint8 cmdbuf[] = {
        0x00,
        SSD1306_COLUMNADDR,
        0,                      // start
        buf->size_x - 1, // end
        SSD1306_PAGEADDR,
        0,                      // start
        buf->size_y/8 - 1                       // end
    };

    write_buf( cmdbuf, sizeof(cmdbuf) ); 

    SSD1306_buffer[0] = 0x40;
    
    int p = 1;
        
    //madness to adapt from vid_buffer structure to display structure;
    for(uint16_t page = 0; page<8; page++){
        
        for(uint8_t k = 0; k < 16; k++){    
            for(uint8_t i = 0; i < 8; i++){ 
                for (uint8_t j = 0; j < 8; j++) {
                    bit = (buf->vid_buf[(128*page)+(k + (j*buf->byte_col_cnt))] & (0b10000000 >> i))<<i;
                    new_col |= bit >> (8-j-1) ;
                    bit = 0;
                } 
            SSD1306_buffer[p++] = new_col;
            new_col = 0;
            } 
        }
    }    
    
    write_buf( SSD1306_buffer, sizeof(SSD1306_buffer) );

}

// for submitting command sequences:
//  buf[0] must be 0x00
// for submitting bulk data (writing to display RAM):
//  buf[0] must be 0x40
static uint32_t write_buf( uint8_t * buf, uint16_t size ){

    uint32_t status = TRANSFER_ERROR;
    oled_MasterSendStart(_i2caddr,oled_WRITE_XFER_MODE);
    
    for(int i=0; i<size; i++){  
        status=oled_MasterWriteByte(buf[i]);
    }
    oled_MasterSendStop();
    return status;
}