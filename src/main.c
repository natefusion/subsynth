#include "raylib.h"

#include <stdlib.h>         // Required for: malloc(), free()
#include <math.h>           // Required for: sinf()

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#define MAX_SAMPLES_PER_UPDATE   4096

#define TO_STR(x) #x

float frequency = 8.0f;
float amplitude = 32000.0f;
float sample_rate = 44100.0f;
float volume = 0.05;
float sineIdx = 0.0f; // Index for audio rendering
int screenWidth = 1000;
int screenHeight = 1000;

enum Envelope_Parameters {
    ENV_TIME,
    ENV_LOOP,
    ENV_TILT,
    ENV_KF,
    VOL_ATK,
    VOL_DCY,
    VOL_SUS,
    VOL_FADE,
    MOD_ATK,
    MOD_DCY,
    MOD_SH,
    MOD_VEL,
    LFO_RATE,
    LFO_AMT,
    LFO_BAL,
    LFO_DLY,

    ENVELOPE_LEN,
};

enum Oscillator_Parameters {
    A_FORM,
    A_NOISE,
    A_MOD,
    A_COLOR,
    A_FREQ,
    FM_MOD,
    FM_AMT,
    MIX_MOD,
    OSC_MIX,
    B_FORM,
    B_NOISE,
    B_MOD,
    SUB_AM,
    B_FREQ,
    B_SH,

    OSCILLATOR_LEN,
};

enum Filter_Parameters {
    FLT_TYPE,
    FLT_Q,
    FLT_MOD,
    FLT_SEP,
    FLT_FREQ,
    FLT_KF,
    SATURATE,
    RVB_MIX,
    RVB_ATK,
    RVB_LEN,
    RVB_DAMP,
    RVB_CHOR,
    RVB_SIZE,
    ADJ_BASS,
    ADJ_TREB,
    ADJ_PAN,
    ADJ_CLIP,
    
    FILTER_LEN,
};

enum Page {
  ENVELOPE,
  OSCILLATOR,
  FILTER,
  MIXED,
};

#define PARAMS_LEN ENVELOPE_LEN + OSCILLATOR_LEN + FILTER_LEN

union Params {
    float p[PARAMS_LEN];
    
    struct {
        float env[ENVELOPE_LEN];
        float osc[OSCILLATOR_LEN];
        float fil[FILTER_LEN];
    };

    struct {
        struct {
            float env_time;
            float env_loop;
            float env_tilt;
            float env_kf;
            float vol_atk;
            float vol_dcy;
            float vol_sus;
            float vol_fade;
            float mod_atk;
            float mod_dcy;
            float mod_sh;
            float mod_vel;
            float lfo_rate;
            float lfo_amt;
            float lfo_bal;
            float lfo_dly;
        };

        struct {
            float a_form;
            float a_noise;
            float a_mod;
            float a_color;
            float a_freq;
            float fm_mod;
            float fm_amt;
            float mix_mod;
            float osc_mix;
            float b_form;
            float b_noise;
            float b_mod;
            float sub_am;
            float b_freq;
            float b_sh;
        };

        struct {
            float flt_type;
            float flt_q;
            float flt_mod;
            float flt_sep;
            float flt_freq;
            float flt_kf;
            float saturate;
            float rvb_mix;
            float rvb_atk;
            float rvb_len;
            float rvb_damp;
            float rvb_chor;
            float rvb_size;
            float adj_bass;
            float adj_treb;
            float adj_pan;
            float adj_clip;
            float params_len;
        };
    };
} params = {{0}};
    /* #include "params.txt" */
    /* ; */

const char *envelope_tostr(enum Envelope_Parameters p) {
    switch (p) {
    case ENV_TIME: return "ENV_TIME";
    case ENV_LOOP: return "ENV_LOOP";
    case ENV_TILT: return "ENV_TILT";
    case ENV_KF: return "ENV_KF";
    case VOL_ATK: return "VOL_ATK";
    case VOL_DCY: return "VOL_DCY";
    case VOL_SUS: return "VOL_SUS";
    case VOL_FADE: return "VOL_FADE";
    case MOD_ATK: return "MOD_ATK";
    case MOD_DCY: return "MOD_DCY";
    case MOD_SH: return "MOD_SH";
    case MOD_VEL: return "MOD_VEL";
    case LFO_RATE: return "LFO_RATE";
    case LFO_AMT: return "LFO_AMT";
    case LFO_BAL: return "LFO_BAL";
    case LFO_DLY: return "LFO_DLY";
    case ENVELOPE_LEN: return "";
    }
    return "";
}

const char *oscillator_tostr(enum Oscillator_Parameters p) {
    switch (p) {
    case A_FORM: return "A_FORM";
    case A_NOISE: return "A_NOISE";
    case A_MOD: return "A_MOD";
    case A_COLOR: return "A_COLOR";
    case A_FREQ: return "A_FREQ";
    case FM_MOD: return "FM_MOD";
    case FM_AMT: return "FM_AMT";
    case MIX_MOD: return "MIX_MOD";
    case OSC_MIX: return "OSC_MIX";
    case B_FORM: return "B_FORM";
    case B_NOISE: return "B_NOISE";
    case B_MOD: return "B_MOD";
    case SUB_AM: return "SUB_AM";
    case B_FREQ: return "B_FREQ";
    case B_SH: return "B_SH";
    case OSCILLATOR_LEN: return "";
    }
    return "";
}

const char *filter_tostr(enum Filter_Parameters p) {
    switch (p) {
    case FLT_TYPE: return "FLT_TYPE";
    case FLT_Q: return "FLT_Q";
    case FLT_MOD: return "FLT_MOD";
    case FLT_SEP: return "FLT_SEP";
    case FLT_FREQ: return "FLT_FREQ";
    case FLT_KF: return "FLT_KF";
    case SATURATE: return "SATURATE";
    case RVB_MIX: return "RVB_MIX";
    case RVB_ATK: return "RVB_ATK";
    case RVB_LEN: return "RVB_LEN";
    case RVB_DAMP: return "RVB_DAMP";
    case RVB_CHOR: return "RVB_CHOIR";
    case RVB_SIZE: return "RVB_SIZE";
    case ADJ_BASS: return "ADJ_BASS";
    case ADJ_TREB: return "ADJ_TREB";
    case ADJ_PAN: return "ADJ_PAN";
    case ADJ_CLIP: return "ADJ_CLIP";
    case FILTER_LEN: return "";
    }
    return "";
}

void write_params(const char* filepath) {
    const char *s = "{"
        ".env_time = %.2f,"
        ".env_loop = %.2f,"
        ".env_tilt = %.2f,"
        ".env_kf = %.2f,"
        ".vol_atk = %.2f,"
        ".vol_dcy = %.2f,"
        ".vol_sus = %.2f,"
        ".vol_fade = %.2f,"
        ".mod_atk = %.2f,"
        ".mod_dcy = %.2f,"
        ".mod_sh = %.2f,"
        ".mod_vel = %.2f,"
        ".lfo_rate = %.2f,"
        ".lfo_amt = %.2f,"
        ".lfo_bal = %.2f,"
        ".lfo_dly = %.2f,"
        ".a_form = %.2f,"
        ".a_noise = %.2f,"
        ".a_mod = %.2f,"
        ".a_color = %.2f,"
        ".a_freq = %.2f,"
        ".fm_mod = %.2f,"
        ".fm_amt = %.2f,"
        ".mix_mod = %.2f,"
        ".osc_mix = %.2f,"
        ".b_form = %.2f,"
        ".b_noise = %.2f,"
        ".b_mod = %.2f,"
        ".sub_am = %.2f,"
        ".b_freq = %.2f,"
        ".b_sh = %.2f,"
        ".flt_type = %.2f,"
        ".flt_q = %.2f,"
        ".flt_mod = %.2f,"
        ".flt_sep = %.2f,"
        ".flt_freq = %.2f,"
        ".flt_kf = %.2f,"
        ".saturate = %.2f,"
        ".rvb_mix = %.2f,"
        ".rvb_atk = %.2f,"
        ".rvb_len = %.2f,"
        ".rvb_damp = %.2f,"
        ".rvb_chor = %.2f,"
        ".rvb_size = %.2f,"
        ".adj_bass = %.2f,"
        ".adj_treb = %.2f,"
        ".adj_pan = %.2f,"
        ".adj_clip = %.2f,"
        ".params_len = %.2f,"
        "}"
        "\n";
    
    FILE *fp;
    errno_t err = fopen_s(&fp, filepath, "w");
    if (!err) {
        fprintf(fp, s,
                params.env_time,
                params.env_loop,
                params.env_tilt,
                params.env_kf,
                params.vol_atk,
                params.vol_dcy,
                params.vol_sus,
                params.vol_fade,
                params.mod_atk,
                params.mod_dcy,
                params.mod_sh,
                params.mod_vel,
                params.lfo_rate,
                params.lfo_amt,
                params.lfo_bal,
                params.lfo_dly,
                params.a_form,
                params.a_noise,
                params.a_mod,
                params.a_color,
                params.a_freq,
                params.fm_mod,
                params.fm_amt,
                params.mix_mod,
                params.osc_mix,
                params.b_form,
                params.b_noise,
                params.b_mod,
                params.sub_am,
                params.b_freq,
                params.b_sh,
                params.flt_type,
                params.flt_q,
                params.flt_mod,
                params.flt_sep,
                params.flt_freq,
                params.flt_kf,
                params.saturate,
                params.rvb_mix,
                params.rvb_atk,
                params.rvb_len,
                params.rvb_damp,
                params.rvb_chor,
                params.rvb_size,
                params.adj_bass,
                params.adj_treb,
                params.adj_pan,
                params.adj_clip,
                params.params_len
            );
    } else {
        fprintf(stderr, "ERR: %d\n", err);
    }
    
    fclose(fp);
}

void read_params(const char *filepath) {
    FILE* fp;
    int idx = 0;
    char buf[6] = {0};
    errno_t err = fopen_s(&fp, filepath, "r");
    printf("%s", "Reading params ... \n");
    if (!err && fp) {
        char c = fgetc(fp);
        if (c != '{') {
            printf("%s", "The file is wrong!\n");
        } else {
            while ((c = fgetc(fp)) != EOF) {
                if (c == '.') {
                    continue;
                } else if (c == '=') {
                    c = fgetc(fp); // consume space

                    float n;
                    int result = fscanf_s(fp, "%f", &n);
                    if (result == EOF) {
                        printf("Bad number %s\n", buf);
                        break;
                    } else {
                        printf("Got %f \n", n);
                    }
                    params.p[idx] = n; 
                    idx += 1;

                    c = fgetc(fp); // consume comma
                } else if (c == '}') {
                    printf("%s", "All Done!\n");
                    break;
                }
            }
        }
    } else {
        printf("Can't read file: %d\n", err);
    }

    fclose(fp);
}

float identity(float x) { return x; }
float map(float x, float in_min, float in_max, float out_min, float out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

float volume_envelope(float x) {
    float a = x / sqrtf(params.env_loop);
    x = (a - floor(a)) / params.env_time;
    
    float env_tilt = powf(params.env_tilt, 4.0f);
    
    if (x >= 0 && x <= env_tilt) {
        return powf(x/env_tilt, powf(0.5 + params.vol_atk, 15.0f));
    } else {
        return 1 - powf((x - env_tilt)/(1 - env_tilt), powf(0.5 + params.vol_dcy, 15.0f));
    }
}

float modulation_envelope(float x) {
    float a = x / sqrtf(params.env_loop);
    x = (a - floor(a)) / params.env_time;
    
    float env_tilt = powf(params.env_tilt, 4.0f);
    
    if (x >= 0 && x <= env_tilt) {
        return powf(x/env_tilt, powf(0.5 + params.mod_atk, 15.0f));
    } else {
        return 1 - powf((x - env_tilt)/(1 - env_tilt), powf(0.5 + params.mod_dcy, 15.0f));
    }
}

float pitch_lfo(float x) {
    float amplitude =
        powf(2.0f * (params.lfo_amt - 0.5f), 2.0f)
        * tanhf(fabsf(x) / (5.0f * params.lfo_dly))
        * (params.lfo_bal < 0.5f ? 1.0f : -2.0f * params.lfo_bal + 2.0f);
    
    x = x * (1.0f + params.lfo_rate);
    float triangle = 4.0f * fabsf(x - floorf(x + 0.5f)) - 1.0f;

    return amplitude * triangle;
}

float volume_lfo(float x) {
    float amplitude =
        1.2f * powf(2.0f * (params.lfo_amt - 0.5f), 2.0f)
        * tanhf(fabsf(x) / (5.0f * params.lfo_dly))
        * (params.lfo_bal > 0.5f ? 1.0f : 2.0f * params.lfo_bal);
    
    x = x * (1.0f + params.lfo_rate);
    float triangle = 4.0f * fabsf(x - floorf(x + 0.5f)) - 1.0f;

    float y = amplitude * triangle;

    if (y < -1.0f) y = -1.0f;
    if (y > 1.0f) y = 1.0f;
    return y;
}

float oscillator(float x, float form) {
    if (form >= 0 && form <= 0.57) {
        float sinx = sinf(2.0f * PI * frequency * x);
        float sawx = tanhf(PI * sinf(frequency * x) / 2.0f) * (1.0f + cosf(frequency * x)) / 2.0f;
        float i = map(form, 0.0f, 0.57f, 0.0f, 1.0f);
        return (1 - i) * sinx + i * sawx;
    } else if (form > 0.57 && form <= 0.81) {
        float shape = 1.0f - map(form, 0.57f, 0.81f, 0.0f, 1.0f);
        return (1.0f - shape / 2.0f) * tanhf(PI * sin(frequency * x) / 2.0f) * (1.0f + shape * cosf(frequency * x));
    } else {
        float duty = 1.0f - map(form, 0.81f, 1.0f, 0.0f, 1.0f);
        return tanhf(2.0f * PI *
                     (tanhf(PI * sinf(frequency *  x)              / 2.0f) * (1.0f + cosf(frequency *  x))             / 2.0f) -
                     (tanhf(PI * sinf(frequency * (x - PI * duty)) / 2.0f) * (1.0f + cosf(frequency * (x - PI * duty)) / 2.0f)));
    }
}

float chain(float x) {
    return (1.0f - params.osc_mix) * oscillator(x, params.a_form) + params.osc_mix * oscillator(x, params.b_form);
}

/* void AudioInputCallback(void *buffer, unsigned int frames) { */
/*     short *d = (short *)buffer; */

/*     for (unsigned int i = 0; i < frames; i++) { */
/*         d[i] = (short)(sine(sineIdx) * amplitude); */
/*         sineIdx += 1/sample_rate; */
/*         if (sineIdx > 1.0f) sineIdx = 0.0f; */
/*     } */
/* } */

void DrawPlot(Rectangle bounds, float *data, Color color) {
    GuiDrawRectangle(bounds, 1, BLACK, RAYWHITE);

    /* bounds.width -= 1; */
    /* /\* bounds.height += 1; *\/ */
    /* bounds.y -= 1; */
    /* bounds.x += 1; */

    Vector2 pv = {.x = bounds.x, .y = bounds.y + bounds.height - (bounds.height / screenHeight) * data[0] };
    Vector2 v = pv;
    for (int i = 1; i < bounds.width; ++i) {
        int x = i * screenWidth / bounds.width;
        float y = data[x];
        pv.x = v.x;
        pv.y = v.y;
        v.x = bounds.x + i;
        v.y = bounds.y + bounds.height - (bounds.height / screenHeight)*y;
        if (v.y < bounds.y) v.y = bounds.y;
        if (v.y > bounds.y + bounds.height) v.y = bounds.y + bounds.height;
        DrawLine(pv.x, pv.y, v.x, v.y, color);
    }
}

enum Plots {
    PLOT_ONE,
    PLOT_TWO,
    PLOT_THREE,
    PLOT_FOUR,
    PLOT_FIVE,
    MAX_PLOTS,
};

int main(int argc, char *argv[]) {
    const char *filepath = argc > 1 ? argv[1] : "params.txt";

    read_params(filepath);
        
    InitWindow(screenWidth, screenHeight, "raylib [audio] example - raw audio streaming");
    int h_fps = 144;
    int l_fps = 6;
    int cur_fps = h_fps;
    SetTargetFPS(cur_fps);

    float *buffer = (float*)calloc(screenWidth*MAX_PLOTS, sizeof(float));
    float *plot[MAX_PLOTS];
    for (int i = 0; i < MAX_PLOTS;  ++i) {
        plot[i] = buffer + screenWidth*i;
    }

    enum Page page = ENVELOPE;
    enum Page prevPage = ENVELOPE;
    bool redraw = true;

    while (!WindowShouldClose()) {
        /* Update */
        if (prevPage != page || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            redraw = true;
        }

        if (IsWindowFocused()) {
            if (cur_fps != h_fps) {
                cur_fps = h_fps;
                SetTargetFPS(cur_fps);
            }
        } else {
            redraw = false;
            if (cur_fps != l_fps) {
                cur_fps = l_fps;
                SetTargetFPS(cur_fps);
            }
        }

        switch (page) {
        case ENVELOPE: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    plot[PLOT_ONE][i] = volume_envelope((float)i / screenWidth) * screenHeight; 
                    plot[PLOT_TWO][i] = modulation_envelope((float)i / screenWidth) * screenHeight;
                    plot[PLOT_THREE][i] = (pitch_lfo(4.0f * (float)i / screenWidth) + 2.0f) * screenHeight / 4.0f;
                    plot[PLOT_FOUR][i] = (volume_lfo(4.0f * (float)i / screenWidth) + 2.0f) * screenHeight / 4.0f;
                    plot[PLOT_FIVE][i] = (chain(4.0f * (float)i / screenWidth) + 2.0f) * screenHeight / 4.0f;
                }
                redraw = false;
            }
        } break;
        case OSCILLATOR: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    plot[PLOT_ONE][i] = (oscillator(4.0f * (float)i / screenWidth, params.a_form) + 2.0f) * screenHeight / 4.0f;
                    plot[PLOT_TWO][i] = (oscillator(4.0f * (float)i / screenWidth, params.b_form) + 2.0f) * screenHeight / 4.0f;
                    plot[PLOT_FIVE][i] = (chain(4.0f * (float)i / screenWidth) + 2.0f) * screenHeight / 4.0f;
                }
                redraw = false;
            }
        } break;
        case FILTER: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    plot[PLOT_ONE][i] = 0;
                    plot[PLOT_TWO][i] = 0;
                    plot[PLOT_THREE][i] = 0;
                    plot[PLOT_FOUR][i] = 0;
                    plot[PLOT_FIVE][i] = (chain(4.0f * (float)i / screenWidth) + 2.0f) * screenHeight / 4.0;
                }
                redraw = false;
            }
        } break;
        case MIXED: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    plot[PLOT_FIVE][i] = (chain(4.0f * (float)i / screenWidth) + 2.0f) * screenHeight / 4.0;
                }
                redraw = false;
            }
        } break;
        }

        /* Draw */
        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        Rectangle r = {.width = 100, .height = 20, .x = 30, .y = 0};

        prevPage = page;
        GuiToggleGroup(r, "Envelope;Oscillator;Filter;Mixed", (int *)&page);
        GuiSlider((Rectangle) { .width = 200, .height = r.height, .x = (2 + r.width)*4 + r.x + 30, .y = r.y}, TextFormat("%.2f", frequency), "Frequency", &frequency, 1.0f, 40.0f);

        switch (page) {
        case ENVELOPE: {
            for (int i = 0; i < ENVELOPE_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.env[i]), envelope_tostr(i), &params.env[i], 0.0f, 1.0f);
            }

            r.x = r.width*2;
            r.y = 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, plot[PLOT_ONE], RED);

            r.y += r.height + 2;
            DrawPlot(r, plot[PLOT_TWO], RED);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, plot[PLOT_FIVE], RED);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, plot[PLOT_THREE], RED);

            r.y += r.height + 2;
            DrawPlot(r, plot[PLOT_FOUR], RED);
        } break;
        case OSCILLATOR: {
            for (int i = 0; i < OSCILLATOR_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.osc[i]), oscillator_tostr(i), &params.osc[i], 0.0f, 1.0f);
            }

            r.x = r.width*2;
            r.y = 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, plot[PLOT_ONE], RED);

            r.y += r.height + 2;
            DrawPlot(r, plot[PLOT_TWO], RED);

            r.x += r.width + 2;
            DrawPlot(r, plot[PLOT_FIVE], RED);
        } break;
        case FILTER: {
            for (int i = 0; i < FILTER_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.fil[i]), filter_tostr(i), &params.fil[i], 0.0f, 1.0f);
            }

            r.x = r.width*2;
            r.y = 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, plot[PLOT_ONE], RED);

            r.y += r.height + 2;
            DrawPlot(r, plot[PLOT_TWO], RED);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, plot[PLOT_FIVE], RED);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, plot[PLOT_THREE], RED);

            r.y += r.height + 2;
            DrawPlot(r, plot[PLOT_FOUR], RED);
        } break;
        case MIXED: {
            r.x = 2;
            r.y = 22;
            r.width = screenWidth - 4;
            r.height = screenHeight - 24;

            DrawPlot(r, plot[PLOT_FIVE], RED);
        } break;
        }

        EndDrawing();
    }

    write_params(filepath);
}
