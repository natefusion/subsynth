#include <float.h>
#include <stdio.h>
#include <math.h>           // Required for: sinf()
#include <stdlib.h>

#include "randq.h"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_SAMPLES_PER_UPDATE   4096

#define TO_STR(x) #x

#define TAU (2.0f*PI)

float midi_f0 = 60.0f;
float frequency = 8.0f;
float audioFrequency = 60.0f;
float sample_rate = 44100.0f;
float length_seconds = 16.0f;
float time_seconds = 0.0f;
float chain_idx = 0.0f;
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
        };
    };
} params = {{0}};

const union Params params_default = {
    .env_time = 0.5f,
    .env_loop = 0.5f,
    .env_tilt = 0.5f,
    .env_kf = 0.5f,
    .vol_atk = 0.5f,
    .vol_dcy = 0.5f,
    .vol_sus = 0.5f,
    .vol_fade = 0.5f,
    .mod_atk = 0.5f,
    .mod_dcy = 0.5f,
    .mod_sh = 0.5f,
    .mod_vel = 0.5f,
    .lfo_rate = 0.5f,
    .lfo_amt = 0.5f,
    .lfo_bal = 0.5f,
    .lfo_dly = 0.5f,
    .a_form = 0.5f,
    .a_noise = 0.5f,
    .a_mod = 0.5f,
    .a_color = 0.5f,
    .a_freq = 0.5f,
    .fm_mod = 0.5f,
    .fm_amt = 0.5f,
    .mix_mod = 0.5f,
    .osc_mix = 0.7937005260f,
    .b_form = 0.5f,
    .b_noise = 0.5f,
    .b_mod = 0.5f,
    .sub_am = 0.5f,
    .b_freq = 0.6875f,
    .b_sh = 0.5f,
    .flt_type = 2.0f/3.0f,
    .flt_q = 0.5f,
    .flt_mod = 0.5f,
    .flt_sep = 0.5f,
    .flt_freq = 0.75f,
    .flt_kf = 0.5f,
    .saturate = 0.5f,
    .rvb_mix = 0.5f,
    .rvb_atk = 0.5f,
    .rvb_len = 0.5f,
    .rvb_damp = 0.5f,
    .rvb_chor = 0.5f,
    .rvb_size = 0.5f,
    .adj_bass = 0.5f,
    .adj_treb = 0.5f,
    .adj_pan = 0.5f,
    .adj_clip = 0.5f,
};

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
        for (int i = 0; i < ENVELOPE_LEN; ++i)   fprintf(fp, ".%s = %.2f,", envelope_tostr(i), params.env[i]);
        for (int i = 0; i < OSCILLATOR_LEN; ++i) fprintf(fp, ".%s = %.2f,", oscillator_tostr(i), params.osc[i]);
        for (int i = 0; i < FILTER_LEN; ++i)     fprintf(fp, ".%s = %.2f,", filter_tostr(i), params.fil[i]);
        fprintf(fp, "}\n");
        fclose(fp);
    } else {
        fprintf(stderr, "Could not open: %s\n", filepath);
    }
}

void read_params(const char *filepath) {
    printf("%s", "Reading params ... ");
    
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("Can't read file\n");
        return;
    }

    if ('{' == fgetc(fp)) {
        int idx = 0;
        while (true) {
            char c = fgetc(fp);
            if ('}' == c || EOF == c) { printf("%s", "All Done!\n"); break; }
            if ('.' == c) { continue; }
            if ('=' == c) {
                fgetc(fp); // consume space
                
                float n;
                int result = fscanf(fp, "%f", &n);
                if (result == EOF) {
                    printf("Bad number\n");
                    break;
                }
                params.p[idx] = n; 
                idx += 1;
                
                fgetc(fp); // consume comma
            }
        }
    } else {
        printf("%s", "The file is wrong!\n");
    }

    fclose(fp);
}

float get_whitenoise(void) {
    return ((float)randq64_double()*2.0f - 1.0f);
}

float get_brownnoise(void) {
    static float SmoothData = 0.0f;
    const float LPF_Beta = 0.025f;
        
    float RawData = get_whitenoise();
	SmoothData -= LPF_Beta * (SmoothData - RawData); // RC Filter
    return SmoothData;
}

float clampf(float x, float min, float max) {
    if (x < min) return min;
    else if (x > max) return max;
    else return x;
}

float identity(float x) { return x; }
float map(float x, float in_min, float in_max, float out_min, float out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }
float midi_to_hz(float midi) { return powf(2.0f, (midi - 69.0f)/12.0f) * 440.0f; }

float volume_envelope(float x) {
    float x_1 = x / (18.0f * (params.env_time + 0.1f));
    float result = 1.0f;

    if (params.env_loop <= params.env_time) {
        x_1 = x / (18.0f * (params.env_loop + 0.1f));
        result = expf(-floor(x_1) / powf(1.0f + (params.env_time - params.env_loop), 10.0f));
        x_1 -= floor(x_1);
    }
    
    float vol_fade = 1.0f;
    if (params.vol_fade >= 0.75f) vol_fade = (4.0f * params.vol_fade - 3) * -x + 1.0f;

    float env_tilt = powf(params.env_tilt, 4.0f);

    float vol_sus = 0.0f;
    if (params.vol_sus > 0.5f) {
        vol_sus = 2.0f * params.vol_sus - 1.0f;
    }

    if (x_1 >= 0 && x_1 <= env_tilt) {
        result *= powf(x_1/env_tilt, powf(0.5 + params.vol_atk, 15.0f)) * (1.0f - vol_sus) + vol_sus;
    } else {
        result *= 1 - powf((x_1 - env_tilt)/(1 - env_tilt), powf(0.5 + params.vol_dcy, 15.0f)) * (1.0f - vol_sus) + vol_sus;
    }
    
    result *= vol_fade;

    return clampf(result, 0.0f, 1.0f);
}

float _modulation_envelope(float x, bool is_drawing) {
    float x_1 = x / (18.0f * (params.env_time + 0.1f));
    float result = 1.0f;

    if (params.env_loop <= params.env_time) {
        x_1 = x / (18.0f * (params.env_loop + 0.1f));
        result = expf(-floor(x_1) / powf(1.0f + (params.env_time - params.env_loop), 10.0f));
        x_1 -= floor(x_1);
    }
    
    float env_tilt = powf(params.env_tilt, 4.0f);

    if (x_1 >= 0 && x_1 <= env_tilt) {
        result *= powf(x_1/env_tilt, powf(0.5 + (1.0f - params.mod_atk), 15.0f));
    } else {
        result *= 1 - powf((x_1 - env_tilt)/(1 - env_tilt), powf(0.5 + params.mod_dcy, 15.0f));
    }

    return clampf(result, 0.0f, 1.0f) * (is_drawing ? 1.0f : x_1);
}

float modulation_envelope_draw(float x) {
    return _modulation_envelope(x, true);
}

float modulation_envelope(float x) {
    return _modulation_envelope(x, false);
}

float pitch_lfo(float x) {
    x /= 4.0f;

    float amplitude =
        4.0f * powf((params.lfo_amt - 0.5f), 2.0f)
        * tanhf(fabsf(x) / (5.0f * params.lfo_dly + FLT_EPSILON))
        * (params.lfo_bal < 0.5f ? 1.0f : -2.0f * params.lfo_bal + 2.0f);

    x *= powf(1.0f + 4.0f*params.lfo_rate, 2.0f);
    float wave = sinf(TAU * x);

    return amplitude * wave;
}

float volume_lfo(float x) {
    x /= 2.0f;
    float amplitude =
        1.2f * powf(2.0f * (params.lfo_amt - 0.5f), 2.0f)
        * powf(params.lfo_bal < 0.5f ? 2.0f * params.lfo_bal : 1.0f, 2.0f)
        * tanhf(fabsf(x) / (5.0f * params.lfo_dly))
        * (params.lfo_bal > 0.5f ? 1.0f : 2.0f * params.lfo_bal);
    
    x *= powf((1.0f + 4.0f*params.lfo_rate), 2.0f);
    float triangle = 4.0f * fabsf(x - floorf(x + 0.5f)) - 0.5f;

    float shift1 = -2.0f * fabsf(params.lfo_amt - 0.5f) + 1.0f;
    float shift2 = params.lfo_bal < 0.5f ? -2.0f * params.lfo_bal + 1 : 0.0f;

    return clampf(amplitude * triangle + shift1 + shift2, 0.0f, 1.0f);
}

float oscillator(float x, float form) {
    float partials = 12000.0f / (frequency * log10f(frequency));
    if (form >= 0 && form <= 0.57) {
        float sinx = sinf(TAU * x);
        float sawx = tanhf(PI * partials * sinf(TAU * x)) * (1.0f + cosf(TAU * x)) / 2.0f;
        float i = map(form, 0.0f, 0.57f, 0.0f, 1.0f);
        return (1 - i) * sinx + i * sawx;
    } else if (form > 0.57) {
        if (form > 0.81) form = 0.81;
        float shape = 1.0f - map(form, 0.57f, 0.81f, 0.0f, 1.0f);
        float square = tanh(PI * partials * sinf(TAU * x) / 2.0f);
        return (1.0f - shape / 2.0f) * square * (1.0f + shape * cosf(TAU * x));
    }
    return 0.0f;
    /* else { */
    /*     float duty = map(form, 0.81f, 1.0f, 0.0f, 1.0f); */
    /*     float a = tanhf(PI * partials * sinf(TAU * frequency * x)) / 2.0f; */
    /*     float sawa = a * (1.0f + cosf(TAU * frequency * x)); */
    /*     float sawb = a * (1.0f + cosf(TAU * frequency * x + duty/4.0f)); */
    /*     return tanh(TAU * partials * (sawa - sawb)); */
    /* } */
}

float oscillator_a(float x) { return oscillator(x, params.a_form); }
float oscillator_b(float x) { return oscillator(x, params.b_form); }

float chain(float x) {
    float mod_depth = 100.0f;
    float mod_env = modulation_envelope(x);
    float p_lfo = pitch_lfo(x);
    float a_freq = params.a_freq < 0.5 ? params.a_freq + 0.5f : 2.0f * params.a_freq;
    float b_freq = a_freq * (0.03632f * powf(74.4159f, params.b_freq) + 0.2970f);

    float mod_partial = mod_depth * mod_env;
    float mod_env_a = mod_partial * (2.0f * (params.a_mod - 0.5f));
    float mod_env_b = mod_partial * (2.0f * (params.b_mod - 0.5f));
    float pitch_partial = frequency * x;
    float pitch_a = p_lfo + mod_env_a + a_freq * pitch_partial;
    float pitch_b = p_lfo + mod_env_b + b_freq * pitch_partial;
    float sub_pitch = p_lfo + (frequency * b_freq / 2.0f) * x + mod_env_b;
    float sub_am = clampf((params.sub_am - 0.35) / 0.65f, 0.0f, 1.0f);

    float a_noise_amp = params.a_noise < 0.75 ? 0.0f : map(params.a_noise, 0.75f, 1.0f, 0.0f, 1.0f);
    float b_noise_amp = params.b_noise < 0.75 ? 0.0f : map(params.b_noise, 0.75f, 1.0f, 0.0f, 1.0f);
    float a_noise = ((1.0f - params.a_color) * get_brownnoise() + params.a_color * get_whitenoise());
    float b_noise = get_whitenoise();

    float osc_a = oscillator_a(pitch_a);
    float a = clampf((1 - a_noise_amp) * osc_a + a_noise_amp * a_noise, -1.0f, 1.0f);

    float osc_b = (oscillator_b(pitch_b) + sub_am * sinf(TAU * sub_pitch)) / map(sub_am, 0.0f, 1.0f, 1.0f, 2.0f);
    float b = clampf((1 - b_noise_amp) * osc_b + b_noise_amp * b_noise, -1.0f, 1.0f);

    float mix_mod = powf(2.0f * (params.mix_mod - 0.5f), 3.0f);
    float osc_mix = powf(params.osc_mix, 3.0f);
    float mix =
        mix_mod > 0.0f
        ? (1.0f - mix_mod) * osc_mix + mix_mod * powf(mod_env, 3.0f)
        : (1.0f + mix_mod) * osc_mix + mix_mod * powf(mod_env, 3.0f) - mix_mod;
    
    float waveform = (1.0f - mix) * a + mix * b;

    float volume = /* volume_envelope(x) * */ volume_lfo(x);

    return volume * waveform;
}

void AudioInputCallback(void *buffer, unsigned int frames) {
    short *d = (short *)buffer;
    for (unsigned int i = 0; i < frames; ++i) {
        d[i] = (short)(chain(time_seconds) * 32000.0f);
        time_seconds += 1.0f/sample_rate;
        if (time_seconds > length_seconds) time_seconds -= length_seconds;
    }
}

struct Options {
    bool export_wav;
    bool export_params;
};

struct Plot_Metadata {
    char *name;
    float (*func)(float);
    float seconds_per_plot;
    float h_shift;
};

#define NUM_PLOTS 11

union Plots {
    struct Plot_Metadata plots[11];
    struct {
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
};

void DrawPlot(Rectangle bounds, struct Plot_Metadata *meta) {
    bounds.height -= 13.0f;
    GuiDrawRectangle(bounds, 1, BLACK, RAYWHITE);
    DrawText(meta->name, bounds.x + 2, bounds.y + 2, 10, GRAY);
    Rectangle zoom_rect = (Rectangle){ .x = bounds.x + 12, .y = bounds.y + bounds.height + 2, .width = 100, .height = 10 };
    GuiSliderBar(zoom_rect, "-", TextFormat("(w) %.4f", meta->seconds_per_plot), &meta->seconds_per_plot, 0.001f, length_seconds);

    float h_offset = bounds.y + bounds.height;
    float h_ratio = bounds.height / 4.0f;
    float seconds_per_pixel = meta->seconds_per_plot / bounds.width;
    float end = fmodf(time_seconds, meta->seconds_per_plot) / seconds_per_pixel;
    float start = time_seconds - fmodf(time_seconds, meta->seconds_per_plot);
    float new_time = -1.0f;


    
    Vector2 mp = GetMousePosition();
    int mx = (int)mp.x;
    int my = (int)mp.y;
    if (CheckCollisionPointRec(mp, bounds)) {
        int xpos = mx + 1;
        int i = mx - (int)bounds.x;
        int fontsize = 10;
        float time_at_cursor = start + i*seconds_per_pixel;
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            fontsize = 20;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            new_time = time_at_cursor;
        }

        float value_at_point = meta->func(time_at_cursor);
        const char *t = TextFormat("(%.3fs, %.3f) (%d, %d)", time_at_cursor, value_at_point, i, (int)h_offset - my - 1);
        int len = MeasureText(t, fontsize);

        int ypos = my - fontsize;
        if (ypos < bounds.y + 1) ypos = bounds.y + 1;
        DrawLine(xpos, my, xpos, clampf(h_offset - h_ratio*(value_at_point + meta->h_shift), bounds.y, h_offset), GREEN);
        
        xpos = xpos + len > bounds.x + bounds.width - 1 ? bounds.x + bounds.width - 1 - len : xpos;
        DrawText(t, xpos, ypos, fontsize, GRAY);
    }

    Vector2 pv = {0};
    Vector2 v = {.x = bounds.x, .y = clampf(h_offset - h_ratio*(meta->func(start) + meta->h_shift), bounds.y, h_offset) };

    for (int i = 1; i < bounds.width; ++i) {
        float y =  meta->func(start + i*seconds_per_pixel) + meta->h_shift;
        pv = v;
        v = (Vector2){ .x = bounds.x + i, .y = clampf(h_offset - h_ratio*y, bounds.y, h_offset) };

        DrawLineEx(pv, v, 2, i < end ? RED : (Color) { .r = RED.r, .g = RED.g, .b = RED.g, .a = 32 });
    }

    if (new_time != -1.0f) {
        time_seconds = new_time;
    }
}

void DrawPlotSimple(Rectangle bounds, struct Plot_Metadata *meta) {
    GuiDrawRectangle(bounds, 1, BLACK, RAYWHITE);
    DrawText(meta->name, bounds.x + 2, bounds.y + 2, 10, GRAY);
    
    float h_offset = bounds.y + bounds.height;
    float h_ratio = bounds.height / 4.0f;

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

        float value_at_point = meta->func(i/bounds.width*2.0f);
        const char *t = TextFormat("(%.3f) (%d, %d)", value_at_point, i, (int)h_offset - my - 1);
        int len = MeasureText(t, fontsize);

        int ypos = my - fontsize;
        if (ypos < bounds.y + 1) ypos = bounds.y + 1;
        DrawLine(xpos, my, xpos, clampf(h_offset - h_ratio*(value_at_point + meta->h_shift), bounds.y, h_offset), GREEN);
        
        xpos = xpos + len > bounds.x + bounds.width - 1 ? bounds.x + bounds.width - 1 - len : xpos;
        DrawText(t, xpos, ypos, fontsize, GRAY);
    }
    
    Vector2 pv = {0};
    Vector2 v = {.x = bounds.x, .y = clampf(h_offset - h_ratio*(meta->func(0) + meta->h_shift), bounds.y, h_offset) };
    for (int i = 0; i < bounds.width; ++i) {
        float y = meta->func((float)i/bounds.width*2.0f) + meta->h_shift;
        pv = v;
        v = (Vector2){ .x = bounds.x + i, .y = clampf(h_offset - h_ratio*y, bounds.y, h_offset) };
        DrawLineEx(pv, v, 2, RED);
    }
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

    union Plots plot_meta = {
        .volume_envelope = {
            .name = "Volume Envelope",
            .func = volume_envelope,
            .seconds_per_plot = length_seconds,
            .h_shift = 1.5f,
        },
        .modulation_envelope = {
            .name = "Modulation Envelope",
            .func = modulation_envelope_draw,
            .seconds_per_plot = length_seconds,
            .h_shift = 1.5f,
        },
        .pitch_lfo =  {
            .name = "Pitch LFO",
            .func = pitch_lfo,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
        },
        .volume_lfo =  {
            .name = "Volume LFO",
            .func = volume_lfo,
            .seconds_per_plot = length_seconds,
            .h_shift = 1.0f,
        },
        .oscillator_a =  {
            .name = "Oscillator A",
            .func = oscillator_a,
            .seconds_per_plot = 1 / frequency,
            .h_shift = 2.0f,
        },
        .oscillator_b =  {
            .name = "Oscillator B",
            .func = oscillator_b,
            .seconds_per_plot = 1 / frequency,
            .h_shift = 2.0f,
        },
        .filter = {
            .name = "Filter",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
        },
        .saturation = {
            .name = "Saturation",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
        },
        .reverb = {
            .name = "Reverb",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
        },
        .adjust = {
            .name = "Adjust",
            .func = identity,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
        },
        .mixed = {
            .name = "Mixed",
            .func = chain,
            .seconds_per_plot = length_seconds,
            .h_shift = 2.0f,
        }
    };

    enum Page page = MIXED;
    bool playing = false;

    frequency = midi_to_hz(floorf(midi_f0));

    while (!WindowShouldClose()) {
        /* Begin Update */
        
        if (IsFileDropped()) {
            FilePathList dropped_files = LoadDroppedFiles();
            if (dropped_files.count > 1) {
                printf("You dropped more than one file, I will only load the first one ...\n");
            }
            
            read_params(dropped_files.paths[0]);
            UnloadDroppedFiles(dropped_files);
        }
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            SetAudioStreamVolume(stream, volume);
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            if (midi_f0 < 127)
                midi_f0 += 1;
        } else if (IsKeyPressed(KEY_LEFT)) {
            if (midi_f0 > 0)
                midi_f0 -= 1;
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

        /* End Update */

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

            if (GuiButton((Rectangle) { .x = r.x, .y = r.y+22, .width=r.width, .height=r.height}, "Default Params")) {
                params = params_default;
            } 

            r.x = r.width*2;
            r.y = top.y + 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, &plot_meta.volume_envelope);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.modulation_envelope);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, &plot_meta.mixed);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, &plot_meta.pitch_lfo);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.volume_lfo);
        } break;
        case OSCILLATOR: {
            for (int i = 0; i < OSCILLATOR_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.osc[i]), oscillator_tostr(i), &params.osc[i], 0.0f, 1.0f);
            }

            if (GuiButton((Rectangle) { .x = r.x, .y = r.y+22, .width=r.width, .height=r.height}, "Default Params")) {
                params = params_default;
            } 

            r.x = r.width*2;
            r.y = top.y + 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlotSimple(r, &plot_meta.oscillator_a);

            r.y += r.height + 2;
            DrawPlotSimple(r, &plot_meta.oscillator_b);

            r.x += r.width + 2;
            DrawPlot(r, &plot_meta.mixed);
        } break;
        case FILTER: {
            for (int i = 0; i < FILTER_LEN; ++i) {
                r.y += 22;
                GuiSlider(r, TextFormat("%.2f", params.fil[i]), filter_tostr(i), &params.fil[i], 0.0f, 1.0f);
            }

            if (GuiButton((Rectangle) { .x = r.x, .y = r.y+22, .width=r.width, .height=r.height}, "Default Params")) {
                params = params_default;
            } 

            r.x = r.width*2;
            r.y = top.y + 22;
            r.width = 300;
            r.height = 300;
            
            DrawPlot(r, &plot_meta.filter);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.saturation);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, &plot_meta.mixed);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, &plot_meta.reverb);

            r.y += r.height + 2;
            DrawPlot(r, &plot_meta.adjust);
        } break;
        case MIXED: {
            r.x = 2;
            r.y = top.y + 22;
            r.width = screenWidth - 4;
            r.height = screenHeight - 24;

            DrawPlot(r, &plot_meta.mixed);
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

    if (options.export_params) {
        write_params(filepath);
    }

    if (options.export_wav) {
        const int len = (int)(sample_rate*length_seconds);
        short *data = malloc(len*sizeof(short));
        float x = 0.0f;
        for (int i = 0; i < len; ++i) {
            data[i] = (short)(chain(x)*32000.0f);
            x += 1.0f / sample_rate;
        }

        Wave wave = {
            .sampleRate = (unsigned int)sample_rate,
            .sampleSize = (unsigned int)sample_size,
            .channels = (unsigned int)channels,
            .data = (void *)data,
            .frameCount = len,
        };

        if (ExportWave(wave, "out.wav")) {
            printf("Exported wav successfully\n");
        }
    }
}
