#warning

#include Anchor.inc

#define defaultNotifyTime 5s

sub SetupNotifyVars
	/if (!${Defined[mobCount]}) {
		/declare mobCount 				int 	outer 	20
	}
	/declare adderIndex 				int 	outer 	0
	/declare player 					int 	outer 	1
	/declare playerId					int		outer
	/declare PlayerInZone 				string 	outer
	/declare pcCount 					int 	outer 	1
	/declare mob 						int 	outer 	1
	/declare MobArray[${mobCount}] 		string 	outer
	/declare PopArray[${mobCount}] 		bool 	outer
	/declare BeepArray[${mobCount}] 	bool 	outer
	/declare notifyTimer				timer 	outer 	0
	
	| Things to set
	/declare doAnchor					bool	outer	FALSE
	/declare doBeep						bool	outer	TRUE
	/declare doText						bool	outer	FALSE
	/declare endOnTargetKill			bool	outer	FALSE
/return

sub CheckTargets
	/for mob 1 to ${mobCount}
		/if (${Bool[${MobArray[${mob}]}]}) {
			/if ((${NearestSpawn[npc ${MobArray[${mob}]}].CleanName.Equal[${MobArray[${mob}]}]} && !${PopArray[${mob}]}) || (!${NearestSpawn[npc ${MobArray[${mob}]}].CleanName.Equal[${MobArray[${mob}]}]} && ${PopArray[${mob}]})) {
				/if (${PopArray[${mob}]}) {
					/echo ${MobArray[${mob}]} died at ${Time.Time24}
					/mqlog ${MobArray[${mob}]} died
					/if (${doText}) /text ${MobArray[${mob}]} died
					/varset PopArray[${mob}] FALSE
					/if (${endOnTargetKill}) {
						/doability "Feign Death"
						/delay 3s
						/call UseAdvLoot
						/echo Target Killed, ending macro
						/end
					}
				} else {
					/echo ${MobArray[${mob}]} spawned at ${Time.Time24}
					/mqlog ${MobArray[${mob}]} spawned
					/if (${doText}) /text ${MobArray[${mob}]} spawned
					|/high ${MobArray[${mob}]}
					/varset PopArray[${mob}] TRUE
				}
			}
			/if (${PopArray[${mob}]}) {
				/call PCList
				/if (${doBeep} && ${BeepArray[${mob}]} && ${notifyTimer} <= 0) {
					/varset notifyTimer defaultNotifyTime
					/beep
				}
			}
			/next mob
		}
	/if (${doAnchor}) /call KillPulse
/return

sub setupAnchor
	/call SetupAnchorVars
	/varset doAnchor TRUE
/return

sub PCList
	/if (${Defined[PlayerArray]}) {
		/if (${PlayerArray.Size} > 0) {
			/for player 1 to ${PlayerArray.Size}
				/if (!${NearestSpawn[${PlayerArray[${player}]}].ID}) {
					/mqlog Left Zone: ${PlayerArray[${player}]}
					/echo Left Zone: ${PlayerArray[${player}]} Time: ${Time}
					/if (${Defined[${PlayerArray[${player}]}InZone]}) {
						/varset ${PlayerArray[${player}]}InZone FALSE
					}
				}
			/next player
		}
		/deletevar PlayerArray
	}
	/varset pcCount ${SpawnCount[pc]}
	/declare PlayerArray[${Math.Calc[${pcCount}-1]}] string outer
	/if (${pcCount} > 1) {
		/for player 2 to ${pcCount}
			/varset playerId ${NearestSpawn[${player}, pc].ID}
			/varset PlayerArray[${Math.Calc[${player}-1]}] ${NearestSpawn[id ${playerId}].Name}
			/varset PlayerInZone ${NearestSpawn[id ${playerId}].Name}InZone
			/if (!${Defined[${PlayerInZone}]}) {
				/declare ${PlayerInZone} bool outer FALSE
			}
			/if (!${${PlayerInZone}}) {
				/mqlog Entered Zone: ${NearestSpawn[id ${playerId}].Level} ${NearestSpawn[id ${playerId}].Class} ${NearestSpawn[id ${playerId}].Name} <${NearestSpawn[id ${playerId}].Guild}> Loc: ${Float[${NearestSpawn[id ${playerId}].Y}].Int},${Float[${NearestSpawn[id ${playerId}].X}].Int},${Float[${NearestSpawn[id ${playerId}].Z}].Int}
				/echo Entered Zone: ${NearestSpawn[id ${playerId}].Level} ${NearestSpawn[id ${playerId}].Class} ${NearestSpawn[id ${playerId}].Name} <${NearestSpawn[id ${playerId}].Guild}> Loc: ${Float[${NearestSpawn[id ${playerId}].Y}].Int},${Float[${NearestSpawn[id ${playerId}].X}].Int},${Float[${NearestSpawn[id ${playerId}].Z}].Int} Time: ${Time}
				/varset ${PlayerInZone} TRUE
			}
		/next player
	}
/return

Sub ArrayAdder(string Name, bool Beep)
	/varset adderIndex ${Math.Calc[${adderIndex}+1]}
	/if (${adderIndex} > ${mobCount}) {
		/echo Ran out of room to add named, increase mobCount. adderIndex: ${adderIndex}, mobCount: ${mobCount}
		/return
	}
	
	/varset MobArray[${adderIndex}] ${Name}
	/varset PopArray[${adderIndex}] FALSE
	/varset BeepArray[${adderIndex}] ${Beep}
	
	/echo Notifying: ${Name} - Beep?: ${Beep}
/return
