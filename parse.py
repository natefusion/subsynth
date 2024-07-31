import os
import torch
from pathlib import Path

def parse_synplant_params_file(fp):
    at_genome = False
    dic = {}
    with open(fp) as f:
        for line in f.readlines():
            if at_genome and '}' in line:
                break
            
            if at_genome:
                keyval = line.split(':')
                dic[keyval[0].strip()] = float(keyval[1].strip())
    
            if 'genome' in line:
                at_genome = True
    return dic

def params_to_c_struct_literal(params):
    order = [
        'env_time',
        'env_loop',
        'env_tilt',
        'env_kf',
        'vol_atk',
        'vol_dcy',
        'vol_sus',
        'vol_fade',
        'mod_atk',
        'mod_dcy',
        'mod_sh',
        'mod_vel',
        'lfo_rate',
        'lfo_amt',
        'lfo_bal',
        'lfo_dly',
        'a_form',
        'a_noise',
        'a_mod',
        'a_color',
        'a_freq',
        'fm_mod',
        'fm_amt',
        'mix_mod',
        'osc_mix',
        'b_form',
        'b_noise',
        'b_mod',
        'sub_am',
        'b_freq',
        'b_sh',
        'flt_type',
        'flt_q',
        'flt_mod',
        'flt_sep',
        'flt_freq',
        'flt_kf',
        'saturate',
        'rvb_mix',
        'rvb_atk',
        'rvb_len',
        'rvb_damp',
        'rvb_chor',
        'rvb_size',
        'adj_bass',
        'adj_treb',
        'adj_pan',
        'adj_clip',
    ]

    string = '{'
    for key in order:
        string += '.' + key + ' = ' + str(params[key]) + ','
    string += '}'
    return string

parent = r"C:\Program Files\Sonic Charge\Synplant Patches\All"
def parse_and_save_many_synplant_params(parent):
    dic = {}
    for name in os.listdir(parent):
        path = parent + '/' + name
        dicx[Path(name).stem] = parse_synplant_params_file(path)
    torch.save(dic, 'synplant_params.pt')


def parse_and_save_c_struct_literals(parent):
    for name in os.listdir(parent):
        path = parent + '/' + name
        params = params_to_c_struct_literal(parse_synplant_params_file(path))
        with open('./params/' + Path(name).stem + '.txt', 'x') as f:
            f.write(params)
        
        
     
