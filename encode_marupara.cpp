// encode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>

#define NLLEN 128//short for NameList
#define NLNUM 16
#define BJ 0//short for beijing keji shipin wang, using crf 26
#define TD 1//short for tudou, using 2pass vbr 1500

//batch list of file name
char NameList[NLNUM][NLLEN];
const char Path[] = "C:\\Users\\endlesstory\\Desktop\\after\\marupara";//put .mp4 & .ass here (with the same name!), and list.txt here used to save batch list
const char Maru[] = "D:\\MarukoToolbox";//put marukoToolbox here

//read the file name, return the number of the file to be coded
unsigned char readFileName(void)
{
	FILE *fpName = NULL;
	char listPath[128];
	char len = 0;

	sprintf(listPath, "%s\\list.txt", Path);
	fpName = fopen(listPath, "r");
	if(NULL == fpName)
	{
		printf("error: cannnot open list.txt\n");
		return 0;
	}
	unsigned char num = 0;
	for(/*num = 0*/; num<NLNUM; num++)
	{
		if(!fgets(NameList[num], NLLEN, fpName))//return NULL when EOF
		{
			break;
		}
		len = strlen(NameList[num]);
		if(10 == NameList[num][len-1])
		{
			NameList[num][len-1] = 0;
		}
		else
		{
			printf("error: such a long file name Or forget add an enter...\n");
			return 0;
		}
	}
	return num;
}
//get the batch list, return the number of the file to be coded
unsigned char batchInit(void)
{
	char name[NLLEN];
	gets(name);
	unsigned char batNum;
	if(0 == name[0])
	{
		batNum = readFileName();
	}
	else
	{
		batNum = 1;
		memcpy(NameList[0], name, NLLEN);
	}
	return batNum;
}

//write the .avs
unsigned char writeAvs(unsigned char num)
{
	FILE *avs = NULL;
	char avsPath[256];
	sprintf(avsPath, "%s\\temp\\%s.avs", Path, NameList[num]);
	
	avs = fopen(avsPath, "w");
	if(NULL == avs)
	{
		return 1;	
	}

	fprintf(avs, "LoadPlugin(\"%s\\tools\\avs\\plugins\\LSMASHSource.DLL\")\n", Maru);
	fprintf(avs, "LoadPlugin(\"%s\\tools\\avs\\plugins\\VSFilter.DLL\")\n", Maru);
	fprintf(avs, "LoadPlugin(\"%s\\tools\\avs\\plugins\\VSFilterMod.dll\")\n", Maru);
	fprintf(avs, "LSMASHVideoSource(\"%s\\%s.mp4\")\n", Path, NameList[num]);
	fprintf(avs, "TextSubmod(\"%s\\%s.ass\")\n", Path, NameList[num]);

	fclose(avs);
	return 0;
}
//write the .bat
unsigned char writeBat(unsigned char num, unsigned char mode, FILE *bat)
{
	fprintf(bat, "\"%s\\tools\\ffmpeg\" -y -i \"%%CurPat%%\\%s.mp4\" -f wav \"%%CurPat%%\\temp\\temp.wav\"\n", Maru, NameList[num]);
	fprintf(bat, "\"%s\\tools\\neroAacEnc\" -ignorelength -lc -br 131072 -if \"%%CurPat%%\\temp\\temp.wav\" -of \"%%CurPat%%\\temp\\temp.aac\"\n", Maru);
	fprintf(bat, "\"%s\\tools\\avs4x26x\" -L \"%s\\tools\\x264_64-8bit\" --crf 26 --preset 8 -r 3 -b 3 --me umh -i 1  -o \"%%CurPat%%\\temp\\temp.mp4\" \"%%CurPat%%\\temp\\%s.avs\"\n", Maru, Maru, NameList[num]);
	fprintf(bat, "\"%s\\tools\\MP4Box\" -fps auto -add \"%%CurPat%%\\temp\\temp.mp4\" -add \"%%CurPat%%\\temp\\temp.aac\" -new \"%%CurPat%%\\%s_6h.mp4\"\n", Maru, NameList[num]);
	fprintf(bat, "\n");
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char lstNum = 0;
	unsigned char batNum = 0;
	unsigned char mode = BJ;//***change mode here***//
	FILE *bat = NULL;
	unsigned char err = 0;
	
	bat = fopen("C:\\Users\\endlesstory\\Desktop\\after\\marupara\\temp\\auto.bat", "w");
	if(NULL == bat)
	{
		printf("error: cannot open .bat\n");
		return 1;
	}
	//prepare .bat
	fprintf(bat, "cd %s\n", Path);
	fprintf(bat, "set CurPat=%%cd%%\n");
	fprintf(bat, "cd tool\n");

	//get all file name
	batNum = batchInit();

	//write file
	for(/*lstNum = 0*/; lstNum<batNum; lstNum++)
	{
		err += writeAvs(lstNum);
		if(err)//anyway, writeBat usually no err
		{
			break;
		}
		err += writeBat(lstNum, mode, bat);
	}
	
	//error killer
	if(lstNum != batNum)
	{
		printf("error: %d\n",lstNum);
	}
	if(NLNUM == batNum)
	{
		printf("warning: batch full, check up!\n");
	}

	fclose(bat);

	system("C:\\Users\\endlesstory\\Desktop\\after\\marupara\\temp\\auto.bat");
	return 0;
}