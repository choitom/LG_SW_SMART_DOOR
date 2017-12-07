#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "speaker.h"

static void voice_command(const char *words)
{
    //system("rm say.wav");
    char cmd[26];
    char tts[128];
    char say[14];

    strcpy(cmd, "pico2wave --wave say.wav ");
    snprintf(tts, 128, "%s\"%s\"", cmd ,words);
    strcpy(say, "aplay say.wav");
    system(tts);
    system(say);
}

void welcome(const char *name)
{
    char str[30] = "Hi ";
    strcat(str, name);
    strcat(str, ", the door's opening");
    voice_command(str);
}

#endif
