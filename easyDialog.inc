/*
	easyDialog.inc - Dialogs made easier!

	With this useful include, scripters can easily create
	dialogs and show them to players.

	This include will prevent dialog spoofing, ID collision
	and a lot more.

	Created by Emmet on Friday, January 24, 2014.

	Updated by Southclaws 2017-10-13 to add include guard.

	Editted by Kaperstone 23/03/2018 for id-based dialogs.
*/

#if defined _easyDialog_included
	#endinput
#endif
#define _easyDialog_included

#if !defined isnull
	#define isnull(%1) \
		((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

#define Dialog:%0(%1) \
	forward dialog_%0(%1); public dialog_%0(%1)

#define Dialog_Show(%0,%1, \
	Dialog_Open(%0, #%1,

#define Dialog_Opened(%0) \
	(CallRemoteFunction("Dialog_IsOpened", "i", (%0)))

static
	s_DialogOpened[MAX_PLAYERS]
;

forward OnDialogPerformed(playerid, dialog[], response, success);

forward @dialog_format(); @dialog_format() {
	format("", 0, "");
}

forward Dialog_IsOpened(playerid);
public Dialog_IsOpened(playerid)
{
	return (s_DialogOpened[playerid]==-1?0:1);
}

stock Dialog_Close(playerid)
{
	s_DialogOpened[playerid] = -1;

	return ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");
}

stock Dialog_Open(playerid, dialogid, style, caption[], info[], button1[], button2[], {Float,_}:...)
{
	static
		string[4096],
		args
	;

	if (!strlen(info))
	{
		return 0;
	}
	if ((args = numargs()) > 7)
	{
		while (--args >= 7)
		{
			#emit LCTRL 5
			#emit LOAD.alt args
			#emit SHL.C.alt 2
			#emit ADD.C 12
			#emit ADD
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit PUSH.S info
		#emit PUSH.C 4096
		#emit PUSH.C string

		#emit LOAD.S.pri 8
		#emit CONST.alt 16
		#emit SUB
		#emit PUSH.pri

		#emit SYSREQ.C format
		#emit LCTRL 5
		#emit SCTRL 4

		ShowPlayerDialog(playerid, dialogid, style, caption, string, button1, button2);
	}
	else
	{
		ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
	}
	s_DialogOpened[playerid] = dialogid;

	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	static
		s_Public = cellmax;

	if (s_Public == cellmax)
	{
		s_Public = funcidx("OnDialogPerformed");
	}

	// Sanitize inputs.
	for (new i = 0, l = strlen(inputtext); i < l; i ++)
	{
		if (inputtext[i] == '%')
		{
			inputtext[i] = '#';
		}
		if (inputtext[i] == '{')
		{
			for(new j = i + 1; j - i < 7; j++)
			{
				if ((inputtext[j] >= '0' && inputtext[j] <= '9') || (inputtext[j] >= 'A' && inputtext[j] <= 'F'))
				{
					if (j - i == 6 && inputtext[j + 1] == '}')
					{
						strdel(inputtext, i, j + 2);
						i--;
					}
				}
				else break;
			}
		}
	}
	// Dialog ID -1 is reserved to indicate that the dialog is closed.
	if (dialogid == s_DialogOpened[playerid] && 0 <= dialogid < 32727)
	{
		new
			string[13];

		strcat(string, "dialog_");
		strcat(string, dialogid);

		s_DialogOpened[playerid] = -1;

		if ((s_Public == -1) || (CallLocalFunction("OnDialogPerformed", "dsdd", playerid, string[7], response, funcidx(string) != -1)))
		{
	 		CallLocalFunction(string, "ddds", playerid, response, listitem, (!inputtext[0]) ? ("\1") : (inputtext));
		}
	}
	#if defined DR_OnDialogResponse
		return DR_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	#else
		return 0;
	#endif
}

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif

#define OnDialogResponse DR_OnDialogResponse

#if defined DR_OnDialogResponse
	forward DR_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif
