#include "CC_Include.h"
#include "locvar.h"

BOOL g_bYDWEEnumUnitsInRangeMultipleFlag = FALSE;
BOOL g_bForForceMultipleFlag = FALSE;

BOOL _fastcall  CC_PutAction_SearchVar(DWORD This, DWORD OutClass);
void _fastcall  CC_PutActionEx_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Type, DWORD Endl);

void _fastcall 
	CC_PutActionEx_ForLoop_GetVarName(DWORD This, DWORD cc_guiid_type, char* varname)
{
	char varname_t[260];

	switch (cc_guiid_type)
	{
	case CC_GUIID_ForLoopA:
	case CC_GUIID_ForLoopAMultiple:
		BLZSStrCopy(varname, "bj_forLoopAIndex", 260);
		break;
	case CC_GUIID_ForLoopB:
	case CC_GUIID_ForLoopBMultiple:
		BLZSStrCopy(varname, "bj_forLoopBIndex", 260);
		break;
	case CC_GUIID_ForLoopVar:
	case CC_GUIID_ForLoopVarMultiple:
		CC_GetGlobalVar((*(DWORD**)(This+0x12C))[0], 0, varname, 260);
		break;
	case CC_GUIID_YDWEForLoopLocVarMultiple:
		BLZSStrPrintf(varname_t, 260, "ydul_%s", ((char*)&GetGUIVar_Value(This, 0)));
		ConvertString(varname_t, varname, 260);
		break;
	}
}

void CC_PutActionEx_ForLoop(DWORD This, DWORD OutClass, char* name, DWORD cc_guiid_type)
{
	char buff[260];
	char varname[260];

	CC_PutActionEx_ForLoop_GetVarName(This, cc_guiid_type, varname);

	CC_PutBegin();

	switch (cc_guiid_type)
	{
	case CC_GUIID_ForLoopA:
	case CC_GUIID_ForLoopB:
	case CC_GUIID_ForLoopAMultiple:
	case CC_GUIID_ForLoopBMultiple:
		BLZSStrPrintf(buff, 260, "set %s = ", varname);
		PUT_CONST(buff, 0);
		PUT_VAR(This, 0);
		PUT_CONST("", 1);

		BLZSStrPrintf(buff, 260, "set %sEnd = ", varname);
		PUT_CONST(buff, 0);
		PUT_VAR(This, 1);
		PUT_CONST("", 1);
		break;
	case CC_GUIID_ForLoopVar:
	case CC_GUIID_ForLoopVarMultiple:
	case CC_GUIID_YDWEForLoopLocVarMultiple:
		BLZSStrPrintf(buff, 260, "set %s = ", varname);
		PUT_CONST(buff, 0);
		PUT_VAR(This, 1);
		PUT_CONST("", 1);
		break;
	}

	PUT_CONST("loop", 1);

	CC_PutBegin();
	switch (cc_guiid_type)
	{
	case CC_GUIID_ForLoopA:
	case CC_GUIID_ForLoopAMultiple: 
	case CC_GUIID_ForLoopB:
	case CC_GUIID_ForLoopBMultiple:   
		BLZSStrPrintf(buff, 260, "exitwhen %s > %sEnd", varname, varname);
		PUT_CONST(buff, 1);
		break;
	case CC_GUIID_ForLoopVar:
	case CC_GUIID_ForLoopVarMultiple:
	case CC_GUIID_YDWEForLoopLocVarMultiple:
		BLZSStrPrintf(buff, 260, "exitwhen %s > ", varname);
		PUT_CONST(buff, 0);
		PUT_VAR(This, 2);
		PUT_CONST("", 1);
		break;
	}
	CC_PutEnd();

	switch (cc_guiid_type)
	{
	case CC_GUIID_ForLoopA:
	case CC_GUIID_ForLoopB:  
		CC_PutVar_Code(This, OutClass, name, 2, CC_GUI_TYPE_ACTION);
		break;
	case CC_GUIID_ForLoopVar: 
		CC_PutVar_Code(This, OutClass, name, 3, CC_GUI_TYPE_ACTION);
		break;
	case CC_GUIID_ForLoopAMultiple: 
	case CC_GUIID_ForLoopBMultiple: 
	case CC_GUIID_ForLoopVarMultiple:
	case CC_GUIID_YDWEForLoopLocVarMultiple:
		CC_PutBlock_Action(This, OutClass, name, 0);
		break;
	}  

	CC_PutBegin();
	BLZSStrPrintf(buff, 260, "set %s = %s + 1", varname, varname);
	PUT_CONST(buff, 1);
	CC_PutEnd();

	PUT_CONST("endloop", 1);
	CC_PutEnd();
}

void CC_PutActionEx_ForGroupEnum(DWORD PThis, DWORD This, DWORD OutClass, char* name)
{
	if (This)
	{
		switch (*(uint32_t*)(This+0x138))
		{
		case CC_GUIID_GetUnitsInRectMatching:
			PUT_CONST("set ydl_group = CreateGroup()", 1);
			PUT_CONST("call GroupEnumUnitsInRect(ydl_group, ", 0); 
			PUT_VAR(This, 0);
			PUT_CONST(", null)", 1); 
			return ;
		case CC_GUIID_GetUnitsInRangeOfLocMatching: 
			PUT_CONST("set ydl_group = CreateGroup()", 1);
			PUT_CONST("call GroupEnumUnitsInRangeOfLoc(ydl_group, ", 0); 
			PUT_VAR(This, 1);
			PUT_CONST(", ", 0); 
			PUT_VAR(This, 0);
			PUT_CONST(", null)", 1); 
			return ;
		case CC_GUIID_GetUnitsOfPlayerMatching:
			PUT_CONST("set ydl_group = CreateGroup()", 1);
			PUT_CONST("call GroupEnumUnitsOfPlayer(ydl_group, ", 0); 
			PUT_VAR(This, 0);
			PUT_CONST(", null)", 1); 
			return ;
		default:
			break;
		}

		char gui_name[260];
		CC_GetGUIName(This, 0, gui_name, 260);
		if (   strcmp(gui_name, "CreateGroup") == 0
			|| strcmp(gui_name, "GetUnitsInRectAll") == 0
			|| strcmp(gui_name, "GetUnitsInRectOfPlayer") == 0
			|| strcmp(gui_name, "GetUnitsInRangeOfLocAll") == 0
			|| strcmp(gui_name, "GetUnitsOfPlayerAll") == 0
			|| strcmp(gui_name, "GetUnitsOfPlayerAndTypeId") == 0
			|| strcmp(gui_name, "GetUnitsOfTypeIdAll") == 0
			|| strcmp(gui_name, "GetUnitsSelectedAll") == 0
			)
		{
			PUT_CONST("set ydl_group = ", 0);
			PUT_VAR(PThis, 0);
			PUT_CONST("", 1);
			return ;
		}
	}

	PUT_CONST("if bj_wantDestroyGroup then", 1);
	CC_PutBegin();
	PUT_CONST("set bj_wantDestroyGroup = false", 1);
	PUT_CONST("set ydl_group = ", 0);
	PUT_VAR(PThis, 0);
	PUT_CONST("", 1);
	CC_PutEnd();
	PUT_CONST("else", 1);
	CC_PutBegin();
	PUT_CONST("set bj_groupAddGroupDest = CreateGroup()", 1);
	PUT_CONST("call ForGroup(", 0);
	PUT_VAR(PThis, 0);
	PUT_CONST(", function GroupAddGroupEnum)", 1);
	PUT_CONST("set ydl_group = bj_groupAddGroupDest", 1);
	CC_PutEnd();
	PUT_CONST("endif", 1);
}

void CC_PutActionEx_ForGroupConditionBegin(DWORD This, DWORD OutClass, char* name)
{
	if (This)
	{
		switch (*(uint32_t*)(This+0x138))
		{
		case CC_GUIID_GetUnitsInRangeOfLocMatching:
			CC_PutSearchCondition(GetGUIVar_Class(This, 2), OutClass, name);
			PUT_CONST("if (", 0);
			CC_PutVar_Code(This, OutClass, name, 2, CC_GUI_TYPE_CONDITION);
			PUT_CONST(") then", 1);
			return;
		case CC_GUIID_GetUnitsInRectMatching:
		case CC_GUIID_GetUnitsOfPlayerMatching:
			CC_PutSearchCondition(GetGUIVar_Class(This, 1), OutClass, name);
			PUT_CONST("if (", 0);
			CC_PutVar_Code(This, OutClass, name, 1, CC_GUI_TYPE_CONDITION);
			PUT_CONST(") then", 1);
			return;
		default:
			break;
		}
	}
	CC_PutEnd();
}

void CC_PutActionEx_ForGroupConditionEnd(DWORD This, DWORD OutClass, char* name)
{
	if (This)
	{
		switch (*(uint32_t*)(This+0x138))
		{
		case CC_GUIID_GetUnitsInRectMatching:
		case CC_GUIID_GetUnitsInRangeOfLocMatching:
		case CC_GUIID_GetUnitsOfPlayerMatching:
			PUT_CONST("endif", 1);
			return ;
		default:
			break;
		}
	}
	CC_PutBegin();
}

void CC_PutActionEx_ForGroup(DWORD This, DWORD OutClass, char* name)
{
	uint32_t ui_type = *(uint32_t*)(This+0x138);
	if (g_bYDWEEnumUnitsInRangeMultipleFlag)
	{
		ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_YDWEEnumUnitsInRangeMultiple");
	}
	else
	{
		g_bYDWEEnumUnitsInRangeMultipleFlag = TRUE;

		CC_PutBegin();
		CC_PutActionEx_ForGroupEnum(This, GetGUIVar_Class(This, 0), OutClass, name);
		PUT_CONST("loop", 1);
		CC_PutBegin();
		PUT_CONST("set ydl_unit = FirstOfGroup(ydl_group)", 1);
		PUT_CONST("exitwhen ydl_unit == null", 1);
		PUT_CONST("call GroupRemoveUnit(ydl_group, ydl_unit)", 1);
		CC_PutActionEx_ForGroupConditionBegin(GetGUIVar_Class(This, 0), OutClass, name);
		if (CC_GUIID_ForGroup == *(DWORD*)(This+0x138))
		{
			CC_PutActionEx_Hook(GetGUIVar_Class(This, 1), 0, OutClass, name, CC_GUI_TYPE_ACTION, 0);
		}
		else
		{
			CC_PutBlock_Action(This, OutClass, name, 0);
		}
		CC_PutActionEx_ForGroupConditionEnd(GetGUIVar_Class(This, 0), OutClass, name);
		CC_PutEnd();
		PUT_CONST("endloop", 1);
		PUT_CONST("call DestroyGroup(ydl_group)", 1);
		CC_PutEnd();

		g_bYDWEEnumUnitsInRangeMultipleFlag = FALSE;
	}
}

void CC_PutActionEx_ForGroupMatching(DWORD This, DWORD OutClass, char* name)
{
	uint32_t ui_type = *(uint32_t*)(This+0x138);

	if (g_bYDWEEnumUnitsInRangeMultipleFlag)
	{
		ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_YDWEEnumUnitsInRangeMultiple");
	}
	else
	{
		g_bYDWEEnumUnitsInRangeMultipleFlag = TRUE;

		CC_PutBegin();
		PUT_CONST("set ydl_group = CreateGroup()", 1);
		PUT_CONST("set ydl_tmp_group = CreateGroup()", 1);
		if (ui_type == CC_GUIID_GetUnitsInRectMatching)
		{
			PUT_CONST("call GroupEnumUnitsInRect(ydl_tmp_group, ", 0); 
			PUT_VAR(This, 0);
			PUT_CONST(", null)", 1); 
		}
		else if (ui_type == CC_GUIID_GetUnitsInRangeOfLocMatching)
		{
			PUT_CONST("call GroupEnumUnitsInRangeOfLoc(ydl_tmp_group, ", 0); 
			PUT_VAR(This, 1);
			PUT_CONST(", ", 0); 
			PUT_VAR(This, 0);
			PUT_CONST(", null)", 1); 
		}
		else
		{
			PUT_CONST("call GroupEnumUnitsOfPlayer(ydl_tmp_group, ", 0); 
			PUT_VAR(This, 0);
			PUT_CONST(", null)", 1); 
		}

		PUT_CONST("loop", 1);
		CC_PutBegin();
		PUT_CONST("set ydl_unit = FirstOfGroup(ydl_tmp_group)", 1);
		PUT_CONST("exitwhen ydl_unit == null", 1);
		PUT_CONST("call GroupRemoveUnit(ydl_tmp_group, ydl_unit)", 1);
		if (ui_type == CC_GUIID_GetUnitsInRangeOfLocMatching)
		{
			CC_PutSearchCondition(GetGUIVar_Class(This, 2), OutClass, name);
			PUT_CONST("if (", 0);
			CC_PutVar_Code(This, OutClass, name, 2, CC_GUI_TYPE_CONDITION);
			PUT_CONST(") then", 1);
		}
		else
		{
			CC_PutSearchCondition(GetGUIVar_Class(This, 1), OutClass, name);
			PUT_CONST("if (", 0);
			CC_PutVar_Code(This, OutClass, name, 1, CC_GUI_TYPE_CONDITION);
			PUT_CONST(") then", 1);
		}

		CC_PutBegin();
		PUT_CONST("call GroupAddUnit(ydl_group, ydl_unit)", 1);
		CC_PutEnd();
		PUT_CONST("endif", 1);
		CC_PutEnd();
		PUT_CONST("endloop", 1);
		PUT_CONST("call DestroyGroup(ydl_tmp_group)", 1);
		CC_PutEnd();

		g_bYDWEEnumUnitsInRangeMultipleFlag = FALSE;
	}
}

void CC_PutActionEx_ForForce(DWORD This, DWORD OutClass, char* name)
{
	if (g_bForForceMultipleFlag)
	{
		ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_ForForceMultiple");
	}
	else
	{
		g_bForForceMultipleFlag = TRUE;
		
		if (GetGUIVar_Class(This, 0) && (CC_GUIID_GetPlayersMatching == *(DWORD*)(GetGUIVar_Class(This, 0)+0x138)))
		{
			CC_PutBegin();
			PUT_CONST("set ydl_index = 0", 1);
			PUT_CONST("loop", 1);
			CC_PutBegin();
			PUT_CONST("exitwhen ydl_index >= 16", 1);
			CC_PutSearchCondition(GetGUIVar_Class(This, 0), OutClass, name);
			PUT_CONST("if (GetPlayerSlotState(Player(ydl_index)) != PLAYER_SLOT_STATE_EMPTY) and (", 0);
			CC_PutVar_Code(GetGUIVar_Class(This, 0), OutClass, name, 0, CC_GUI_TYPE_CONDITION);
			PUT_CONST(") then", 1);
			if (CC_GUIID_ForForce == *(DWORD*)(This+0x138))
			{
				CC_PutBegin();
				CC_PutActionEx_Hook(GetGUIVar_Class(This, 1), 0, OutClass, name, CC_GUI_TYPE_ACTION, 0);
				CC_PutEnd();
			}
			else
			{
				CC_PutBlock_Action(This, OutClass, name, 0);
			}
			PUT_CONST("endif", 1);
			PUT_CONST("set ydl_index = ydl_index + 1", 1);
			CC_PutEnd();
			PUT_CONST("endloop", 1);
			CC_PutEnd();
		}
		else
		{
			CC_PutBegin();
			PUT_CONST("set ydl_force = ", 0); 
			PUT_VAR(This, 0); 
			PUT_CONST("", 1); 
			PUT_CONST("set ydl_index = 0", 1);
			PUT_CONST("loop", 1);
			CC_PutBegin();
			PUT_CONST("exitwhen ydl_index >= 16", 1);
			PUT_CONST("if IsPlayerInForce(Player(ydl_index), ydl_force) then", 1);
			if (CC_GUIID_ForForce == *(DWORD*)(This+0x138))
			{
				CC_PutBegin();
				CC_PutActionEx_Hook(GetGUIVar_Class(This, 1), 0, OutClass, name, CC_GUI_TYPE_ACTION, 0);
				CC_PutEnd();
			}
			else
			{
				CC_PutBlock_Action(This, OutClass, name, 0);
			}
			PUT_CONST("endif", 1);
			PUT_CONST("set ydl_index = ydl_index + 1", 1);
			CC_PutEnd();
			PUT_CONST("endloop", 1);
			CC_PutEnd();
		}

		g_bForForceMultipleFlag = FALSE;
	}
}

void CC_PutActionEx_ForForceMatching(DWORD This, DWORD OutClass, char* name)
{
	uint32_t ui_type = *(uint32_t*)(This+0x138);

	if (g_bForForceMultipleFlag)
	{
		ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_YDWEEnumUnitsInRangeMultiple");
	}
	else
	{
		g_bForForceMultipleFlag = TRUE;

		CC_PutBegin();
		PUT_CONST("set ydl_force = CreateForce()", 1);
		PUT_CONST("set ydl_index = 0", 1);
		PUT_CONST("loop", 1);
		CC_PutBegin();
		PUT_CONST("exitwhen ydl_index >= 16", 1);
		CC_PutSearchCondition(GetGUIVar_Class(This, 0), OutClass, name);
		PUT_CONST("if (GetPlayerSlotState(Player(ydl_index)) != PLAYER_SLOT_STATE_EMPTY) and (", 0);
		CC_PutVar_Code(This, OutClass, name, 0, CC_GUI_TYPE_CONDITION);
		PUT_CONST(") then", 1);
		CC_PutBegin();
		PUT_CONST("call ForceAddPlayer(ydl_force, Player(ydl_index))", 1);
		CC_PutEnd();
		PUT_CONST("endif", 1);
		PUT_CONST("set ydl_index = ydl_index + 1", 1);
		CC_PutEnd();
		PUT_CONST("endloop", 1);
		CC_PutEnd();

		g_bForForceMultipleFlag = FALSE;
	}
}

void CC_PutSearchCondition(DWORD This, DWORD OutClass, char* name)
{
	cc_search_var(This, [&](uint32_t var_ptr)
	{
		switch (*(uint32_t*)(var_ptr+0x138))
		{
		case CC_GUIID_GetUnitsInRectMatching:
		case CC_GUIID_GetUnitsInRangeOfLocMatching:
		case CC_GUIID_GetUnitsOfPlayerMatching:
			{
				CC_PutActionEx_ForGroupMatching(var_ptr, OutClass, name);
			}
			break;
		case CC_GUIID_GetPlayersMatching:
			{
				CC_PutActionEx_ForForceMatching(var_ptr, OutClass, name);
			}
			break;
		}
	});
}

void CC_PutSearchCondition2(DWORD This, DWORD OutClass, char* name)
{
	DWORD nItemCount = *(DWORD*)(This+0xC);
	for (DWORD i = 0; i < nItemCount; i++)
	{
		DWORD nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
		if (Proc_GetGUIType(nItemClass) == CC_GUI_TYPE_CONDITION)
		{
			if (*(DWORD*)(nItemClass+0x13C) != 0)
			{
				CC_PutSearchCondition(nItemClass, OutClass, name);
			}
		}
	}
}

void CC_PutSearchCondition3(DWORD This, DWORD OutClass, char* name, DWORD index)
{
	DWORD nItemCount = *(DWORD*)(This+0xC);

	for (DWORD i = 0; i < nItemCount; i++)
	{
		DWORD nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
		if (*(DWORD*)(nItemClass+0x13C) != 0)
		{
			if ((index) == -1 || (*(DWORD*)(nItemClass+0x154) == index))
			{
				CC_PutSearchCondition(nItemClass, OutClass, name);
			}
		}
	}
}


void _fastcall 
	CC_PutActionEx_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Type, DWORD Endl)
{
	char buff[260];

	if (CC_GUIID_ForGroupMultiple == *(DWORD*)(This+0x138)
		|| CC_GUIID_ForGroup == *(DWORD*)(This+0x138))
	{
		CC_PutActionEx_ForGroup(This, OutClass, name);
		return ;
	}
	else if (CC_GUIID_ForForceMultiple == *(DWORD*)(This+0x138)
		|| CC_GUIID_ForForce == *(DWORD*)(This+0x138))
	{
		CC_PutActionEx_ForForce(This, OutClass, name);
		return ;
	}
	else
	{
		CC_PutSearchCondition(This, OutClass, name);
	}

	switch (*(DWORD*)(This+0x138))
	{
	case CC_GUIID_IfThenElse:
		{
			CC_PutSearchCondition(GetGUIVar_Class(This, 0), OutClass, name);
			CC_PutBegin();
			PUT_CONST("if (", 0);
			CC_PutVar_Code(This, OutClass, name, 0, CC_GUI_TYPE_CONDITION);
			PUT_CONST(") then", 1);
			CC_PutVar_Code(This, OutClass, name, 1, CC_GUI_TYPE_ACTION);
			PUT_CONST("else", 1);
			CC_PutVar_Code(This, OutClass, name, 2, CC_GUI_TYPE_ACTION);
			PUT_CONST("endif", 1);
			CC_PutEnd();

			break;
		}
	case CC_GUIID_IfThenElseMultiple:
		{
			CC_PutSearchCondition3(This, OutClass, name, 0);
			CC_PutBegin();
			PUT_CONST("if (", 0);
			CC_PutBlock_Condition_And(This, OutClass, name, 0);
			PUT_CONST(") then", 1);
			CC_PutBlock_Action(This, OutClass, name, 1);
			PUT_CONST("else", 1);
			CC_PutBlock_Action(This, OutClass, name, 2);
			PUT_CONST("endif", 1);
			CC_PutEnd();

			break;
		}

	case CC_GUIID_GroupEnumUnitsInRange:
	case CC_GUIID_GroupEnumUnitsInRangeCounted:
	case CC_GUIID_GroupEnumUnitsInRangeOfLoc:
	case CC_GUIID_GroupEnumUnitsInRangeOfLocCounted:
		{
			uint32_t ui_type = *(uint32_t*)(This+0x138);
			if (g_bYDWEEnumUnitsInRangeMultipleFlag)
			{
				ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_YDWEEnumUnitsInRangeMultiple");
			}
			else
			{
				g_bYDWEEnumUnitsInRangeMultipleFlag = TRUE;

				CC_PutBegin();
				PUT_CONST("set ydl_group = ", 0);
				PUT_VAR(This, 0);
				PUT_CONST("", 1);
				PUT_CONST("call GroupClear(ydl_group)", 1);
				PUT_CONST("set ydl_tmp_group = CreateGroup()", 1);
				if (ui_type == CC_GUIID_GroupEnumUnitsInRangeOfLoc
					|| ui_type == CC_GUIID_GroupEnumUnitsInRangeOfLocCounted)
				{
					PUT_CONST("call GroupEnumUnitsInRangeOfLoc(ydl_tmp_group, ", 0); 
					PUT_VAR(This, 1);
					PUT_CONST(", ", 0);
					PUT_VAR(This, 2);
					PUT_CONST(", null)", 1); 
				}
				else
				{
					PUT_CONST("call GroupEnumUnitsInRange(ydl_tmp_group, ", 0); 
					PUT_VAR(This, 1);
					PUT_CONST(", ", 0); 
					PUT_VAR(This, 2);
					PUT_CONST(", ", 0); 
					PUT_VAR(This, 3);
					PUT_CONST(", null)", 1); 
				}
				PUT_CONST("loop", 1);
				CC_PutBegin();
				PUT_CONST("set ydl_unit = FirstOfGroup(ydl_tmp_group)", 1);
				PUT_CONST("exitwhen ydl_unit == null", 1);
				PUT_CONST("call GroupRemoveUnit(ydl_tmp_group, ydl_unit)", 1);
				if (ui_type == CC_GUIID_GroupEnumUnitsInRangeOfLoc
					|| ui_type == CC_GUIID_GroupEnumUnitsInRangeOfLocCounted)
				{
					CC_PutSearchCondition(GetGUIVar_Class(This, 3), OutClass, name);
					PUT_CONST("if (", 0);
					CC_PutVar_Code(This, OutClass, name, 3, CC_GUI_TYPE_CONDITION);
					PUT_CONST(") then", 1);
				}
				else
				{
					CC_PutSearchCondition(GetGUIVar_Class(This, 4), OutClass, name);
					PUT_CONST("if (", 0);
					CC_PutVar_Code(This, OutClass, name, 4, CC_GUI_TYPE_CONDITION);
					PUT_CONST(") then", 1);
				}
				CC_PutBegin();
				PUT_CONST("call GroupAddUnit(ydl_group, ydl_unit)", 1);
				CC_PutEnd();
				PUT_CONST("endif", 1);
				CC_PutEnd();
				PUT_CONST("endloop", 1);
				PUT_CONST("call DestroyGroup(ydl_tmp_group)", 1);
				CC_PutEnd();

				g_bYDWEEnumUnitsInRangeMultipleFlag = FALSE;
			}
		}
		break;
	case CC_GUIID_YDWEEnumUnitsInRangeMultiple:
		{
			if (g_bYDWEEnumUnitsInRangeMultipleFlag)
			{
				ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_YDWEEnumUnitsInRangeMultiple");
			}
			else
			{
				g_bYDWEEnumUnitsInRangeMultipleFlag = TRUE;

				CC_PutBegin();
				PUT_CONST("set ydl_group = CreateGroup()", 1);
				PUT_CONST("call GroupEnumUnitsInRange(ydl_group, ", 0); 
				PUT_VAR(This, 0);
				PUT_CONST(", ", 0); 
				PUT_VAR(This, 1);
				PUT_CONST(", ", 0); 
				PUT_VAR(This, 2);
				PUT_CONST(", null)", 1); 
				PUT_CONST("loop", 1);
				CC_PutBegin();
				PUT_CONST("set ydl_unit = FirstOfGroup(ydl_group)", 1);
				PUT_CONST("exitwhen ydl_unit == null", 1);
				PUT_CONST("call GroupRemoveUnit(ydl_group, ydl_unit)", 1);
				CC_PutEnd();
				CC_PutBlock_Action(This, OutClass, name, 0);
				PUT_CONST("endloop", 1);
				PUT_CONST("call DestroyGroup(ydl_group)", 1);
				CC_PutEnd();

				g_bYDWEEnumUnitsInRangeMultipleFlag = FALSE;
			}
		}
		break;
	case CC_GUIID_ForLoopA:
	case CC_GUIID_ForLoopB:
	case CC_GUIID_ForLoopVar:
	case CC_GUIID_ForLoopAMultiple:
	case CC_GUIID_ForLoopBMultiple:
	case CC_GUIID_ForLoopVarMultiple:
	case CC_GUIID_YDWEForLoopLocVarMultiple:
		{ 
			CC_PutActionEx_ForLoop(This, OutClass, name, *(DWORD*)(This+0x138));
			break;
		}
	case CC_GUIID_ReturnAction:
		{
			locvar::return_before(This, OutClass);
			CC_PutBegin();
			PUT_CONST("return", 1);
			CC_PutEnd();
			break;
		}
	case CC_GUIID_YDWESetAnyTypeLocalVariable:
		{
			locvar::set(This, OutClass, name);
			break;  
		}
	case CC_GUIID_YDWETimerStartMultiple:
		{   
			CC_PutBegin();
			PUT_CONST("set "YDL_TIMER" = ", 0);
			PUT_VAR(This, 0);
			PUT_CONST("", 1);
			CC_PutEnd();

			locvar::params(This, OutClass, name, 0, YDL_TIMER);

			CC_PutBegin();
			PUT_CONST("call TimerStart("YDL_TIMER", ", 0);
			PUT_VAR(This, 1);
			PUT_CONST(", ", 0);
			PUT_VAR(This, 2);
			BLZSStrPrintf(buff, 260, ", function %sT)", name);
			PUT_CONST(buff, 1);
			CC_PutEnd();

			break;
		}
	case CC_GUIID_YDWETimerStartFlush:
		locvar::flush_in_timer(This, OutClass);
		break;
	case CC_GUIID_TriggerSleepAction:
		CC_PutBegin();
		PUT_CONST("call TriggerSleepAction(", 0);
		PUT_VAR(This, 0);
		PUT_CONST(")", 1);
		locvar::sleep_after(This, OutClass);
		CC_PutEnd();
		break;
	case CC_GUIID_PolledWait:
		CC_PutBegin();
		PUT_CONST("call PolledWait(", 0);
		PUT_VAR(This, 0);
		PUT_CONST(")", 1);
		locvar::sleep_after(This, OutClass);
		CC_PutEnd();
		break;
	case CC_GUIID_YDWERegisterTriggerMultiple:
		{
			CC_PutBegin();
			PUT_CONST("set "YDL_TRIGGER" = ", 0);
			PUT_VAR(This, 0);
			PUT_CONST("", 1);
			CC_PutEnd();

			locvar::params(This, OutClass, name, 1, YDL_TRIGGER);

			// Event  
			CC_PutBlock_Event(This, OutClass, name, 0, YDL_TRIGGER);

			CC_PutBegin();
			BLZSStrPrintf(buff, 260, "call TriggerAddCondition("YDL_TRIGGER", Condition(function %sConditions))", name);
			PUT_CONST(buff, 1); 
			CC_PutEnd();

			break;
		}
	case CC_GUIID_YDWERegisterTriggerFlush:
		locvar::flush_in_trigger(This, OutClass);
		break;
	case CC_GUIID_YDWESaveAnyTypeDataByUserData:
		{
			CC_PutBegin();
			CC_Put_YDWESaveAnyTypeDataByUserData(This, OutClass, name);
			CC_PutEnd();
			break;
		}
	case CC_GUIID_YDWEFlushAnyTypeDataByUserData:
		{
			CC_PutBegin();
			CC_Put_YDWEFlushAnyTypeDataByUserData(This, OutClass, name);
			CC_PutEnd();
			break;
		}
	case CC_GUIID_YDWEFlushAllByUserData:
		{
			CC_PutBegin();
			CC_Put_YDWEFlushAllByUserData(This, OutClass, name);
			CC_PutEnd();
			break;
		}
	case CC_GUIID_YDWEActivateTrigger:
		{
			LPCSTR lpszTrigVar = (char*)&GetGUIVar_Value(This, 0);
			LPCSTR lpszMode = (char*)&GetGUIVar_Value(This, 1);

			if (0 == BLZSStrCmp("gg_trg_", lpszTrigVar, sizeof("gg_trg_")-1))
			{
				if (0 == BLZSStrCmp("OnOffOn", lpszMode, 0x7FFFFFFF))
				{
					CC_PutBegin();
					BLZSStrPrintf(buff, 260, "call ExecuteFunc(\"InitTrig_%s_Orig\")", lpszTrigVar+(sizeof("gg_trg_")-1));
					PUT_CONST(buff, 1); 
					CC_PutEnd();
				}
				else if (0 == BLZSStrCmp("OnOffOff", lpszMode, 0x7FFFFFFF))
				{
					CC_PutBegin();
					BLZSStrPrintf(buff, 260, "call InitTrig_%s_Orig()", lpszTrigVar+(sizeof("gg_trg_")-1));
					PUT_CONST(buff, 1); 
					CC_PutEnd();
				}
			}

			break;
		}
	case CC_GUIID_AddTriggerEvent:
		{
			DWORD nVarClass = GetGUIVar_Class(This, 1);
			if (!nVarClass)
			{
				return ;
			}

			CC_PutBegin();

			char szName[260];
			char szTrigName[260];

			CC_GetGUIName(nVarClass, 0, szName, 260);
			BLZSStrPrintf(buff, 260, "call %s(", szName);
			PUT_CONST(buff, 0);
			PUT_VAR(This, 0);

			DWORD nVar = *(DWORD*)(nVarClass+0x128);
			BLZSStrPrintf(szTrigName, 260, "%s002", name);
			for (DWORD i = 0; i < nVar; ++i)
			{
				PUT_CONST(", ", 0);
				CC_PutVar(nVarClass, 0, OutClass, szTrigName, i, 2, 1);
			}
			PUT_CONST(")", 1);

			CC_PutEnd();
		}
		break;
	default:
		{
			if (CC_PutAction_SearchVar(This, OutClass))
			{
				ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_ANYPLAYER");
			}
			CC_PutActionEx(This, EDX, OutClass, name, Type, Endl);
		}
		break;
	}
}

void _fastcall
	CC_PutAction(DWORD This, DWORD OutClass, char* name, DWORD index, DWORD Endl)
{
	char NewName[260];
	BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, index+1);

	CC_PutActionEx_Hook(This, 0, OutClass, NewName, CC_GUI_TYPE_ACTION, Endl);
}