/*=============================================================================
| Copyright 2013 Matthew D. Steele <mdsteele@alum.mit.edu>                    |
|                                                                             |
| This file is part of Milestone.                                             |
|                                                                             |
| Milestone is free software: you can redistribute it and/or modify it under  |
| the terms of the GNU General Public License as published by the Free        |
| Software Foundation, either version 3 of the License, or (at your option)   |
| any later version.                                                          |
|                                                                             |
| Milestone is distributed in the hope that it will be useful, but WITHOUT    |
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       |
| FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   |
| more details.                                                               |
|                                                                             |
| You should have received a copy of the GNU General Public License along     |
| with Milestone.  If not, see <http://www.gnu.org/licenses/>.                |
=============================================================================*/

#include "milestone/gui/audio.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "milestone/util/audio.h"
#include "milestone/util/misc.h"
#include "milestone/util/random.h"
#include "milestone/util/vector.h"

/*===========================================================================*/
// Constants:

// We use 16-bit stereo at 22050 samples/sec and a buffer size of 1024 samples.
#define AUDIO_FORMAT AUDIO_S16SYS
#define AUDIO_CHANNELS 2
#define AUDIO_RATE 22050
#define AUDIO_BUFFERSIZE 1024

// How many sound effects we can play simultaneously:
#define NUM_MIXER_CHANNELS 16

/*===========================================================================*/
// Music:

static struct {
  const char *filename;
  Mix_Music *music; // may be NULL if music failed to load
} music_entries[] = {
  [AZ_MUS_TITLE] = { .filename = "title.ogg" },
  [AZ_MUS_PLAY] = { .filename = "play.ogg" }
};

static void load_all_music(void) {
  // TODO: enable music
/*   const char *resource_dir = az_get_resource_directory(); */
/*   char path_buffer[strlen(resource_dir) + 48u]; */
/*   AZ_ARRAY_LOOP(entry, music_entries) { */
/*     assert(entry->filename != NULL); */
/*     assert(strlen(entry->filename) > 0); */
/*     assert(strlen(entry->filename) <= 40); */
/*     sprintf(path_buffer, "%s/music/%s", resource_dir, entry->filename); */
/*     entry->music = Mix_LoadMUS(path_buffer); */
/*     if (entry->music == NULL) { */
/*       fprintf(stderr, "Warning: failed to load %s\n", entry->filename); */
/*     } */
/*   } */
}

static void free_all_music(void) {
  AZ_ARRAY_LOOP(entry, music_entries) {
    if (entry->music != NULL) {
      Mix_FreeMusic(entry->music);
      entry->music = NULL;
    }
  }
}

static void tick_music(const az_soundboard_t *soundboard) {
  static enum { STOPPED, FADING_OUT, PLAYING } music_status = STOPPED;
  static az_music_key_t current_music = AZ_MUS_TITLE;
  static bool music_pending = false;
  static az_music_key_t next_music = AZ_MUS_TITLE;

  // If nothing is playing right now (e.g. because we finished fading out, or
  // because the last music we started failed to load at program startup), set
  // us to STOPPED status.
  if (!Mix_PlayingMusic()) music_status = STOPPED;

  // If desired, change which music will play next.
  int fade_out_millis = -1;
  switch (soundboard->music_action) {
    case AZ_MUSA_NOTHING: break;
    case AZ_MUSA_CHANGE:
      next_music = soundboard->next_music;
      music_pending = true;
      if (current_music != next_music || music_status != PLAYING) {
        fade_out_millis = 500;
      }
      break;
    case AZ_MUSA_STOP:
      assert(soundboard->music_fade_out_millis >= 0);
      music_pending = false;
      fade_out_millis = soundboard->music_fade_out_millis;
      break;
  }
  if (fade_out_millis >= 0 && music_status == PLAYING) {
    if (fade_out_millis > 0) {
      Mix_FadeOutMusic(fade_out_millis);
      music_status = FADING_OUT;
    } else {
      Mix_HaltMusic();
      music_status = STOPPED;
    }
  }

  // If we're ready to start the next music, do so.
  if (music_pending && music_status == STOPPED) {
    current_music = next_music;
    music_pending = false;
    Mix_Music *music_data = music_entries[current_music].music;
    if (music_data != NULL && Mix_PlayMusic(music_data, -1) == 0) {
      music_status = PLAYING;
    }
  }
}

/*===========================================================================*/
// Sound effects:

// This section of the file contains a modified version of the sound and WAV
// file generation code from sfxr (http://www.drpetter.se/project_sfxr.html),
// written by Tomas Pettersson ("DrPetter").  sfxr is made available under the
// MIT/Expat license, reproduced here:

  /*******************************************************************\
  * sfxr                                                              *
  * Copyright (c) 2007 Tomas Pettersson                               *
  *                                                                   *
  * Permission is hereby granted, free of charge, to any person       *
  * obtaining a copy of this software and associated documentation    *
  * files (the "Software"), to deal in the Software without           *
  * restriction, including without limitation the rights to use,      *
  * copy, modify, merge, publish, distribute, sublicense, and/or sell *
  * copies of the Software, and to permit persons to whom the         *
  * Software is furnished to do so, subject to the following          *
  * conditions:                                                       *
  *                                                                   *
  * The above copyright notice and this permission notice shall be    *
  * included in all copies or substantial portions of the Software.   *
  *                                                                   *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,   *
  * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   *
  * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          *
  * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       *
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      *
  * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     *
  * OTHER DEALINGS IN THE SOFTWARE.                                   *
  \*******************************************************************/

// Many thanks to DrPetter for developing sfxr, and for releasing it as Free
// Software.

// Rather than store our sound effects as resource files, we generate them from
// scratch at program startup.  For each sound effect, the table below stores
// the sfxr parameters used to generate the sound; then in init_all_sounds(),
// for each entry in the table we call synth_sound_wav() to run the sfxr
// algorithm and generate an in-memory WAV file, which we then pass to the SDL
// mixer to read and convert into a Mix_Chunk.

typedef struct {
  // Input parameters to the sfxr algorithm.  These correspond directly to the
  // sliders in sfxr, and each have ranges of [0,1] or [-1,1].  Two minor
  // adjustments have been made: first, lpf_cutoff here is 1 minus its usual
  // value in sfxr (this is so that we can leave it zero most of the time,
  // rather than constantly having to explicitly specify it as 1.0), and
  // second, we use a volume range of -1 to 1 instead of 0 to 1 (again so that
  // we can usually leave it at zero, rather than explicitly specifying 0.5).
  enum { SQUARE, SAWTOOTH, TRIANGLE, SINE, WOBBLE, NOISE } wave_kind;
  float env_attack, env_sustain, env_punch, env_decay;
  float start_freq, freq_limit, freq_slide, freq_delta_slide;
  float vibrato_depth, vibrato_speed;
  float arp_mod, arp_speed;
  float square_duty, duty_sweep;
  float repeat_speed;
  float phaser_offset, phaser_sweep;
  float lpf_cutoff, lpf_ramp, lpf_resonance; // cutoff is 1 - value from sfxr
  float hpf_cutoff, hpf_ramp;
  float volume_adjust; // -1.0 to 1.0
  // SDL mixer chunk object, produced by running the sfxr algorithm:
  Mix_Chunk *chunk;
} az_sound_entry_t;

static az_sound_entry_t sound_entries[] = {
  [AZ_SND_BONUS_ROUND] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.31792, .env_decay = 0.37936,
    .start_freq = 0.36764, .freq_slide = 0.29932,
    .square_duty = 0.44148, .repeat_speed = 0.48436
  },
  [AZ_SND_BREAK_TARGET] = {
    .wave_kind = NOISE,
    .env_sustain = 0.2272, .env_punch = 0.33782, .env_decay = 0.45975,
    .start_freq = 0.1628042, .freq_slide = -0.2587,
    .vibrato_depth = 0.4403, .vibrato_speed = 0.11262
  },
  [AZ_SND_FAKE_TARGET] = {
    .wave_kind = SQUARE,
    .env_attack = 0.07240744, .env_sustain = 0.03976036,
    .env_punch = 0.1997017, .env_decay = 0.4092,
    .start_freq = -0.2695826, .freq_slide = 0.1016292,
    .freq_delta_slide = 0.1620569,
    .vibrato_depth = -0.2683361, .vibrato_speed = 0.8036,
    .arp_mod = -0.6722, .arp_speed = 0.8645999,
    .square_duty = -0.9372, .duty_sweep = 0.113943,
    .repeat_speed = 0.1008,
    .phaser_offset = -0.6810075, .phaser_sweep = -0.0009761918,
    .lpf_cutoff = 0.279472, .lpf_ramp = -0.1031617, .lpf_resonance = -0.8484,
    .hpf_cutoff = 0.217439, .hpf_ramp = -0.0007733077
  },
  [AZ_SND_FIRE_BOMB] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.26908, .env_decay = 0.487,
    .start_freq = 0.30491, .freq_slide = 0.44384,
    .square_duty = 0.12162, .repeat_speed = 0.63352
  },
  [AZ_SND_FIRE_BULLET] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.169014, .env_punch = 0.05175, .env_decay = 0.23136,
    .start_freq = 0.3802817, .freq_limit = 0.2, .freq_slide = -0.2253521,
    .square_duty = 0.4215, .duty_sweep = 0.12774
  },
  [AZ_SND_FIRE_STOPPER] = {
    .wave_kind = SINE,
    .env_sustain = 0.15466, .env_decay = 0.34644,
    .start_freq = 0.74095, .freq_limit = 0.30665, .freq_slide = -0.29576,
    .vibrato_depth = 0.478873252869, .vibrato_speed = 0.83098590374,
    .square_duty = 0.7107, .duty_sweep = -0.05537,
    .phaser_offset = 0.12402, .phaser_sweep = -0.00736, .hpf_cutoff = 0.04752
  },
  [AZ_SND_FIRE_TANK_SHELL] = {
    .wave_kind = NOISE,
    .env_sustain = 0.161972, .env_punch = 0.37388, .env_decay = 0.29095,
    .start_freq = 0.1732557, .freq_slide = 0.2668,
    .arp_mod = -0.30704, .arp_speed = 0.71166,
    .repeat_speed = 0.75975, .phaser_offset = -0.13071,
    .phaser_sweep = -0.15447
  },
  [AZ_SND_GAIN_BOMB] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.00246, .env_punch = 0.44127, .env_decay = 0.48796,
    .start_freq = 0.53315, .arp_mod = 0.55284, .arp_speed = 0.61086
  },
  [AZ_SND_GAIN_LIFE] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.688236117363, .env_punch = 0.612775921822,
    .env_decay = 0.443661957979,
    .start_freq = 1.0, .freq_slide = 0.267605662346,
    .arp_mod = -0.760563373566, .arp_speed = 0.894366204739,
    .square_duty = -0.71439999342,
    .repeat_speed = 0.352112680674,
    .phaser_offset = -0.126760542393,
  },
  [AZ_SND_HIT_TANK_SHELL] = {
    .wave_kind = NOISE,
    .env_sustain = 0.23179, .env_punch = 0.36284, .env_decay = 0.232394367456,
    .start_freq = 0.1927034, .freq_slide = -0.3496,
    .vibrato_depth = 0.52885, .vibrato_speed = 0.32592,
    .phaser_offset = -0.12909, .phaser_sweep = -0.04236
  },
  [AZ_SND_KILL_BASILISK] = {
    .wave_kind = NOISE,
    .env_sustain = 0.1897, .env_punch = 0.48116, .env_decay = 0.33415,
    .start_freq = 0.4400996, .freq_slide = -0.35086, .repeat_speed = 0.50885
  },
  [AZ_SND_KILL_GUARD] = {
    .wave_kind = NOISE,
    .env_sustain = 0.32899, .env_punch = 0.5654, .env_decay = 0.3979,
    .start_freq = 0.1300612, .freq_slide = 0.17176, .repeat_speed = 0.6765
  },
  [AZ_SND_KILL_TANK] = {
    .wave_kind = NOISE,
    .env_sustain = 0.29548, .env_punch = 0.24128, .env_decay = 0.37485,
    .start_freq = 0.2346821,
    .vibrato_depth = 0.19887, .vibrato_speed = 0.42264,
    .arp_mod = -0.35056, .arp_speed = 0.74433
  },
  [AZ_SND_LOSE_LIFE] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.41549295187, .env_decay = 0.647887349129,
    .start_freq = 0.281690150499, .freq_limit = 0.2,
    .freq_slide = -0.0704225301743, .freq_delta_slide = 0.0140844583511,
    .square_duty = 0.725352108479, .duty_sweep = -0.140845060349
  },
  [AZ_SND_NEXT_WAVE] = {
    .wave_kind = SQUARE,
    .env_sustain = 0.38416, .env_decay = 0.30604,
    .start_freq = 0.49277, .freq_slide = 0.39104,
    .square_duty = 0.06852, .repeat_speed = 0.47532
  }
};

static struct {
  // Fields for maintaining sfxr synth state:
  int phase;
  double fperiod, fmaxperiod, fslide, fdslide;
  int period;
  float square_duty, square_slide;
  int env_stage;
  int env_time;
  int env_length[3];
  float env_vol;
  float fphase, fdphase;
  int iphase;
  float phaser_buffer[1024];
  int ipp;
  float noise_buffer[32];
  float fltp, fltdp, fltw, fltw_d;
  float fltdmp, fltphp, flthp, flthp_d;
  float vib_phase, vib_speed, vib_amp;
  int rep_time, rep_limit;
  int arp_time, arp_limit;
  double arp_mod;
  // Fields for constructing WAV files:
  char wav_buffer[128 * 1024]; // buffer for storing in-memory WAV file
  char *wav_chunk_size;  // points to where in wav_buffer we write chunk size
  char *wav_data_size;   // points to where in wav_buffer we write data size
  char *wav_data_start;  // points to where in wav_buffer we write actual data
  size_t wav_buffer_len; // stores total size of WAV file
} synth;

// Copy a NUL-terminated string to the buffer and advance the pointer.
static void write_string(const char *str, char **ptr) {
  const size_t length = strlen(str);
  memcpy(*ptr, str, length);
  *ptr += length;
}

// Write a uint16 to the buffer in little-endian order and advance the pointer.
static void write_uint16le(uint16_t value, char **ptr) {
  (*ptr)[0] = value & 0xff;
  (*ptr)[1] = (value >> 8) & 0xff;
  *ptr += 2;
}

// Write a uint32 to the buffer in little-endian order and advance the pointer.
static void write_uint32le(uint32_t value, char **ptr) {
  (*ptr)[0] = value & 0xff;
  (*ptr)[1] = (value >> 8) & 0xff;
  (*ptr)[2] = (value >> 16) & 0xff;
  (*ptr)[3] = (value >> 24) & 0xff;
  *ptr += 4;
}

// Fill synth.wav_buffer with the common WAV header data, and initialize the
// synth.wav_* pointers so that we can fill those fields in for each WAV that
// we generate.
static void init_wav_buffer(void) {
  const int sample_rate = 22050; // 22.05 kHz
  const int sample_bytes = 2; // 16-bit samples
  const int num_channels = 1; // mono
  char *ptr = synth.wav_buffer;
  // File header:
  write_string("RIFF", &ptr); // chunk ID
  synth.wav_chunk_size = ptr;
  ptr += 4; // leave space for file size
  write_string("WAVE", &ptr); // chunk format
  // Subchunk 1 (fmt):
  write_string("fmt ", &ptr); // subchunk 1 ID
  write_uint32le(16, &ptr); // subchunk 1 size
  write_uint16le(1, &ptr); // audio format (1 = PCM, no compression)
  write_uint16le(num_channels, &ptr); // num channels (1 = mono)
  write_uint32le(sample_rate, &ptr); // sample rate
  write_uint32le(sample_rate * sample_bytes * num_channels, &ptr); // bytes/sec
  write_uint16le(sample_bytes * num_channels, &ptr); // block align
  write_uint16le(sample_bytes * 8, &ptr); // bits per sample
  // Subchunk 2 (data):
  write_string("data", &ptr); // subchunk 1 ID
  synth.wav_data_size = ptr;
  ptr += 4; // leave space for data size
  synth.wav_data_start = ptr;
}

// Reset the sfxr synth.  This code is taken directly from sfxr, with only
// minor changes.
static void reset_synth(const az_sound_entry_t *entry, bool restart) {
  if (!restart) synth.phase = 0;
  synth.fperiod = 100.0 / (entry->start_freq * entry->start_freq + 0.001);
  synth.period = (int)synth.fperiod;
  synth.fmaxperiod = 100.0 / (entry->freq_limit * entry->freq_limit + 0.001);
  synth.fslide = 1.0 - pow(entry->freq_slide, 3.0) * 0.01;
  synth.fdslide = -pow(entry->freq_delta_slide, 3.0) * 0.000001;
  synth.square_duty = 0.5f - entry->square_duty * 0.5f;
  synth.square_slide = -entry->duty_sweep * 0.00005f;
  if (entry->arp_mod >= 0.0f) {
    synth.arp_mod = 1.0 - pow(entry->arp_mod, 2.0) * 0.9;
  } else {
    synth.arp_mod = 1.0 + pow(entry->arp_mod, 2.0) * 10.0;
  }
  synth.arp_time = 0;
  synth.arp_limit = (int)(pow(1.0 - entry->arp_speed, 2.0) * 20000 + 32);
  if (entry->arp_speed == 1.0f) synth.arp_limit = 0;
  if (!restart) {
    // Reset filter:
    synth.fltp = 0.0f;
    synth.fltdp = 0.0f;
    synth.fltw = pow(1.0 - entry->lpf_cutoff, 3.0) * 0.1f;
    synth.fltw_d = 1.0f + entry->lpf_ramp * 0.0001f;
    synth.fltdmp = 5.0f / (1.0f + pow(entry->lpf_resonance, 2.0) * 20.0f) *
      (0.01f + synth.fltw);
    if (synth.fltdmp > 0.8f) synth.fltdmp = 0.8f;
    synth.fltphp = 0.0f;
    synth.flthp = pow(entry->hpf_cutoff, 2.0) * 0.1f;
    synth.flthp_d = 1.0 + entry->hpf_ramp * 0.0003f;
    // Reset vibrato:
    synth.vib_phase = 0.0f;
    synth.vib_speed = pow(entry->vibrato_speed, 2.0) * 0.01f;
    synth.vib_amp = entry->vibrato_depth * 0.5f;
    // Reset envelope:
    synth.env_vol = 0.0f;
    synth.env_stage = 0;
    synth.env_time = 0;
    synth.env_length[0] =
      (int)(entry->env_attack * entry->env_attack * 100000.0f);
    synth.env_length[1] =
      (int)(entry->env_sustain * entry->env_sustain * 100000.0f);
    synth.env_length[2] =
      (int)(entry->env_decay * entry->env_decay * 100000.0f);
    // Reset phaser:
    synth.fphase = pow(entry->phaser_offset, 2.0) * 1020.0f;
    if (entry->phaser_offset < 0.0f) synth.fphase = -synth.fphase;
    synth.fdphase = pow(entry->phaser_sweep, 2.0);
    if (entry->phaser_sweep < 0.0f) synth.fdphase = -synth.fdphase;
    synth.iphase = abs((int)synth.fphase);
    synth.ipp = 0;
    for (int i = 0; i < 1024; ++i)
      synth.phaser_buffer[i] = 0.0f;
    // Refill noise buffer:
    for (int i = 0; i < 32; ++i)
      synth.noise_buffer[i] = az_random(-1.0, 1.0);
    // Reset repeat:
    synth.rep_time = 0;
    synth.rep_limit =
      (int)(pow(1.0f - entry->repeat_speed, 2.0f) * 20000 + 32);
    if (entry->repeat_speed == 0.0f) synth.rep_limit = 0;
  }
}

// Generate the given sound effect and populate synth.wav_buffer with an
// in-memory WAV file of the sound.  This code is taken directly from sfxr,
// with only minor changes.
static void synth_sound_wav(const az_sound_entry_t *entry) {
  const char *end = synth.wav_buffer + AZ_ARRAY_SIZE(synth.wav_buffer);
  char *ptr = synth.wav_data_start;
  reset_synth(entry, false);
  float filesample = 0.0f;
  int fileacc = 0;
  bool finished = false;

  while (ptr < end && !finished) {
    ++synth.rep_time;
    if (synth.rep_limit != 0 && synth.rep_time >= synth.rep_limit) {
      synth.rep_time = 0;
      reset_synth(entry, true);
    }

    // frequency envelopes/arpeggios
    ++synth.arp_time;
    if (synth.arp_limit != 0 && synth.arp_time >= synth.arp_limit) {
      synth.arp_limit = 0;
      synth.fperiod *= synth.arp_mod;
    }
    synth.fslide += synth.fdslide;
    synth.fperiod *= synth.fslide;
    if (synth.fperiod > synth.fmaxperiod) {
      synth.fperiod = synth.fmaxperiod;
      if (entry->freq_limit > 0.0f) finished = true;
    }
    float rfperiod = synth.fperiod;
    if (synth.vib_amp > 0.0f) {
      synth.vib_phase += synth.vib_speed;
      rfperiod = synth.fperiod * (1.0 + sin(synth.vib_phase) * synth.vib_amp);
    }
    synth.period = (int)rfperiod;
    if (synth.period < 8) synth.period = 8;
    synth.square_duty += synth.square_slide;
    if (synth.square_duty < 0.0f) synth.square_duty=0.0f;
    if (synth.square_duty > 0.5f) synth.square_duty=0.5f;
    // volume envelope
    synth.env_time++;
    if (synth.env_time > synth.env_length[synth.env_stage]) {
      synth.env_time = 0;
      ++synth.env_stage;
      if (synth.env_stage == 3) finished = true;
    }
    if (synth.env_stage == 0)
      synth.env_vol = (float)synth.env_time / synth.env_length[0];
    if (synth.env_stage == 1)
      synth.env_vol = 1.0f +
        pow(1.0f - (float)synth.env_time / synth.env_length[1], 1.0f) *
        2.0f * entry->env_punch;
    if (synth.env_stage==2)
      synth.env_vol = 1.0f - (float)synth.env_time / synth.env_length[2];

    // phaser step
    synth.fphase += synth.fdphase;
    synth.iphase = abs((int)synth.fphase);
    if (synth.iphase > 1023) synth.iphase = 1023;

    if (synth.flthp_d != 0.0f) {
      synth.flthp *= synth.flthp_d;
      if (synth.flthp < 0.00001f) synth.flthp=0.00001f;
      if (synth.flthp > 0.1f) synth.flthp=0.1f;
    }

    float ssample = 0.0f;
    for (int si = 0; si < 8; ++si) { // 8x supersampling
      float sample = 0.0f;
      synth.phase++;
      if (synth.phase >= synth.period) {
        synth.phase %= synth.period;
        if (entry->wave_kind == NOISE) {
          for (int i = 0; i < 32; ++i) {
            synth.noise_buffer[i] = az_random(-1.0, 1.0);
          }
        }
      }
      // base waveform
      float fp = (float)synth.phase / synth.period;
      switch (entry->wave_kind) {
        case SQUARE:
          sample = (fp < synth.square_duty ? 0.5f : -0.5f);
          break;
        case SAWTOOTH:
          sample = 1.0f - fp * 2.0f;
          break;
        case TRIANGLE:
          sample = 4.0f * fabs(fp - 0.5f) - 1.0f;
          break;
        case SINE:
          sample = (float)sin(fp * AZ_TWO_PI);
          break;
        case WOBBLE:
          sample = (float)(0.5 * (cos(fp * AZ_TWO_PI) +
                                  sin(2.0 * fp * AZ_TWO_PI)));
          break;
        case NOISE:
          sample = synth.noise_buffer[synth.phase * 32 / synth.period];
          break;
      }
      // lp filter
      float pp = synth.fltp;
      synth.fltw *= synth.fltw_d;
      if (synth.fltw < 0.0f) synth.fltw = 0.0f;
      if (synth.fltw > 0.1f) synth.fltw = 0.1f;
      if (entry->lpf_cutoff != 0.0f) {
        synth.fltdp += (sample - synth.fltp) * synth.fltw;
        synth.fltdp -= synth.fltdp * synth.fltdmp;
      } else {
        synth.fltp = sample;
        synth.fltdp = 0.0f;
      }
      synth.fltp += synth.fltdp;
      // hp filter
      synth.fltphp += synth.fltp - pp;
      synth.fltphp -= synth.fltphp * synth.flthp;
      sample = synth.fltphp;
      // phaser
      synth.phaser_buffer[synth.ipp & 1023] = sample;
      sample += synth.phaser_buffer[(synth.ipp - synth.iphase + 1024) & 1023];
      synth.ipp = (synth.ipp + 1) & 1023;
      // final accumulation and envelope application
      ssample += sample * synth.env_vol;
    }
    const float master_vol = 0.05f;
    ssample = ssample / 8 * master_vol;

    ssample *= 1.0f + entry->volume_adjust;

    ssample *= 4.0f; // arbitrary gain to get reasonable output volume...
    if (ssample > 1.0f) ssample = 1.0f;
    if (ssample < -1.0f) ssample = -1.0f;
    filesample += ssample;
    ++fileacc;
    if (fileacc == 2) {
      filesample /= fileacc;
      fileacc = 0;
      write_uint16le((uint16_t)(filesample * 32000), &ptr);
      filesample = 0.0f;
    }
  }

  // Fill in file and data sizes.
  const size_t data_size = ptr - synth.wav_data_start;
  ptr = synth.wav_data_size;
  write_uint32le(data_size, &ptr);
  assert(synth.wav_data_start > synth.wav_buffer);
  synth.wav_buffer_len = synth.wav_data_start - synth.wav_buffer + data_size;
  ptr = synth.wav_chunk_size;
  write_uint32le(synth.wav_buffer_len - 8, &ptr);
}

static void generate_all_sounds(void) {
  init_wav_buffer();
  AZ_ARRAY_LOOP(entry, sound_entries) {
    synth_sound_wav(entry);
    entry->chunk = Mix_LoadWAV_RW(SDL_RWFromMem(synth.wav_buffer,
                                                synth.wav_buffer_len), true);
    if (entry->chunk == NULL) {
      AZ_FATAL("Failed to generate sound chunk\n");
    }
  }
}

static void free_all_sounds(void) {
  AZ_ARRAY_LOOP(entry, sound_entries) {
    assert(entry->chunk != NULL);
    Mix_FreeChunk(entry->chunk);
    entry->chunk = NULL;
  }
}

static void tick_sounds(const az_soundboard_t *soundboard) {
  static struct {
    enum { INACTIVE = 0, PLAYING, PAUSED, FINISHED } status;
    az_sound_key_t sound;
    int channel;
  } persisting_sounds[NUM_MIXER_CHANNELS];

  // First, go through each of the active persisting_sounds and update their
  // status based on the soundboard.
  bool already_active[soundboard->num_persists];
  memset(already_active, 0, sizeof(bool) * soundboard->num_persists);
  AZ_ARRAY_LOOP(persisting, persisting_sounds) {
    if (persisting->status == INACTIVE) continue;
    // Check if this sound is in the soundboard's persists array.  If not, we
    // will implicitly halt and reset the sound.
    bool reset = true;
    for (int i = 0; i < soundboard->num_persists; ++i) {
      if (soundboard->persists[i].sound == persisting->sound) {
        // We only need to worry about play/pause right now if we're not
        // resetting the sound.  If reset and play are both true, we'll still
        // halt the sound here, and then start it again below.
        if (!soundboard->persists[i].reset) {
          reset = false;
          already_active[i] = true;
          // Resume or pause the sound as needed.
          if (soundboard->persists[i].play) {
            if (persisting->status == PAUSED) {
              Mix_Resume(persisting->channel);
              persisting->status = PLAYING;
            }
          } else {
            if (persisting->status == PLAYING) {
              Mix_Pause(persisting->channel);
              persisting->status = PAUSED;
            }
          }
        }
        break;
      }
    }
    // If the sound has finished playing on its own (meaning its channel is
    // available for use by another sound), mark it as FINISHED so we'll know
    // not to mess with that channel later if we need to halt/pause this sound.
    if (persisting->status == PLAYING && !Mix_Playing(persisting->channel)) {
      persisting->status = FINISHED;
    }
    // If we're supposed to reset this sound, halt it.
    if (reset) {
      if (persisting->status == PLAYING || persisting->status == PAUSED) {
        Mix_HaltChannel(persisting->channel);
      }
      persisting->status = INACTIVE;
    }
  }

  // Second, go through the soundboard and start playing any new persistent
  // sounds that need to be started.
  for (int i = 0; i < soundboard->num_persists; ++i) {
    if (already_active[i] || !soundboard->persists[i].play) continue;
    AZ_ARRAY_LOOP(persisting, persisting_sounds) {
      if (persisting->status == INACTIVE) {
        const az_sound_key_t sound = soundboard->persists[i].sound;
        const int channel =
          Mix_PlayChannel(-1, sound_entries[sound].chunk,
                          (soundboard->persists[i].loop ? -1 : 0));
        if (channel >= 0) {
          persisting->status = PLAYING;
          persisting->sound = sound;
          persisting->channel = channel;
        }
        break;
      }
    }
  }

  // Third, start playing any new one-shot sounds.
  for (int i = 0; i < soundboard->num_oneshots; ++i) {
    Mix_PlayChannel(-1, sound_entries[soundboard->oneshots[i]].chunk, 0);
  }
}

/*===========================================================================*/
// Audio system:

static bool audio_mixer_initialized = false;
static bool audio_mixer_paused = false;

static void shut_down_audio_mixer(void) {
  free_all_sounds();
  free_all_music();
  Mix_CloseAudio();
}

void az_init_audio_mixer(void) {
  assert(!audio_mixer_initialized);
  // Initialize the SDL mixer.
  if (Mix_OpenAudio(AUDIO_RATE, AUDIO_FORMAT, AUDIO_CHANNELS,
                    AUDIO_BUFFERSIZE) != 0) {
    AZ_FATAL("Mix_OpenAudio failed.\n");
  }
  atexit(shut_down_audio_mixer);
  Mix_AllocateChannels(NUM_MIXER_CHANNELS);
  // Load our music and sound data.
  load_all_music();
  generate_all_sounds();
  audio_mixer_initialized = true;
}

static int to_sdl_volume(float volume) {
  return az_imin(az_imax(0, (int)(volume * (float)MIX_MAX_VOLUME)),
                 MIX_MAX_VOLUME);
}

void az_set_global_music_volume(float volume) {
  assert(audio_mixer_initialized);
  assert(!audio_mixer_paused);
  Mix_VolumeMusic(to_sdl_volume(volume));
}

void az_set_global_sound_volume(float volume) {
  assert(audio_mixer_initialized);
  assert(!audio_mixer_paused);
  Mix_Volume(-1, to_sdl_volume(volume));
}

void az_tick_audio_mixer(az_soundboard_t *soundboard) {
  assert(audio_mixer_initialized);
  assert(!audio_mixer_paused);
  tick_music(soundboard);
  tick_sounds(soundboard);
  memset(soundboard, 0, sizeof(*soundboard));
}

void az_pause_all_audio(void) {
  if (!audio_mixer_initialized) return;
  assert(!audio_mixer_paused);
  Mix_Pause(-1); // pause all (non-music) channels
  Mix_PauseMusic();
  audio_mixer_paused = true;
}

void az_unpause_all_audio(void) {
  if (!audio_mixer_initialized) return;
  assert(audio_mixer_paused);
  Mix_Resume(-1); // unpause all (non-music) channels
  Mix_ResumeMusic();
  audio_mixer_paused = false;
}

/*===========================================================================*/
