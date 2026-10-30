#include <eadk.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "gfx.h"
#include "np_text.h"
#include "epsilon_app.h"
#include "epsilon_files.h"

#ifndef HOST
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "NUMA";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;
#endif

void np_push(int x, int y, int w, int h, const color_t *px) {
  eadk_display_push_rect((eadk_rect_t){(uint16_t)x, (uint16_t)y, (uint16_t)w, (uint16_t)h}, px);
}

void *np_alloc(uint32_t size) {
  (void)size;
  return 0;
}

#define KEY(k) (1ull << (k))
#define TICK_MS 20

/* Hardware Key bit numbers */
#define K_LEFT      0
#define K_UP        1
#define K_DOWN      2
#define K_RIGHT     3
#define K_OK        4
#define K_BACK       5
#define K_HOME      6
#define K_ONOFF     8
#define K_SHIFT     12
#define K_ALPHA     13
#define K_XNT       14
#define K_VAR       15
#define K_TOOLBOX   16
#define K_BACKSPACE 17
#define K_EXP       18
#define K_LN        19
#define K_LOG       20
#define K_I         21
#define K_COMMA     22
#define K_POWER     23
#define K_SIN       24
#define K_COS       25
#define K_TAN       26
#define K_PI        27
#define K_SQRT      28
#define K_SQUARE    29
#define K_7         30
#define K_8         31
#define K_9         32
#define K_LPAREN    33
#define K_RPAREN    34
#define K_4         36
#define K_5         37
#define K_6         38
#define K_MUL       39
#define K_DIV       40
#define K_1         42
#define K_2         43
#define K_3         44
#define K_PLUS      45
#define K_MINUS     46
#define K_0         48
#define K_DOT       49
#define K_EE        50
#define K_ANS       51
#define K_EXE       52

#define BG_COLOR    HEX(0x0F172A)
#define HEADER_BG   HEX(0x0B1120)
#define CARD_USER   HEX(0x1E293B)
#define CARD_AI     HEX(0x1E1B4B)
#define TEXT_USER   HEX(0x4ADE80)
#define TEXT_AI     HEX(0x38BDF8)
#define ACCENT_AMB  HEX(0xF59E0B)
#define ACCENT_GRN  HEX(0x10B981)
#define TEXT_MUTED  HEX(0x94A3B8)
#define WHITE       0xFFFF

static color_t strip[SCREEN_W * STRIP_H];

#define MAX_HIST 4
typedef struct {
  char q[48];
  char r[192];
} Entry;

static Entry history[MAX_HIST];
static int hist_count = 0;
static int cur_view = 0;

static char user_buf[64] = "";
static int user_len = 0;
static bool alpha_mode = false;
static char saved_user_name[24] = "";

static const char *suggestions[] = {
  "aide",
  "derivees",
  "pythagore",
  "second degre",
  "loi ohm",
  "energie",
  "constantes",
  "qui es tu",
  "25 * 4 + 18",
  "thales",
  "trigo",
  "suites",
  "ph",
  "logarithme",
  "2^10",
  "cinematique"
};
#define NUM_SUGG 16
static int sugg_idx = 0;

static void str_tolower(char *dst, const char *src) {
  int i = 0;
  while (src[i] && i < 63) {
    char c = src[i];
    if (c >= 'A' && c <= 'Z') c += ('a' - 'A');
    dst[i++] = c;
  }
  dst[i] = 0;
}

static bool str_contains(const char *haystack, const char *needle) {
  return strstr(haystack, needle) != NULL;
}

/* ── Integer to string ─────────────────────────── */

static void int_to_str(int val, char *buf) {
  int n = 0, v = val < 0 ? -val : val;
  if (v == 0) { buf[0] = '0'; buf[1] = 0; return; }
  while (v > 0) { buf[n++] = '0' + (v % 10); v /= 10; }
  if (val < 0) buf[n++] = '-';
  buf[n] = 0;
  for (int j = 0; j < n / 2; j++) {
    char t = buf[j]; buf[j] = buf[n - 1 - j]; buf[n - 1 - j] = t;
  }
}

/* ── Recursive descent expression evaluator ────── */
/* Grammar:                                         */
/*   expr   = term (('+' | '-') term)*              */
/*   term   = factor (('*' | '/') factor)*          */
/*   factor = base ('^' factor)?                    */
/*   base   = number | '(' expr ')' | ('+'|'-') base */

static const char *cp;
static int c_err;
static int c_divzero;

static int p_expr(void);
static int p_term(void);
static int p_factor(void);
static int p_base(void);

static int p_num(void) {
  while (*cp == ' ') cp++;
  if (*cp < '0' || *cp > '9') { c_err = 1; return 0; }
  int val = 0;
  while (*cp >= '0' && *cp <= '9') {
    val = val * 10 + (*cp - '0');
    cp++;
  }
  return val;
}

static int p_base(void) {
  while (*cp == ' ') cp++;
  if (*cp == '(') {
    cp++;
    int v = p_expr();
    if (c_err) return 0;
    while (*cp == ' ') cp++;
    if (*cp != ')') { c_err = 1; return 0; }
    cp++;
    return v;
  }
  if (*cp == '-') { cp++; return -p_base(); }
  if (*cp == '+') { cp++; return p_base(); }
  return p_num();
}

static int p_factor(void) {
  int base = p_base();
  if (c_err) return 0;
  while (*cp == ' ') cp++;
  if (*cp == '^') {
    cp++;
    int exp = p_factor();
    if (c_err) return 0;
    int r = 1;
    for (int i = 0; i < exp; i++) r *= base;
    return r;
  }
  return base;
}

static int p_term(void) {
  int v = p_factor();
  if (c_err) return 0;
  while (1) {
    while (*cp == ' ') cp++;
    if (*cp == '*') { cp++; v *= p_factor(); if (c_err) return 0; }
    else if (*cp == '/') {
      cp++;
      int d = p_factor();
      if (c_err) return 0;
      if (d == 0) { c_divzero = 1; return 0; }
      v /= d;
    }
    else break;
  }
  return v;
}

static int p_expr(void) {
  int v = p_term();
  if (c_err) return 0;
  while (1) {
    while (*cp == ' ') cp++;
    if (*cp == '+') { cp++; v += p_term(); if (c_err) return 0; }
    else if (*cp == '-') { cp++; v -= p_term(); if (c_err) return 0; }
    else break;
  }
  return v;
}

static bool try_eval_arith(const char *s, char *out, int max_len) {
  bool has_digit = false, has_op = false;
  for (int i = 0; s[i]; i++) {
    char c = s[i];
    if (c >= '0' && c <= '9') has_digit = true;
    else if (c == '+' || c == '*' || c == '/' || c == '^') has_op = true;
    else if (c == '-') {
      int j = i - 1;
      while (j >= 0 && s[j] == ' ') j--;
      if (j >= 0 && ((s[j] >= '0' && s[j] <= '9') || s[j] == ')')) has_op = true;
    }
    else if (c != ' ' && c != '\n' && c != '(' && c != ')') return false;
  }
  if (!has_digit || !has_op) return false;

  cp = s;
  c_err = 0;
  c_divzero = 0;

  int result = p_expr();
  if (c_err) return false;

  while (*cp == ' ') cp++;
  if (*cp != 0 && *cp != '\n') return false;

  if (c_divzero) {
    strncpy(out, "Erreur: Division par zero impossible !", max_len - 1);
    out[max_len - 1] = 0;
    return true;
  }

  char numbuf[16];
  int_to_str(result, numbuf);

  int expr_start = 0;
  while (s[expr_start] == ' ') expr_start++;
  int expr_end = (int)strlen(s);
  while (expr_end > expr_start && (s[expr_end - 1] == ' ' || s[expr_end - 1] == '\n')) expr_end--;

  int w = 0;
  for (int i = expr_start; i < expr_end && w < max_len - 1; i++) out[w++] = s[i];
  if (w < max_len - 1) out[w++] = ' ';
  if (w < max_len - 1) out[w++] = '=';
  if (w < max_len - 1) out[w++] = ' ';
  int k = 0;
  while (numbuf[k] && w < max_len - 1) out[w++] = numbuf[k++];
  out[w] = 0;
  return true;
}

/* ── Knowledge base & NLP ──────────────────────── */

static void numa_respond(const char *input, char *reply, int max_len) {
  char low[64];
  str_tolower(low, input);

  if (try_eval_arith(low, reply, max_len)) {
    return;
  }

  if (str_contains(low, "je m appelle ") || str_contains(low, "je m'appelle ")) {
    const char *p = strstr(low, "appelle ");
    if (p) {
      p += 8;
      int j = 0;
      while (*p && *p != ' ' && j < 23) saved_user_name[j++] = *p++;
      saved_user_name[j] = 0;
      if (saved_user_name[0] >= 'a' && saved_user_name[0] <= 'z') saved_user_name[0] -= ('a' - 'A');
    }
    strncpy(reply, "Enchante ! C'est note, je retiens ton prenom pour notre session.", max_len - 1);
  } else if (str_contains(low, "qui suis je") || str_contains(low, "mon nom") || str_contains(low, "mon prenom")) {
    if (saved_user_name[0]) {
      char tmp[128];
      int w = 0;
      const char *p1 = "Tu t'appelles ";
      while (p1[w]) tmp[w] = p1[w], w++;
      int k = 0;
      while (saved_user_name[k] && w < 100) tmp[w++] = saved_user_name[k++];
      const char *p2 = " ! Pret a etudier ?";
      k = 0;
      while (p2[k] && w < 127) tmp[w++] = p2[k++];
      tmp[w] = 0;
      strncpy(reply, tmp, max_len - 1);
    } else {
      strncpy(reply, "Tu ne me l'as pas encore dit ! Dis-moi par exemple: 'je m'appelle ...'", max_len - 1);
    }
  } else if (str_contains(low, "qui es tu") || str_contains(low, "qui t'a cree") || str_contains(low, "presentation")) {
    strncpy(reply, "Je suis NUMA (NumWorks AI), assistant autonome en C natif sur ARM Cortex-M7 ! 100% hors-ligne sans Wifi.", max_len - 1);
  } else if (str_contains(low, "bonjour") || str_contains(low, "salut") || str_contains(low, "hello") || str_contains(low, "coucou") || str_contains(low, "hey")) {
    strncpy(reply, "Bonjour ! Pret pour les maths et les sciences ? Tape 'aide' ou un mot-cle pour commencer !", max_len - 1);
  } else if (str_contains(low, "ca va") || str_contains(low, "comment vas")) {
    strncpy(reply, "Je tourne a plein regime sans surchauffer ! Que veux-tu reviser aujourd'hui ?", max_len - 1);
  } else if (str_contains(low, "merci")) {
    strncpy(reply, "Avec plaisir ! N'hesite pas si tu as d'autres questions.", max_len - 1);
  } else if (str_contains(low, "au revoir") || str_contains(low, "bye")) {
    strncpy(reply, "A bientot ! Bon courage pour tes revisions.", max_len - 1);
  } else if (str_contains(low, "aide") || str_contains(low, "menu") || str_contains(low, "commandes")) {
    strncpy(reply, "Mots-cles: 'derivees', 'primitives', 'pythagore', 'thales', 'second degre', 'trigo', 'suites', 'logarithme', 'ohm', 'gravitation', 'energie', 'gaz', 'cinematique', 'ph', 'constantes' ou un calcul !", max_len - 1);
  } else if (str_contains(low, "derivee")) {
    strncpy(reply, "Derivees: (x^n)'=n*x^(n-1) | (e^x)'=e^x | (ln x)'=1/x | (sin)'=cos | (cos)'=-sin | (tan)'=1/cos^2 | (u*v)'=u'v+uv' | (u/v)'=(u'v-uv')/v^2 | (u^n)'=n*u^(n-1)*u'", max_len - 1);
  } else if (str_contains(low, "primitive") || str_contains(low, "integrale")) {
    strncpy(reply, "Primitives: x^n -> x^(n+1)/(n+1) | 1/x -> ln|x| | e^x -> e^x | e^(ax) -> (1/a)e^(ax) | cos x -> sin x | sin x -> -cos x | 1/(1+x^2) -> arctan x", max_len - 1);
  } else if (str_contains(low, "pythagore")) {
    strncpy(reply, "Pythagore: Triangle rectangle ABC en A -> BC^2 = AB^2 + AC^2. Hypotenuse = sqrt(a^2 + b^2). Reciproque: si BC^2=AB^2+AC^2 alors rectangle en A.", max_len - 1);
  } else if (str_contains(low, "thales")) {
    strncpy(reply, "Thales: Droites (BC)//(DE) coupant (AB) et (AC) -> AB/AD = AC/AE = BC/DE.", max_len - 1);
  } else if (str_contains(low, "second degre") || str_contains(low, "delta") || str_contains(low, "discriminant")) {
    strncpy(reply, "ax^2 + bx + c = 0. Delta = b^2 - 4ac. D>0: x1,2 = (-b +/- sqrt(D))/(2a). D=0: x0 = -b/(2a). D<0: pas de racine reelle. Somme: -b/a | Produit: c/a", max_len - 1);
  } else if (str_contains(low, "trigo") || str_contains(low, "cos") || str_contains(low, "sin")) {
    strncpy(reply, "Trigo: cos^2+sin^2=1 | tan=sin/cos | cos(a+b)=cos a cos b - sin a sin b | sin(a+b)=sin a cos b + cos a sin b | sin(2x)=2sin x cos x", max_len - 1);
  } else if (str_contains(low, "suite")) {
    strncpy(reply, "Suites: Arithmetique Un=U0+n*r, Somme=(n+1)(U0+Un)/2. Geometrique Un=U0*q^n, Somme=U0*(1-q^(n+1))/(1-q). Somme infinie (|q|<1): U0/(1-q).", max_len - 1);
  } else if (str_contains(low, "logarithme") || str_contains(low, "ln") || str_contains(low, "exponentielle")) {
    strncpy(reply, "Logarithme: ln(a*b)=ln a+ln b | ln(a/b)=ln a-ln b | ln(a^n)=n*ln a | log(x)=ln x/ln 10 | e^(a+b)=e^a*e^b | ln(e^x)=x", max_len - 1);
  } else if (str_contains(low, "ohm") || str_contains(low, "electricite") || str_contains(low, "tension")) {
    strncpy(reply, "Loi d'Ohm: U = R * I (V, Ohm, A). Puissance: P = U * I = R * I^2 (W). Energie: E = P * t (J). Noeud: Somme I = 0. Maille: Somme U = 0.", max_len - 1);
  } else if (str_contains(low, "gravitation") || str_contains(low, "newton")) {
    strncpy(reply, "Gravitation (Newton): F = G * (m1 * m2) / d^2 avec G = 6.674e-11 N.m^2/kg^2. Poids terrestre: P = m * g (g = 9.81 N/kg).", max_len - 1);
  } else if (str_contains(low, "energie") || str_contains(low, "cinetique")) {
    strncpy(reply, "Energie: Cinetique Ec = (1/2)*m*v^2 | Potentielle Ep = m*g*h | Mecanique Em = Ec + Ep (conservee sans frottement). Puissance: P = E/t.", max_len - 1);
  } else if (str_contains(low, "gaz")) {
    strncpy(reply, "Gaz parfaits: P * V = n * R * T (P en Pa, V en m^3, n en mol, R = 8.314 J/(mol.K), T en Kelvin = C + 273.15).", max_len - 1);
  } else if (str_contains(low, "constantes")) {
    strncpy(reply, "Constantes: c = 3.0e8 m/s | h = 6.626e-34 J.s | Na = 6.022e23/mol | g = 9.81 m/s^2 | R = 8.314 J/(mol.K) | G = 6.674e-11", max_len - 1);
  } else if (str_contains(low, "cinematique") || str_contains(low, "mouvement") || str_contains(low, "mrua")) {
    strncpy(reply, "Cinematique: v=d/t (MRU) | v=v0+a*t (MRUA) | d=v0*t+0.5*a*t^2 | v^2=v0^2+2*a*d | Chute libre: v=g*t, h=0.5*g*t^2", max_len - 1);
  } else if (str_contains(low, "ph") || str_contains(low, "acide") || str_contains(low, "base")) {
    strncpy(reply, "pH: pH = -log10([H3O+]) | pH<7 acide, pH=7 neutre, pH>7 basique | [H3O+] = 10^(-pH) | pH + pOH = 14 (25C)", max_len - 1);
  } else {
    strncpy(reply, "Non reconnu. Essaie: 'aide', 'derivees', 'pythagore', 'second degre', 'ohm', 'ph', 'logarithme' ou un calcul (ex: 2+3*4) !", max_len - 1);
  }
  reply[max_len - 1] = 0;
}

static void add_history(const char *q, const char *r) {
  if (hist_count < MAX_HIST) {
    strncpy(history[hist_count].q, q, 47);
    history[hist_count].q[47] = 0;
    strncpy(history[hist_count].r, r, 191);
    history[hist_count].r[191] = 0;
    hist_count++;
    cur_view = hist_count - 1;
  } else {
    for (int i = 0; i < MAX_HIST - 1; i++) {
      history[i] = history[i + 1];
    }
    strncpy(history[MAX_HIST - 1].q, q, 47);
    history[MAX_HIST - 1].q[47] = 0;
    strncpy(history[MAX_HIST - 1].r, r, 191);
    history[MAX_HIST - 1].r[191] = 0;
    cur_view = MAX_HIST - 1;
  }
}

static void scene(void *ctx) {
  (void)ctx;
  gfx_fill(0, 0, SCREEN_W, SCREEN_H, BG_COLOR);

  /* Header */
  gfx_fill(0, 0, SCREEN_W, 24, HEADER_BG);
  gfx_text(&np_font_title, 10, 18, "NUMA AI", TEXT_AI, 256);
  gfx_text(&np_font_small, 88, 17, "v2.0 - Hors-Ligne", TEXT_MUTED, 200);

  /* Badge Mode Clavier */
  if (alpha_mode) {
    gfx_rrect(248, 4, 62, 16, 4, ACCENT_AMB, 256);
    gfx_text_center(&np_font_small, 279, 16, "ALPHA", WHITE, 256);
  } else {
    gfx_rrect(248, 4, 62, 16, 4, ACCENT_GRN, 256);
    gfx_text_center(&np_font_small, 279, 16, "NUM", WHITE, 256);
  }

  /* Discussion */
  if (hist_count == 0) {
    gfx_rrect(15, 45, 290, 130, 8, CARD_AI, 256);
    gfx_text_center(&np_font_title, 160, 75, "Bienvenue sur NUMA !", TEXT_AI, 256);
    gfx_paragraph(&np_font_body, 160, 100, 260, 16, "Ton assistant scientifique hors-ligne. Pose une question en maths, physique, chimie ou entre un calcul !", WHITE, 256);
    gfx_text_center(&np_font_small, 160, 155, "Appuie sur les fleches Droite/Gauche pour des exemples", ACCENT_AMB, 256);
  } else {
    Entry *e = &history[cur_view];
    /* Bulle utilisateur */
    gfx_rrect(15, 30, 290, 32, 6, CARD_USER, 256);
    gfx_text(&np_font_body, 22, 50, "Toi :", TEXT_USER, 256);
    gfx_text(&np_font_body, 60, 50, e->q, WHITE, 256);

    /* Bulle NUMA */
    gfx_rrect(15, 68, 290, 118, 6, CARD_AI, 256);
    gfx_text(&np_font_body, 22, 88, "NUMA :", TEXT_AI, 256);
    gfx_paragraph(&np_font_body, 160, 108, 260, 15, e->r, WHITE, 256);

    /* Indicateur de defilement si plusieurs messages */
    if (hist_count > 1) {
      char ind[16] = "[ / ]";
      ind[1] = '1' + cur_view;
      ind[3] = '0' + hist_count;
      gfx_text_right(&np_font_small, 300, 180, ind, TEXT_MUTED, 200);
    }
  }

  /* Barre de saisie */
  gfx_rrect(10, 192, 300, 26, 6, CARD_USER, 256);
  gfx_rrect_outline(10, 192, 300, 26, 6, 1, alpha_mode ? ACCENT_AMB : TEXT_AI, 256);

  if (user_len > 0) {
    char disp[70];
    int l = user_len < 60 ? user_len : 60;
    memcpy(disp, user_buf, l);
    disp[l] = '_';
    disp[l + 1] = 0;
    gfx_text(&np_font_body, 18, 210, disp, WHITE, 256);
  } else {
    gfx_text(&np_font_body, 18, 210, "Tape ou fleches pour exemples...", TEXT_MUTED, 180);
  }

  /* Pied de page */
  gfx_text_center(&np_font_small, 160, 234, "ALPHA: Mode | OK: Envoyer | Fleches: Suggs/Scroll | Home: Quitter", TEXT_MUTED, 200);
}

int main(void) {
  np_app_begin();
  color_t *their_strip = gfx_buf;
  gfx_buf = strip;

  uint64_t held = eadk_keyboard_scan();
  for (bool quit = false; !quit;) {
    uint64_t k = eadk_keyboard_scan(), hit = k & ~held;
    held = k;

    if (k & (KEY(K_HOME) | KEY(K_ONOFF))) break;

    /* Touche ALPHA : bascule de mode */
    if (hit & KEY(K_ALPHA)) {
      alpha_mode = !alpha_mode;
    }

    /* Touche BACKSPACE : effacer */
    if (hit & KEY(K_BACKSPACE)) {
      if (user_len > 0) {
        user_buf[--user_len] = 0;
      }
    }

    /* Touche BACK : vider la ligne */
    if (hit & KEY(K_BACK)) {
      user_len = 0;
      user_buf[0] = 0;
    }

    /* Touches HAUT / BAS : defilement de l'historique */
    if (hit & KEY(K_UP)) {
      if (cur_view > 0) cur_view--;
    }
    if (hit & KEY(K_DOWN)) {
      if (cur_view < hist_count - 1) cur_view++;
    }

    /* Touches GAUCHE / DROITE : suggestions rapides si champ vide */
    if (user_len == 0) {
      if (hit & KEY(K_RIGHT)) {
        sugg_idx = (sugg_idx + 1) % NUM_SUGG;
        strncpy(user_buf, suggestions[sugg_idx], 60);
        user_len = (int)strlen(user_buf);
      } else if (hit & KEY(K_LEFT)) {
        sugg_idx = (sugg_idx + NUM_SUGG - 1) % NUM_SUGG;
        strncpy(user_buf, suggestions[sugg_idx], 60);
        user_len = (int)strlen(user_buf);
      }
    }

    /* Saisie alphanumerique */
    char c = 0;
    if (alpha_mode) {
      if (hit & KEY(K_EXP)) c = 'a';
      else if (hit & KEY(K_LN)) c = 'b';
      else if (hit & KEY(K_LOG)) c = 'c';
      else if (hit & KEY(K_I)) c = 'd';
      else if (hit & KEY(K_COMMA)) c = 'e';
      else if (hit & KEY(K_POWER)) c = 'f';
      else if (hit & KEY(K_SIN)) c = 'g';
      else if (hit & KEY(K_COS)) c = 'h';
      else if (hit & KEY(K_TAN)) c = 'i';
      else if (hit & KEY(K_PI)) c = 'j';
      else if (hit & KEY(K_SQRT)) c = 'k';
      else if (hit & KEY(K_SQUARE)) c = 'l';
      else if (hit & KEY(K_7)) c = 'm';
      else if (hit & KEY(K_8)) c = 'n';
      else if (hit & KEY(K_9)) c = 'o';
      else if (hit & KEY(K_LPAREN)) c = 'p';
      else if (hit & KEY(K_RPAREN)) c = 'q';
      else if (hit & KEY(K_4)) c = 'r';
      else if (hit & KEY(K_5)) c = 's';
      else if (hit & KEY(K_6)) c = 't';
      else if (hit & KEY(K_MUL)) c = 'u';
      else if (hit & KEY(K_DIV)) c = 'v';
      else if (hit & KEY(K_1)) c = 'w';
      else if (hit & KEY(K_2)) c = 'x';
      else if (hit & KEY(K_3)) c = 'y';
      else if (hit & KEY(K_PLUS)) c = 'z';
      else if (hit & KEY(K_MINUS)) c = ' ';
      else if (hit & KEY(K_0)) c = ' ';
      else if (hit & KEY(K_DOT)) c = '?';
      else if (hit & KEY(K_XNT)) c = 'x';
    } else {
      if (hit & KEY(K_0)) c = '0';
      else if (hit & KEY(K_1)) c = '1';
      else if (hit & KEY(K_2)) c = '2';
      else if (hit & KEY(K_3)) c = '3';
      else if (hit & KEY(K_4)) c = '4';
      else if (hit & KEY(K_5)) c = '5';
      else if (hit & KEY(K_6)) c = '6';
      else if (hit & KEY(K_7)) c = '7';
      else if (hit & KEY(K_8)) c = '8';
      else if (hit & KEY(K_9)) c = '9';
      else if (hit & KEY(K_PLUS)) c = '+';
      else if (hit & KEY(K_MINUS)) c = '-';
      else if (hit & KEY(K_MUL)) c = '*';
      else if (hit & KEY(K_DIV)) c = '/';
      else if (hit & KEY(K_DOT)) c = '.';
      else if (hit & KEY(K_LPAREN)) c = '(';
      else if (hit & KEY(K_RPAREN)) c = ')';
      else if (hit & KEY(K_POWER)) c = '^';
      else if (hit & KEY(K_XNT)) c = 'x';
    }

    if (c && user_len < 60) {
      user_buf[user_len++] = c;
      user_buf[user_len] = 0;
    }

    /* Envoi avec OK ou EXE */
    if (hit & (KEY(K_OK) | KEY(K_EXE))) {
      if (user_len > 0) {
        char rep[192];
        numa_respond(user_buf, rep, sizeof(rep));
        add_history(user_buf, rep);
        user_len = 0;
        user_buf[0] = 0;
      }
    }

    gfx_render(scene, 0, 0, SCREEN_H);
    eadk_timing_msleep(TICK_MS);
  }

  gfx_buf = their_strip;
  return np_app_end();
}
