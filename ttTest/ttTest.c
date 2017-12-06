#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void voice_command(const char *words)
{
    system("rm say.wav");
    char cmd[26];
    char tts[128];
    char say[14];
	
    strcpy(cmd, "pico2wave --wave say.wav ");
	strcpy(say, "aplay say.wav");

	snprintf(tts, 128, "%s\"%s\"", cmd, words);
	system(tts);
	system(say);
}

void unwelcome()
{
    char str[80] = "Sorry";
    voice_command(str);
}

void welcome(const char *name)
{
    char str[80] = "Hi ";
	strcat(str, name);
	strcat(str, ", the door's opening");
    voice_command(str);
}
