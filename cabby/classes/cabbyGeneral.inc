
Sub initGeneral
    /call initPriorities
    /call initFollowState
    /call initCombatState
    /call initGroupState
/return

Sub general_safeTarget(int targetId)
	/squelch /target id ${targetId}
	/delay 2s ${Target.ID} == ${targetId}
/return

|################# CHECKS ############################

|################# COMMANDS ##########################

Sub cabby_do(string command)
    /docommand ${command}
/return TRUE

Sub cabby_sendTell(string command)
    /declare to string local ${command.Arg[1, ]}
    /declare msgLength int local ${Math.Calc[${command.Length} - ${to.Length} - 1]}
    /declare message string local
    /if (${msgLength} > 0) /varset message ${command.Right[${msgLength}]}
    /tell ${to} ${message}
/return TRUE

Sub cabby_tellToMe(string message)
    /bct ${configRelayTellsTo} ${message}
/return TRUE

Sub cabby_tellNotOnline(string to)
    /bct ${configRelayTellsTo} (${to}) was not online for tell
/return TRUE

Sub cabby_refresh
    /mac cabby
/return TRUE
