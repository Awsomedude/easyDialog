/*
	easyDialog.inc - Dialogs made easier!

	With this useful include, scripters can easily create
	dialogs and show them to players.

	This include will prevent dialog spoofing, ID collision
	and a lot more.

	Created by Emmet on Friday, January 24, 2014.

	Updated by Southclaws 2017-10-13 to add include guard.

    Updated by Emmett 2021-05-02 to speed up code execution, code cleanup...
*/

#if defined _easyDialog_included
	#endinput
#endif
#define _easyDialog_included

#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_va>

#define Dialog:%0(%1) \
	forward dialog_%0(%1); public dialog_%0(%1)

#define Dialog_Show(%0,%1, \
	Dialog_Open(%0, #%1,

static
	s_DialogName[MAX_PLAYERS][32 char];

/**
  * API
 */
forward OnDialogPerformed(playerid, const dialog[], response, success);

/**
  * Functions
 */
stock Dialog_Close(playerid)
{
	s_DialogName[playerid]{0} = 0;

	return ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");
}

stock Dialog_Open(playerid, const function[], style, const caption[], const info[], const button1[], const button2[], {Float,_}:...)
{
	static string[4096];

	if (info[0] == EOS) {
		return 0;
	}

	if (numargs() < 7) {
        ShowPlayerDialog(playerid, 32700, style, caption, info, button1, button2);
        return 1;
	}

	va_format(string, sizeof(string), info, ___(7));

	ShowPlayerDialog(playerid, 32700, style, caption, string, button1, button2);
	strpack(s_DialogName[playerid], function, 32 char);

	return 1;
}

/**
  * Hooks
 */
hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	static s_Public;

	s_Public = funcidx("OnDialogPerformed");
	
	if (dialogid == 32700) {
		new string[40]; // 7 + 32 + \0

		strcat(string, "dialog_");
		strcat(string, s_DialogName[playerid]);

		Dialog_Close(playerid);

		if ((s_Public == -1) || (CallLocalFunction("OnDialogPerformed", "dsdd", playerid, string[7], response, funcidx(string) != -1))) {
	 		CallLocalFunction(string, "ddds", playerid, response, listitem, (!inputtext[0]) ? ("\1") : (inputtext));
		}
	}

	return Y_HOOKS_CONTINUE_RETURN_0;
}
