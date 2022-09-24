#warning

#define CommanderCount 10
#define CommandSubCount 100

#event CommandEvent "[#1#(msg)] #2#"

Sub InitCommand
	/declare receiveCommands 								outer 	TRUE
	/declare commandChannel 										IG
	/declare commandMasters[CommanderCount] 		string 	outer
	/declare arrayInterator 						int 	outer 	0
	/declare commandSubRoutines[CommandSubCount]	string	outer
	/declare numCommandSubs							int		outer	0
	
	/varset commandMasters[1] "Ishtass"
	/bccmd channels ${commandChannel}
/return

Sub DoCommands
	/doevents CommandEvent
/return

Sub SendCommand(String command)
	/bct ${commandChannel} ${command}
/return

Sub Event_CommandEvent(Line, commander, command)
	/if (${receiveCommands}) {
		/if (${IsCommander[${commander}]}) {
			/call PerformCommand "${command}"
		}
	}
/return

Sub IsCommander(String commander)
/return ${ArrayContains[commandMasters, CommanderCount, ${commander}]}

Sub IsRegisteredCommand(String command)
/return ${ArrayContains[commandSubRoutines, ${numCommandSubs}, ${command}]}

Sub RegisterCommandSub(String subName)
	/varcalc numCommandSubs ${numCommandSubs} + 1
	/varset commandSubRoutines[${numCommandSubs}] ${subName}
/return

Sub PerformCommand(String commandStr)
	/declare command local ${commandStr.Arg[1]}
	/if (!${IsRegisteredCommand[${command}]}) /return
	
	/declare commandArgs local ${GetCommandArgs[${commandStr}]}
	
	/call ${command} "${commandArgs}"
/return

Sub GetCommandArgs(string s)
	/declare result local
	/if (${NumArgs[${s}]} > 1) {
		/declare commandLen local ${s.Arg[1].Length}
		/declare remainderLen local
		/varcalc remainderLen ${s.Length} - ${commandLen} - 1
		/varset result ${s.Right[${remainderLen}]}
	}
/return ${result}

Sub NumArgs(String s)
	/declare numArgs int local 0
	/if (${s.Length} > 0) {
		/varcalc numArgs ${s.Count[ ]} + 1
	}
/return ${numArgs}

Sub ArrayContains(string arrayName, int arraySize, string findStr)
	/declare result local FALSE
	
	/for arrayInterator 1 to ${arraySize}
		/if (${findStr.Equal[${${arrayName}[${arrayInterator}]}]}) {
			/varset result TRUE
		} else {
			/next arrayInterator
		}
/return ${result}
