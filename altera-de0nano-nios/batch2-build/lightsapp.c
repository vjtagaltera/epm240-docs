/*
 */

#define switches ((volatile unsigned char *)0x0002010)
#define leds     ((volatile unsigned char *)0x0002000)

static volatile unsigned int value = 0;
static volatile unsigned char sw = 0;
static volatile unsigned char sw_last = 0;
static volatile unsigned char sw_count = 0;
static volatile unsigned char sw_out = 0;

void main()
{
    while(1) {
        value ++;
        sw = *switches;
        sw = (sw + (sw>>1) + (sw>>2) + (sw>>3) + (sw>>4) + (sw>>5) + (sw>>6) + (sw>>7));
        if ( sw != sw_last ) {
            sw_count ++;
            sw_last = sw;
        }
        sw_out = (sw_count << 6) & ((unsigned char)0x80);
        sw_out |= (unsigned char)( (value >> 14) & 0x7f);
        *leds = sw_out;
    }
}


