#include <stdio.h>
#include <math.h>           // Required for: sinf()
#include <stdlib.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#define MAX_SAMPLES_PER_UPDATE   4096

#define TO_STR(x) #x

#define TAU (2*PI)

float midi_f0 = 60.0f;
float frequency = 8.0f;
float sample_rate = 44100.0f;
float length_seconds = 16.0f;
float time_seconds = 0.0f;
float volume = 0.2f;
int screenWidth = 1000;
int screenHeight = 1000;
int sample_size = 16;
int channels = 1;

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
  OPTIONS,
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

const char *envelope_tostr(enum Envelope_Parameters p) {
    switch (p) {
    case ENV_TIME: return "env_time";
    case ENV_LOOP: return "env_loop";
    case ENV_TILT: return "env_tilt";
    case ENV_KF: return "env_kf";
    case VOL_ATK: return "vol_atk";
    case VOL_DCY: return "vol_dcy";
    case VOL_SUS: return "vol_sus";
    case VOL_FADE: return "vol_fade";
    case MOD_ATK: return "mod_atk";
    case MOD_DCY: return "mod_dcy";
    case MOD_SH: return "mod_sh";
    case MOD_VEL: return "mod_vel";
    case LFO_RATE: return "lfo_rate";
    case LFO_AMT: return "lfo_amt";
    case LFO_BAL: return "lfo_bal";
    case LFO_DLY: return "lfo_dly";
    case ENVELOPE_LEN: return "";
    }
    return "";
}

const char *oscillator_tostr(enum Oscillator_Parameters p) {
    switch (p) {
    case A_FORM: return "a_form";
    case A_NOISE: return "a_noise";
    case A_MOD: return "a_mod";
    case A_COLOR: return "a_color";
    case A_FREQ: return "a_freq";
    case FM_MOD: return "fm_mod";
    case FM_AMT: return "fm_amt";
    case MIX_MOD: return "mix_mod";
    case OSC_MIX: return "osc_mix";
    case B_FORM: return "b_form";
    case B_NOISE: return "b_noise";
    case B_MOD: return "b_mod";
    case SUB_AM: return "sub_am";
    case B_FREQ: return "b_freq";
    case B_SH: return "b_sh";
    case OSCILLATOR_LEN: return "";
    }
    return "";
}

const char *filter_tostr(enum Filter_Parameters p) {
    switch (p) {
    case FLT_TYPE: return "flt_type";
    case FLT_Q: return "flt_q";
    case FLT_MOD: return "flt_mod";
    case FLT_SEP: return "flt_sep";
    case FLT_FREQ: return "flt_freq";
    case FLT_KF: return "flt_kf";
    case SATURATE: return "saturate";
    case RVB_MIX: return "rvb_mix";
    case RVB_ATK: return "rvb_atk";
    case RVB_LEN: return "rvb_len";
    case RVB_DAMP: return "rvb_damp";
    case RVB_CHOR: return "rvb_choir";
    case RVB_SIZE: return "rvb_size";
    case ADJ_BASS: return "adj_bass";
    case ADJ_TREB: return "adj_treb";
    case ADJ_PAN: return "adj_pan";
    case ADJ_CLIP: return "adj_clip";
    case FILTER_LEN: return "";
    }
    return "";
}

void write_params(const char* filepath) {
    FILE *fp = fopen(filepath, "w");
    if (fp) {
        fputc('{', fp);
        
        for (int i = 0; i < ENVELOPE_LEN; ++i) {
            fprintf(fp, ".%s = %.2f,", envelope_tostr(i), params.env[i]);
        }

        for (int i = 0; i < OSCILLATOR_LEN; ++i) {
            fprintf(fp, ".%s = %.2f,", oscillator_tostr(i), params.osc[i]);
        }

        for (int i = 0; i < FILTER_LEN; ++i) {
            fprintf(fp, ".%s = %.2f,", filter_tostr(i), params.fil[i]);
        }

        fprintf(fp, "}\n");
        
        fclose(fp);
    } else {
        fprintf(stderr, "Could not open: %s\n", filepath);
    }
}

void read_params(const char *filepath) {
    int idx = 0;
    FILE *fp = fopen(filepath, "r");
    printf("%s", "Reading params ... ");
    if (fp) {
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
                    int result = fscanf(fp, "%f", &n);
                    if (result == EOF) {
                        printf("Bad number\n");
                        break;
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

        fclose(fp);
    } else {
        printf("Can't read file\n");
    }
}

typedef struct {
    float *data;
    int size;
    int capacity;
    float sample_rate;
} Signal;

Signal scratch = {0};

float clampf(float x, float min, float max) {
    if (x < min) return min;
    else if (x > max) return max;
    else return x;
}

float identity(float x) { return x; }
float map(float x, float in_min, float in_max, float out_min, float out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

float midi_to_hz(float midi) {
    return powf(2.0f, (midi - 69.0f)/12.0f) * 440.0f;
}

void make_vco_argument(float midi_f0, Signal *s) {
    if (s->size == 0) {
        s->data[0] = midi_to_hz(midi_f0);
        s->size = s->capacity;
        float prev = 0;
        for (int i = 1; i < s->size; ++i) {
            float cur = prev + TAU * s->data[0] / s->sample_rate;
            s->data[i] = cur;
            prev = cur;
        }
    } else {
        int mod_depth = 0; // me no understand purpose
        float prev = 0;
        for (int i = 0; i < s->size; ++i) {
            float cur = prev + TAU * midi_to_hz(clampf(midi_f0 + mod_depth*s->data[i], 0.0f, 127.0f)) / s->sample_rate;
            s->data[i] = cur;
            prev = cur;
        }
    }
}

void square_saw(float midi_f0, Signal *argument) {
    make_vco_argument(midi_f0, argument);
    
    // mod_depth is used here too in torchsynth
    float max_f0 = midi_to_hz(midi_f0);
    float partials = 12000 / (max_f0 * log10f(max_f0));
    
    for (int i = 0; i < argument->size; ++i) {
        float square = tanhf(PI * partials * sinf(argument->data[i]) / 2.0f);
        argument->data[i] = (screenHeight/2.0f)*(1 - params.a_form / 2.0f) * square * (1.0f + params.a_form * cosf(argument->data[i]));
    }
}

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
        4.0f * powf((params.lfo_amt - 0.5f), 2.0f)
        * tanhf(fabsf(x) / (5.0f * params.lfo_dly))
        * (params.lfo_bal < 0.5f ? 1.0f : -2.0f * params.lfo_bal + 2.0f);
    
    x = x * (1.0f + params.lfo_rate);
    float triangle = 4.0f * fabsf(x - floorf(x + 0.5f)) - 1.0f;

    return amplitude * triangle;
}

float volume_lfo(float x) {
    x /= 2.0f;
    float amplitude =
        1.2f * powf(2.0f * (params.lfo_amt - 0.5f), 2.0f)
        * tanhf(fabsf(x) / (5.0f * params.lfo_dly))
        * (params.lfo_bal > 0.5f ? 1.0f : 2.0f * params.lfo_bal);
    
    x *= powf((1.0f + 4.0f*params.lfo_rate), 2.0f);
    float triangle = 4.0f * fabsf(x - floorf(x + 0.5f)) - 1.0f;

    return clampf(amplitude * triangle + 0.5f, 0.0f, 1.0f);
}

float oscillator(float x, float form) {
    float partials = 12000.0f / (frequency * log10f(frequency));
    if (form >= 0 && form <= 0.57) {
        float sinx = sinf(TAU * x);
        float sawx = tanhf(PI * partials * sinf(TAU * x)) * (1.0f + cosf(TAU * x)) / 2.0f;
        float i = map(form, 0.0f, 0.57f, 0.0f, 1.0f);
        return (1 - i) * sinx + i * sawx;
    } else /* if (form > 0.57 && form <= 0.81) */ {
        float shape = 1.0f - map(form, 0.57f, 1.0f/* 0.81f */, 0.0f, 1.0f);
        float square = tanh(PI * partials * sinf(TAU * x) / 2.0f);
        return (1.0f - shape / 2.0f) * square * (1.0f + shape * cosf(TAU * x));
    }
    /* else { */
    /*     float duty = map(form, 0.81f, 1.0f, 0.0f, 1.0f); */
    /*     float a = tanhf(PI * partials * sinf(TAU * frequency * x)) / 2.0f; */
    /*     float sawa = a * (1.0f + cosf(TAU * frequency * x)); */
    /*     float sawb = a * (1.0f + cosf(TAU * frequency * x + duty/4.0f)); */
    /*     return tanh(TAU * partials * (sawa - sawb)); */
    /* } */
}

float oscillator_a(float x) {
    return oscillator(x * frequency, params.a_form);
}

float oscillator_b(float x) {
    return oscillator(x * frequency, params.b_form);
}

float chain(float x) {
    float pitch = frequency + 50.0f * pitch_lfo(x);
    float a = (1.0f - params.osc_mix) * oscillator(x * pitch, params.a_form) + params.osc_mix * oscillator(x * pitch, params.b_form);
    return volume_lfo(x) * a;
}

void AudioInputCallback(void *buffer, unsigned int frames) {
    short *d = (short *)buffer;
    for (unsigned int i = 0; i < frames; ++i) {
        d[i] = (short)(chain(time_seconds) * 32000.0f);
        time_seconds += 1.0f / sample_rate;
        if (time_seconds > length_seconds) time_seconds = 0.0f;
    }
}

struct Options {
    bool export_wav;
    bool export_params;
};

enum Plot_Data {
    PLOT_ONE,
    PLOT_TWO,
    PLOT_THREE,
    PLOT_FOUR,
    PLOT_FIVE,
    MAX_PLOTS,
};

struct Plot_Metadata {
    char *name;
    float (*func)(float);
    /* Signal signal; */
    float seconds_per_plot;
    float h_scale;
    float h_shift;
    int w_shift;
};

struct Plots {
    struct Plot_Metadata volume_envelope;
    struct Plot_Metadata modulation_envelope;
    struct Plot_Metadata pitch_lfo;
    struct Plot_Metadata volume_lfo;
    struct Plot_Metadata oscillator_a;
    struct Plot_Metadata oscillator_b;
    struct Plot_Metadata filter;
    struct Plot_Metadata saturation;
    struct Plot_Metadata reverb;
    struct Plot_Metadata adjust;
    struct Plot_Metadata mixed;
};

void DrawPlot(Rectangle bounds, struct Plot_Metadata *meta, Color color) {
    GuiDrawRectangle(bounds, 1, BLACK, RAYWHITE);
    DrawText(meta->name, bounds.x + 2, bounds.y + 2, 10, GRAY);
    GuiSliderBar((Rectangle){ .x = bounds.x + 12, .y = bounds.y + bounds.height - 13, .width = 100, .height = 10 },
                 "-", TextFormat("(w) %.4f", meta->seconds_per_plot), &meta->seconds_per_plot, 0.001f, length_seconds);

    float h_offset = bounds.y + bounds.height;
    float h_ratio = bounds.height / 4.0f;
    float seconds_per_pixel = meta->seconds_per_plot / bounds.width;
    float end = fmodf(time_seconds, meta->seconds_per_plot) / seconds_per_pixel;
    
    Vector2 mp = GetMousePosition();
    int mx = (int)mp.x;
    int my = (int)mp.y;
    if (CheckCollisionPointRec(mp, bounds)) {
        int xpos = mx + 1;
        int i = mx - (int)bounds.x;
        int fontsize = 10;
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            fontsize = 20;
        }

        float value_at_point = meta->func((time_seconds - fmodf(time_seconds, meta->seconds_per_plot)) + i*seconds_per_pixel);
        const char *t = TextFormat("(%.3fs, %.3f) (%d, %d)", (time_seconds - fmodf(time_seconds, meta->seconds_per_plot)) + i*seconds_per_pixel, value_at_point, i, (int)h_offset - my - 1);
        int len = MeasureText(t, fontsize);

        int ypos = my - fontsize;
        if (ypos < bounds.y + 1) ypos = bounds.y + 1;
        if (i <= end)
            DrawLine(xpos, my, xpos, clampf(h_offset - h_ratio*(value_at_point + meta->h_shift), bounds.y, h_offset), GREEN);
        
        xpos = xpos + len > bounds.x + bounds.width - 1 ? bounds.x + bounds.width - 1 - len : xpos;
        DrawText(t, xpos, ypos, fontsize, GRAY);
    }

    Vector2 pv = {0};
    Vector2 v = {.x = bounds.x, .y = clampf(h_offset - h_ratio*(meta->func(0.0f) + meta->h_shift), bounds.y, h_offset) };

    for (int i = 1; i < end; ++i) {
        float y =  meta->func((time_seconds - fmodf(time_seconds, meta->seconds_per_plot)) + i*seconds_per_pixel) + meta->h_shift;
        pv = v;
        v = (Vector2){ .x = bounds.x + i, .y = clampf(h_offset - h_ratio*y, bounds.y, h_offset) };
        DrawLineEx(pv, v, 2, color);
    }
    /* DrawLine(end, bounds.y, end, h_offset, GREEN); */
}

int main(int argc, char *argv[]) {
    const char *filepath = argc > 1 ? argv[1] : "params.txt";

    read_params(filepath);
        
    InitWindow(screenWidth, screenHeight, "subsynth");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    AudioStream stream = LoadAudioStream(sample_rate, sample_size, channels);
    SetAudioStreamCallback(stream, AudioInputCallback);
    SetAudioStreamVolume(stream, volume);
    
    int h_fps = GetMonitorRefreshRate(GetCurrentMonitor());
    int l_fps = h_fps / 6;
    int cur_fps = h_fps;
    SetTargetFPS(cur_fps);

    struct Options options = {
        .export_wav = false,
        .export_params = true,
    };

    /* float *buffer = (float*)calloc((int)(screenWidth*MAX_PLOTS), sizeof(float)); */
    /* Signal plot[MAX_PLOTS]; */
    /* for (int i = 0; i < MAX_PLOTS; ++i) { */
    /*     plot[i] = (Signal) { */
    /*         .data = buffer + screenWidth * i, */
    /*         .sample_rate = 44100, */
    /*         .size = screenWidth, */
    /*         .capacity = screenWidth, */
    /*     }; */
    /* } */

    struct Plots plot_meta = {
        .volume_envelope = {
            .name = "Volume Envelope",
            .func = volume_envelope,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
        },
        .modulation_envelope = {
            .name = "Modulation Envelope",
            .func = modulation_envelope,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
        },
        .pitch_lfo =  {
            .name = "Pitch LFO",
            .func = pitch_lfo,
            .seconds_per_plot = length_seconds,
            .h_scale = 0.01f,
            .h_shift = 2.0f,
        },
        .volume_lfo =  {
            .name = "Volume LFO",
            .func = volume_lfo,
            .h_scale = 1.0f,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .oscillator_a =  {
            .name = "Oscillator A",
            .func = oscillator_a,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .oscillator_b =  {
            .name = "Oscillator B",
            .func = oscillator_b,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .filter = {
            .name = "Filter",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .saturation = {
            .name = "Saturation",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .reverb = {
            .name = "Reverb",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .adjust = {
            .name = "Adjust",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .mixed = {
            .name = "Mixed",
            .func = chain,
            .seconds_per_plot = length_seconds,
            .h_scale = 1.0f,
            .h_shift = 2.0f,
            .w_shift = 0,
        }
    };

    enum Page page = MIXED;
    bool playing = false;

    frequency = midi_to_hz(floorf(midi_f0));

    while (!WindowShouldClose()) {
        /* Update */

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            SetAudioStreamVolume(stream, volume);
        }

        if (IsWindowFocused()) {
            if (cur_fps != h_fps) {
                cur_fps = h_fps;
                SetTargetFPS(cur_fps);
            }
        } else {
            if (cur_fps != l_fps) {
                cur_fps = l_fps;
                SetTargetFPS(cur_fps);
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            playing = !playing;

            if (time_seconds == length_seconds) {
                time_seconds = 0.0f;
            }

            if (playing) {
                PlayAudioStream(stream);
            } else {
                PauseAudioStream(stream);
            }
        }

        if (time_seconds == length_seconds) {
            PauseAudioStream(stream);
            playing = false;
        }

        /* Draw */
        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        Rectangle top = {.width = 100, .height = 20, .x = 30, .y = 2};
        Rectangle r = top;

        GuiToggleGroup(r, "Envelope;Oscillator;Filter;Mixed", (int *)&page);

        if (GuiButton((Rectangle){.width=26, .height = 20, .x = 2, .y = 2 }, "O")) {
            page = OPTIONS;
        }

        top.width = 100;
        top.x = (2 + r.width)*4 + r.x + 30;
        
        GuiSlider(top, TextFormat("%.0f", midi_f0), "Frequency", &midi_f0, 0.0f, 127.0f);
        frequency = midi_to_hz(floorf(midi_f0));

        top.x += top.width + 90;
        top.width = 125;
        GuiSliderBar(top, "Time", TextFormat("%.1fs", time_seconds), &time_seconds, 0.0f, length_seconds);

        top.x += top.width + 50;
        top.width = 75;
        GuiSliderBar(top, "Vol", TextFormat("%.1f", volume), &volume, 0.0f, 1.0f);

        if (GuiLabelButton((Rectangle) { .x = screenWidth - 65, .y = top.y + 5, .height = 10 }, playing ? "Playing" : "Not Playing")) {
            playing = !playing;
            if (playing) {
                PlayAudioStream(stream);
            } else {
                PauseAudioStream(stream);
            }
        }

        switch (page) {
        case ENVELOPE: {
            for (int i = 0; i < ENVELOPE_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.env[i]), envelope_tostr(i), &params.env[i], 0.0f, 1.0f);
            }

            r.x = r.width*2;
            r.y = top.y + 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, &plot_meta.volume_envelope, RED);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.modulation_envelope, RED);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, &plot_meta.mixed, RED);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, &plot_meta.pitch_lfo, RED);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.volume_lfo, RED);
        } break;
        case OSCILLATOR: {
            for (int i = 0; i < OSCILLATOR_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.osc[i]), oscillator_tostr(i), &params.osc[i], 0.0f, 1.0f);
            }

            r.x = r.width*2;
            r.y = top.y + 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, &plot_meta.oscillator_a, RED);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.oscillator_b, RED);

            r.x += r.width + 2;
            DrawPlot(r, &plot_meta.mixed, RED);
        } break;
        case FILTER: {
            for (int i = 0; i < FILTER_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.fil[i]), filter_tostr(i), &params.fil[i], 0.0f, 1.0f);
            }

            r.x = r.width*2;
            r.y = top.y + 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, &plot_meta.filter, RED);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.saturation, RED);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, &plot_meta.mixed, RED);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, &plot_meta.reverb, RED);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.adjust, RED);
        } break;
        case MIXED: {
            r.x = 2;
            r.y = top.y + 22;
            r.width = screenWidth - 4;
            r.height = screenHeight - 24;

            DrawPlot(r, &plot_meta.mixed, RED);
        } break;
        case OPTIONS: {
            Rectangle r = {.x = 2, .y = 24, .width = 20, .height = 20};
            GuiDrawText("Options", (Rectangle) {.x = r.x, .y=r.y, .width=200, .height=20}, 0, GRAY);

            r.y += r.height + 2;
            GuiCheckBox(r, "Export wave?", &options.export_wav);

            r.y += r.height + 2;
            GuiCheckBox(r, "Export params?", &options.export_params);
        } break;
        }

        EndDrawing();
    }

    write_params(filepath);

    if (options.export_wav) {
        short *data = malloc((int)sample_rate*length_seconds*sizeof(short));
        float x = 0.0f;
        for (int i = 0; i < (int)(length_seconds*sample_rate); ++i) {
            data[i] = (short)(chain(x)*32000.0f);
            x += 1.0f / sample_rate;
        }

        Wave wave = {
            .sampleRate = (unsigned int)sample_rate,
            .sampleSize = (unsigned int)sample_size,
            .channels = (unsigned int)channels,
            .data = (void *)data,
            .frameCount = length_seconds*sample_rate,
        };

        if (ExportWave(wave, "out.wav")) {
            printf("Exported wav successfully\n");
        }
    }
}
