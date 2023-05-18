#include "pico/multicore.h"

#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

#include "fixed_fft.hpp"

using namespace pimoroni;

#define FRAME_WIDTH 720
#define FRAME_HEIGHT 480

#define NUM_SAMPLES 1024
#define DISPLAY_SAMPLES 50
#define SAMPLE_WIDTH 8
#define DISPLAY_WIDTH (DISPLAY_SAMPLES * SAMPLE_WIDTH)
#define SAMPLE_HEIGHT 200

static DVDisplay display(FRAME_WIDTH, FRAME_HEIGHT);
static PicoGraphics_PenDV_RGB555 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

static volatile int16_t sample_buffer[DISPLAY_SAMPLES];

static FIX_FFT fft;

#define MAX_TITLE_LEN 63
static char title[MAX_TITLE_LEN+1];

static void init_screen() {
    graphics.set_pen(0,0,50);
    graphics.clear();

    graphics.set_pen(200,200,200);
    graphics.set_font("bitmap8");
    graphics.text("Pico DV Player", Point(160, 50), 400, 4);
}

static void set_title() {
    graphics.set_pen(0,0,50);
    graphics.rectangle(Rect(160, 400, 400, 50));
    graphics.set_pen(200,200,200);
    graphics.set_font("bitmap8");
    graphics.text(title, Point(160, 400), 400, 2);
}

static void set_clip() {
    graphics.set_clip(Rect((FRAME_WIDTH - DISPLAY_WIDTH) / 2, (FRAME_HEIGHT - SAMPLE_HEIGHT) / 2, DISPLAY_WIDTH, SAMPLE_HEIGHT));
}

static void core1_main() {
    printf("Core1 launched\n");
    display.init();
    multicore_fifo_push_blocking(1);
    init_screen();
    display.flip();
    init_screen();

    set_clip();

    while (true) {
        graphics.set_pen(0);
        graphics.clear();

        graphics.set_pen(0,127,0);
        uint32_t req = multicore_fifo_pop_blocking();
        while (req == 0 && multicore_fifo_rvalid()) {
            req = multicore_fifo_pop_blocking();
        }
        if (req == 1) {
            graphics.remove_clip();
            set_title();
            display.flip();
            set_title();
            set_clip();
            continue;
        }

        int y = (FRAME_HEIGHT + SAMPLE_HEIGHT) / 2;
        for (int i = 0, x = (FRAME_WIDTH - DISPLAY_WIDTH) / 2; i < DISPLAY_SAMPLES; ++i, x += SAMPLE_WIDTH) {
            //graphics.pixel_span(Point(x, y - sample_buffer[i]), SAMPLE_WIDTH);
            graphics.rectangle(Rect(x, y - sample_buffer[i], SAMPLE_WIDTH, sample_buffer[i]));
        }

        display.flip();
    }
}

void bt_display_init() {
    printf("Initializing display\n");
    fft.set_scale(.25f);
    memset((void*)sample_buffer, 0, sizeof(int16_t) * DISPLAY_SAMPLES);
    memset(title, 0, MAX_TITLE_LEN+1);
    multicore_launch_core1(core1_main);
    multicore_fifo_pop_blocking();
    printf("Display started\n");
}

void bt_display_set_samples(int16_t * samples, uint16_t num_samples)
{
    memcpy(fft.sample_array, samples, sizeof(int16_t)*std::min(num_samples, (uint16_t)NUM_SAMPLES));
    fft.update();
    for (int i = 0; i < DISPLAY_SAMPLES; ++i)
        sample_buffer[i] = fft.get_scaled(i);
    //printf("%hd %hd %hd\n", sample_buffer[2], sample_buffer[4], sample_buffer[8]);
    multicore_fifo_push_blocking(0);
}

void bt_display_set_track_title(char* _title)
{
    strncpy(title, _title, MAX_TITLE_LEN);
    multicore_fifo_push_blocking(1);
}