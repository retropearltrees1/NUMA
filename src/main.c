#include <eadk.h>
#include "../numplay_repo/launcher/src/gfx.h"
#include "../numplay_repo/games/common/np_text.h"
#include "../numplay_repo/games/common/epsilon_app.h"

#ifndef HOST
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "NUMA";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;
#endif

void np_push(int x, int y, int w, int h, const color_t *px) {
  eadk_display_push_rect((eadk_rect_t){(uint16_t)x, (uint16_t)y, (uint16_t)w, (uint16_t)h}, px);
}

#define KEY(k) (1ull << (k))
#define BG_COLOR HEX(0x101828)
#define CARD_BG HEX(0x1E293B)
#define NUMA_BLUE HEX(0x38BDF8)
#define USER_GREEN HEX(0x4ADE80)
#define WHITE 0xFFFF

static color_t strip[SCREEN_W * STRIP_H];
static char user_input[64] = "";
static int input_len = 0;
static char response[128] = "Bonjour ! Je suis NUMA, ton assistant hors ligne.";

static void analyze_and_respond(void) {
  if (input_len == 0) return;
  
  if (user_input[0] == 'h' || user_input[0] == 'H') {
    int i = 0;
    const char *r = "Bonjour ! Comment puis-je t'aider en maths ou sciences ?";
    while (r[i] && i < 127) { response[i] = r[i]; i++; }
    response[i] = 0;
  } else if (user_input[0] == 'm' || user_input[0] == 'M') {
    int i = 0;
    const char *r = "NUMA v1.0 : Processeur ARM Cortex-M7 NumWorks actif.";
    while (r[i] && i < 127) { response[i] = r[i]; i++; }
    response[i] = 0;
  } else {
    int i = 0;
    const char *r = "Intention recue. NUMA analyse ta demande...";
    while (r[i] && i < 127) { response[i] = r[i]; i++; }
    response[i] = 0;
  }
  user_input[0] = 0;
  input_len = 0;
}

static void scene(void *ctx) {
  (void)ctx;
  gfx_fill(0, 0, SCREEN_W, SCREEN_H, BG_COLOR);
  
  gfx_rrect(10, 10, 300, 32, 8, CARD_BG, 256);
  gfx_text_center(&np_font_title, 160, 28, "NUMA AI Assistant", NUMA_BLUE, 256);

  gfx_rrect(10, 50, 300, 80, 8, CARD_BG, 256);
  gfx_paragraph(&np_font_body, 160, 75, 280, 16, response, WHITE, 256);

  gfx_rrect(10, 140, 300, 40, 8, CARD_BG, 256);
  gfx_text(&np_font_body, 20, 165, "Toi:", USER_GREEN, 256);
  gfx_text(&np_font_body, 60, 165, user_input[0] ? user_input : "Tape OK pour envoyer", WHITE, 256);

  gfx_text_center(&np_font_small, 160, 220, "Flèches: taper text   OK: Envoyer   Home: Quitter", WHITE, 160);
}

int main(void) {
  np_app_begin();
  gfx_buf = strip;

  uint64_t held = eadk_keyboard_scan();
  for (bool quit = false; !quit;) {
    uint64_t k = eadk_keyboard_scan(), hit = k & ~held;
    held = k;

    if (k & (KEY(eadk_key_home) | KEY(eadk_key_on_off))) break;

    if (hit & KEY(eadk_key_right) && input_len < 60) {
      user_input[input_len++] = 'm';
      user_input[input_len] = 0;
    }
    if (hit & KEY(eadk_key_left) && input_len < 60) {
      user_input[input_len++] = 'h';
      user_input[input_len] = 0;
    }
    if (hit & (KEY(eadk_key_ok) | KEY(eadk_key_exe))) {
      analyze_and_respond();
    }

    gfx_render(scene, 0, 0, SCREEN_H);
    eadk_timing_msleep(20);
  }

  return np_app_end();
}
