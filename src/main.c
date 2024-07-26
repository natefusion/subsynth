#include <stdlib.h>         // Required for: malloc(), free()
#include <stdio.h>
#include <math.h>           // Required for: sinf()

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#define MAX_SAMPLES_PER_UPDATE   4096

#define TO_STR(x) #x

float midi_f0 = 60.0f;
float frequency = 8.0f;
float amplitude = 32000.0f;
float sample_rate = 44100.0f;
float length_seconds = 4.0f;
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

float identity(float x) { return x; }
float map(float x, float in_min, float in_max, float out_min, float out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

float midi_to_hz(float midi) {
    return powf(2.0f, (midi - 69.0f)/12.0f) * 440.0f;
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

float oscillator_a(float x) {
    return oscillator(x, params.a_form);
}

float oscillator_b(float x) {
    return oscillator(x, params.b_form);
}

float chain(float x) {
    return (1.0f - params.osc_mix) * oscillator(x, params.a_form) + params.osc_mix * oscillator(x, params.b_form);
}

void AudioInputCallback(void *buffer, unsigned int frames) {
    short *d = (short *)buffer;

    for (unsigned int i = 0; i < frames; i++) {
        d[i] = (short)(chain(sineIdx) * amplitude);
        sineIdx += 1/sample_rate;
        if (sineIdx > 1.0f) sineIdx = 0.0f;
    }
}

void DrawPlot(Rectangle bounds, const char *title, float *data, Color color) {
    GuiDrawRectangle(bounds, 1, BLACK, RAYWHITE);
    DrawText(title, bounds.x + 2, bounds.y + 2, 10, GRAY);

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
    float *data;
    float h_scale;
    float w_scale;
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

void pm_apply(struct Plot_Metadata *pm, int i) {
    pm->data[i] = (pm->func((float)i * pm->w_scale) + pm->h_shift) * pm->h_scale;;
}

int main(int argc, char *argv[]) {
    const char *filepath = argc > 1 ? argv[1] : "params.txt";

    read_params(filepath);
        
    InitWindow(screenWidth, screenHeight, "subsynth");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    AudioStream stream = LoadAudioStream(44100, 16, 1);
    SetAudioStreamCallback(stream, AudioInputCallback);
    
    int h_fps = GetMonitorRefreshRate(GetCurrentMonitor());
    int l_fps = h_fps / 6;
    int cur_fps = h_fps;
    SetTargetFPS(cur_fps);

    float *buffer = (float*)calloc((int)(screenWidth*MAX_PLOTS), sizeof(float));
    float *plot[MAX_PLOTS];
    for (int i = 0; i < MAX_PLOTS; ++i) {
        plot[i] = buffer + screenWidth * i;
    }

    struct Plots plot_meta = {
        .volume_envelope = {
            .name = "Volume Envelope",
            .func = volume_envelope,
            .data = plot[PLOT_ONE],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .modulation_envelope = {
            .name = "Modulation Envelope",
            .func = modulation_envelope,
            .data = plot[PLOT_TWO],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .pitch_lfo =  {
            .name = "Pitch LFO",
            .func = modulation_envelope,
            .data = plot[PLOT_THREE],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .volume_lfo =  {
            .name = "Volume LFO",
            .func = modulation_envelope,
            .h_scale = screenHeight / 4.0f,
            .data = plot[PLOT_FOUR],
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .oscillator_a =  {
            .name = "Oscillator A",
            .func = oscillator_a,
            .data = plot[PLOT_ONE],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .oscillator_b =  {
            .name = "Oscillator B",
            .func = oscillator_b,
            .data = plot[PLOT_TWO],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .filter = {
            .name = "Filter",
            .func = identity,
            .data = plot[PLOT_ONE],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .saturation = {
            .name = "Saturation",
            .func = identity,
            .data = plot[PLOT_TWO],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .reverb = {
            .name = "Reverb",
            .func = identity,
            .data = plot[PLOT_THREE],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .adjust = {
            .name = "Adjust",
            .func = identity,
            .data = plot[PLOT_FOUR],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        },
        .mixed = {
            .name = "Mixed",
            .func = chain,
            .data = plot[PLOT_FIVE],
            .h_scale = screenHeight / 4.0f,
            .w_scale = 4.0f / screenWidth,
            .h_shift = 2.0f,
            .w_shift = 0,
        }
    };

    enum Page page = ENVELOPE;
    enum Page prevPage = ENVELOPE;
    bool redraw = true;
    bool playing = false;
    

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

        if (IsKeyPressed(KEY_SPACE)) {
            playing = !playing;
        }

        if (playing) {
            PlayAudioStream(stream);
        } else {
            PauseAudioStream(stream);
        }

        switch (page) {
        case ENVELOPE: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    pm_apply(&plot_meta.volume_envelope, i);
                    pm_apply(&plot_meta.modulation_envelope, i);
                    pm_apply(&plot_meta.pitch_lfo, i);
                    pm_apply(&plot_meta.volume_lfo, i);
                    pm_apply(&plot_meta.mixed, i);
                }
                redraw = false;
            }
        } break;
        case OSCILLATOR: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    pm_apply(&plot_meta.oscillator_a, i);
                    pm_apply(&plot_meta.oscillator_b, i);
                    pm_apply(&plot_meta.mixed, i);
                }
                redraw = false;
            }
        } break;
        case FILTER: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    pm_apply(&plot_meta.filter, i);
                    pm_apply(&plot_meta.reverb, i);
                    pm_apply(&plot_meta.saturation, i);
                    pm_apply(&plot_meta.adjust, i);
                    pm_apply(&plot_meta.mixed, i);
                }
                redraw = false;
            }
        } break;
        case MIXED: {
            if (redraw) {
                for (int i = 0; i < screenWidth; ++i) {
                    pm_apply(&plot_meta.mixed, i);
                }
                redraw = false;
            }
        } break;
        }

        /* Draw */
        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        Rectangle top = {.width = 100, .height = 20, .x = 30, .y = 2};
        Rectangle r = top;

        prevPage = page;
        GuiToggleGroup(r, "Envelope;Oscillator;Filter;Mixed", (int *)&page);
        
        top.width = 200;
        top.x = (2 + r.width)*4 + r.x + 30;
        
        GuiSlider(top, TextFormat("%.0f", midi_f0), "Frequency", &midi_f0, 0.0f, 127.0f);
        frequency = midi_to_hz(floorf(midi_f0));

        DrawText(playing ? "Playing" : "Not Playing", screenWidth - 65, top.y + 5, 10, GRAY);

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
            
            DrawPlot(r, plot_meta.volume_envelope.name, plot_meta.volume_envelope.data, RED);

            r.y += r.height + 2;
            DrawPlot(r, plot_meta.modulation_envelope.name, plot_meta.modulation_envelope.data, RED);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, plot_meta.mixed.name, plot_meta.mixed.data, RED);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, plot_meta.pitch_lfo.name, plot_meta.pitch_lfo.data, RED);

            r.y += r.height + 2;
            DrawPlot(r, plot_meta.volume_lfo.name, plot_meta.volume_lfo.data, RED);
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
            
            DrawPlot(r, plot_meta.oscillator_a.name, plot_meta.oscillator_a.data, RED);

            r.y += r.height + 2;
            DrawPlot(r, plot_meta.oscillator_b.name, plot_meta.oscillator_b.data, RED);

            r.x += r.width + 2;
            DrawPlot(r, plot_meta.mixed.name, plot_meta.mixed.data, RED);
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
            
            DrawPlot(r, plot_meta.filter.name, plot_meta.filter.data, RED);

            r.y += r.height + 2;
            DrawPlot(r, "Saturation", plot[PLOT_TWO], RED);

            DrawPlot((Rectangle) { r.x + r.width + 2, r.y, r.width, r.height}, plot_meta.mixed.name, plot_meta.mixed.data, RED);

            r.y += r.height + 2;
            r.height /= 2;
            DrawPlot(r, plot_meta.reverb.name, plot_meta.reverb.data, RED);

            r.y += r.height + 2;
            DrawPlot(r, plot_meta.adjust.name, plot_meta.adjust.data, RED);
        } break;
        case MIXED: {
            r.x = 2;
            r.y = top.y + 22;
            r.width = screenWidth - 4;
            r.height = screenHeight - 24;

            DrawPlot(r, plot_meta.mixed.name, plot_meta.mixed.data, RED);
        } break;
        }

        EndDrawing();
    }

    write_params(filepath);
}
