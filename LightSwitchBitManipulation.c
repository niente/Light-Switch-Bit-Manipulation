/*********************************************************************************
This program lets the user choose from 10 different settings for "theater lights."
They can turn all lights on or off, center/left/right lights on or off, or overlay
a user-inputted pattern starting at the desired bit.
	
	"Theater lights:"
         LEFT CENTER RIGHT
Lights: 11111 111111 11111
1 = ON, 0 = OFF
***************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ON_MASK(numbits, startbit) ((~(((unsigned short)~0) << (numbits))) << (startbit))
#define OFF_MASK(numbits, startbit) (~((~(((unsigned short)~0) << (numbits))) << (startbit)))
#define ALL_MASK (~(((unsigned short)~0) << 16))
#define CENTER_MASK ((~(((unsigned short)~0) << 6)) << 5)
#define LEFT_MASK ((~(((unsigned short)~0) << 5)) << 11)
#define RIGHT_MASK (~(((unsigned short)~0) << 5))
#define FLUSH while( getchar() != '\n')
#define MAX_SIZE 50
#define NUM_SIZE 16
#define ILLEGAL_PATTERN printf("Illegal on/off pattern! Try again!\n")
#define RANGE_ERROR printf("Pattern out of range! Try again!\n")

unsigned short light_switch(unsigned short lights, unsigned short light_mask, int on_flag);
unsigned short overlayBits(unsigned short lights, unsigned short overlay, const int startbit, const int numbits);
void printbits(unsigned short wrd);
int  testbit(unsigned short wrd, int bit_to_test);
void menu();
int cleanInput(char *input);
unsigned short processOverlayInput(char *input, int *startbit, int *numbits);

int main(void)
{
	menu();
	system("pause");
	return 0;
}

/***********************LIGHT_SWITCH****************************
This function takes the current light setting, a mask, and ON or
OFF flag as input. It manipulates the bits of the light setting
- if we turn the lights ON, do lights |= mask
- if we turn the lights OFF, do lights &= mask
Lights are adjusted and returned.
*/
unsigned short light_switch(unsigned short lights, unsigned short light_mask, int on_flag)
{
	return on_flag ? (lights = lights | light_mask) : (lights = lights & light_mask);
}

/***********************OVERLAYBITS******************************
Accepts lights, overlay mask, startbit and numbits as input.
Rewrites the masked part of lights with the overlay.
Lights are adjusted and returned.
*/
unsigned short overlayBits(unsigned short lights, unsigned short overlay, const int startbit, const int numbits)
{
	return lights ^ ((lights ^ overlay) & ON_MASK(numbits, startbit));
}

/***********************CLEANINPUT****************************
Accepts a char * as input from user. Makes a copy to preserve
original when using strtok. Reads first string as number (menu
selection), and checks if there is additional input. Any errors
cause return -1 to re-prompt for new input. If input is good,
menu selection is passed back.
*/
int cleanInput(char *input)
{
	char *firstStr, *secondStr, *end;
	char inputCopy[MAX_SIZE]; // create copy to leave original input string intact
	int selection = 0;
	strcpy(inputCopy, input);
	firstStr = strtok(inputCopy, "\040\t"); // grab first number
	secondStr = strtok(NULL, "\040\t"); // check for additional data in copy of input.
	if (secondStr) // if there was anything else, exit
	{
		printf("Only one menu section allowed! Try again!\n");
		return -1;
	}
	if (firstStr) // if input was not empty
	{
		selection = strtol(firstStr, &end, 10); // convert string to number
		if (*end != '\0' || selection > 10) // if there was text at the end of the number
		{								   // or number was beyond the menu, exit
			printf("Illegal menu selection! Try again!\n");
			return -1;
		}
	}
	return selection; // if everything was good, return the menu selection
}

/***********************MENU****************************
Prompts user for menu selection for which light setting they want.
If they use no input, default is triggered. If any functions fail,
they return menu selection -1 which also goes to default (re-prompt).
*/
void menu()
{
	unsigned short lights = 0, overlay = 0;
	int flag = 0, startbit = 1, numbits = 0;
	char input[MAX_SIZE] = "", overlayInput[MAX_SIZE] = "";
	while (printf("\n\t1) Turn on all lights.\n\t2) Turn on center stage lights.\n\t"), 
		   printf("3) Turn on left stage lights.\n\t4) Turn on right stage lights.\n\t"),
		   printf("5) Turn off all lights.\n\t6) Turn off center stage lights.\n\t"),
		   printf("7) Turn off left stage lights.\n\t8) Turn off right stage lights.\n\t"),
		   printf("9) Enter on/off pattern.\n\t10) Quit the menu.\nEnter menu choice: "), gets(input))
	{
		flag = cleanInput(input);
		switch (flag)
		{
		case 1: // turn on all lights: lights | 11111 111111 11111
			lights = light_switch(lights, ALL_MASK, 1);
			printbits(lights);
			break;
		case 2: // turn on center lights 5-10: lights | [5-10 ON]
			lights = light_switch(lights, CENTER_MASK, 1);
			printbits(lights);
			break;
		case 3: // turn on left 11-15: lights | [11-15 ON]
			lights = light_switch(lights, LEFT_MASK, 1);
			printbits(lights);
			break;
		case 4: // turn on right 0-4: lights | [0-4 ON]
			lights = light_switch(lights, RIGHT_MASK, 1);
			printbits(lights);
			break;
		case 5: // turn off all lights: lights & 00000 000000 00000
			lights = light_switch(lights, ~ALL_MASK, 0);
			printbits(lights);
			break;
		case 6: // turn off center lights: lights & [5-10 OFF]
			lights = light_switch(lights, ~CENTER_MASK, 0);
			printbits(lights);
			break;
		case 7: // turn OFF left: lights & [11-15 OFF] 
			lights = light_switch(lights, ~LEFT_MASK, 0);
			printbits(lights);
			break;
		case 8: // turn OFF right: lights & [0-4 OFF]
			lights = light_switch(lights, ~RIGHT_MASK, 0);
			printbits(lights);
			break;
		case 9:	// overlay pattern
			overlay = 0;
			while (overlay == 0)
			{
				printf("Enter starting light number and on/off pattern: ");
				gets(overlayInput);
				overlay = processOverlayInput(overlayInput, &startbit, &numbits);
			}
			if (overlay != 1) // using 1 to escape previous while loop when input is empty
			{
				lights = overlayBits(lights, overlay, startbit, numbits);
				printbits(lights);
			}
			break;
		case 10: // exit
			return;
		default: // re-prompt user
			break;
		}
	}
}

/***********************TESTBIT****************************
Left bit shift a word, if the value is 1, return 1, else return 0.
*/
int  testbit(unsigned short wrd, int bit_to_test)
{
	wrd >>= bit_to_test;
	wrd &= 1;
	return wrd;
}

/***********************PRINTBITS****************************
For each of 16 bits call testbit to print 0 or 1 of a number.
Add a space between bits 4-5, 10-11 for "lights" formatting.
*/
void printbits(unsigned short wrd)
{
	int  testbit(unsigned short wrd, int bit_to_test);
	int i;
	printf("Light settings: ");
	for (i = 15; i >= 0; i--)
	{
		printf("%1d", testbit(wrd, i));
		if (i == 5 || i == 11)
			printf(" ");
	}
	printf("\n");
	return;
}

/********************PROCESSOVERLAYINPUT*************************
Different input parsing for menu 9. Parse input for 3 strings.
- First string must be an int representing startbit
- Second string must be composed of 0 or 1 and checked for range.
- If a third string is found, re-prompt for new input.
Returns 0 on fail (re prompt), 1 to "exit" menu 9 (no input), 
otherwise returns an unshifted overlay the user entered, and passes
startbit and numbits back to menu() as reference parameters.
*/
unsigned short processOverlayInput(char *input, int *startbit, int *numbits)
{
	char *firstStr, *secondStr, *thirdStr, *end;
	char *binaryDigits = "01";
	int count = 0;
	unsigned short overlay = 0;

	firstStr = strtok(input, "\040\t");
	if (firstStr) // if there was input
	{
		*startbit = strtol(firstStr, &end, 10); // read first number as startbit
		if (*end != '\0') // check for chars at end
		{
			ILLEGAL_PATTERN;
			return 0;
		}
		else if (*startbit > NUM_SIZE)
		{
			RANGE_ERROR;
			return 0;
		}
	}
	else
		return 1; // empty input - go back to main menu

	secondStr = strtok(NULL, "\040\t");
	if (secondStr)
	{
		/// check if chars include ONLY 1 and 0
		count = strspn(secondStr, binaryDigits);
		*numbits = strlen(secondStr);
		if (count != *numbits || *numbits > NUM_SIZE)
		{
			ILLEGAL_PATTERN;
			return 0;
		}
		else if (*numbits + *startbit > NUM_SIZE)
		{
			RANGE_ERROR;
			return 0;
		}
		overlay = strtol(secondStr, &end, 2);
		overlay = overlay << *startbit;
		if (*end != '\0')
		{
			ILLEGAL_PATTERN;
			return 0;
		}
	}
	else // if there was only one entry
	{
		printf("You did not enter a bit pattern! Try again!\n");
		return 0;
	}

	thirdStr = strtok(NULL, "\040\t");
	if (thirdStr)
	{
		printf("Too many entries on line! Try again!\n");
		return 0;
	}
	return overlay;
}

/*
---------SAMPLE OUTPUT #1 (good input)----------

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 1
Light settings: 11111 111111 11111

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 6
Light settings: 11111 000000 11111

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 7
Light settings: 00000 000000 11111

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 8
Light settings: 00000 000000 00000

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 2
Light settings: 00000 111111 00000

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 3
Light settings: 11111 111111 00000

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 4
Light settings: 11111 111111 11111

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 9
Enter starting light number and on/off pattern: 5 101101
Light settings: 11111 101101 11111

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 10
Press any key to continue . . .

*/

/*
---------SAMPLE OUTPUT #2 (bad input)----------


1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice:

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: q
Illegal menu selection! Try again!

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 13
Illegal menu selection! Try again!

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 1 555
Only one menu section allowed! Try again!

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 1x abc
Only one menu section allowed! Try again!

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 1y
Illegal menu selection! Try again!

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 1 1 1 1 1 1
Only one menu section allowed! Try again!

1) Turn on all lights.
2) Turn on center stage lights.
3) Turn on left stage lights.
4) Turn on right stage lights.
5) Turn off all lights.
6) Turn off center stage lights.
7) Turn off left stage lights.
8) Turn off right stage lights.
9) Enter on/off pattern.
10) Quit the menu.
Enter menu choice: 9
Enter starting light number and on/off pattern: 1111 111
Pattern out of range! Try again!
Enter starting light number and on/off pattern: 1 45435
Illegal on/off pattern! Try again!
Enter starting light number and on/off pattern: 15 101
Pattern out of range! Try again!
Enter starting light number and on/off pattern: 13 1011
Pattern out of range! Try again!
Enter starting light number and on/off pattern: 1 x
Illegal on/off pattern! Try again!
Enter starting light number and on/off pattern: 10 10101010
Pattern out of range! Try again!
*/