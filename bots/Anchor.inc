#warning

#include Ninjadvloot.inc

sub SetupAnchorVars
	/if (!${Defined[ignoredMobCount]}) {
		/declare ignoredMobCount 	int 	outer 	20
	}
	/declare yloc 					int 	outer 	${Me.Y}
	/declare xloc 					int 	outer 	${Me.X}
	/declare waiting 				bool 	outer 	FALSE
	/declare doLoot					bool	outer	FALSE
	/if (!${Defined[killRange]}) {
		/declare killRange 			int outer 50
	}
	/declare ignoreAdderIndex 		int 	outer 	0
	/declare IgnoreArray[5] 		string 	outer
	/declare nearestTargetId		int		outer
	/declare ignoreTarget			bool	outer
	
	| Things to override
	/declare killCount				bool	outer	FALSE
	/declare killCountMax 			int 	outer 	1
	/declare ignorePC 				bool 	outer 	FALSE
	/declare ignorePcRange 			int 	outer 	1000
	| If true, then it ignores based on LOS. If false, then it uses range instead
	/declare ignorePcByLOS 			bool 	outer 	FALSE
	
	/echo Anchor at ${yloc} ${xloc}
	
	/mapfilter spellradius ${killRange}
	/moveto dist 10
/return
	
sub KillPulse
	/if (${killCount} && ${killCountMax} < 1) {
		/echo Reached killCountMax, ending...
		/if (${Me.AbilityReady[Feign Death]}) /doability "Feign Death"
		/return
	}
	/if (!${ignorePC} && (!${ignorePcByLOS} && ${SpawnCount[pc radius ${ignorePcRange}]} > 1 || ${ignorePcByLOS} && ${SpawnCount[pc radius ${ignorePcRange} los]} > 1)) {
		/if (!${Me.Feigning} && ${Me.AbilityReady[Feign Death]}) {
			/doability "Feign Death"
		}
		/if (!${waiting}) {
			/if (${Target.ID}) /squelch /target clear
			/echo Waiting for PC's to leave...
			/varset waiting TRUE
		}
		/return
	}
	/varset waiting FALSE
	/if (${Me.Feigning}) {
		/if (${Me.PctHPs} > 50) {
			/squelch /target clear
			/stand
		}
	} else {
		/if (!${Me.Combat}) {
			| Wait until I'm back at anchor before picking new target
			/if (${NearestSpawn[${Me.Name} radius 12 loc ${xloc} ${yloc}].ID}) {
				/if (${doLoot}) {
					/call UseAdvLoot
				}
				
				/varset nearestTargetId ${NearestSpawn[npc radius ${killRange} los].ID}
				/varset ignoreTarget FALSE
				/call CheckIgnored ${nearestTargetId}
				/if (${nearestTargetId} && !${ignoreTarget} && !${Spawn[id nearestTargetId].Dead}) {
					/call SafeTarget ${nearestTargetId}
					/stick loose 10
					/delay 20s ${Spawn[${nearestTargetId}].Distance} < 200
					/if (${Spawn[${nearestTargetId}].Distance} > 200) /return
					/attack on		
					/if (${killCount}) /varcalc killCountMax ${killCountMax}-1
				}
			} else /if (!${Stick.Active} && !${MoveTo.Moving}) {
				/moveto loc ${yloc} ${xloc}
			} else /if (${Target.ID} && ${Target.ID} == ${nearestTargetId} && ${Stick.Active} && ${Target.Distance} < 200) /attack on
		}
		
		/if (${Target.ID} && ${Target.ID} == ${nearestTargetId} && ${Target.Distance} > ${killRange}) /squelch /target clear
	}
/return

Sub setupLoot
	/if (${Group} && !${Group.MasterLooter.ID}) {
		/grouproles set ${Me.Name} 5
		/delay 5s ${Group.MasterLooter.ID}==${Me.ID}
	}
	/call SetupAdvLootVars
	/varset doLoot TRUE
/return

Sub CheckIgnored(int targetId)
	/declare itr int local 1
	/for itr 1 to 3
		/if (${NearestSpawn[id ${targetId}].DisplayName.Find[${IgnoreArray[${itr}]}]}) {
			/varset ignoreTarget TRUE
			/return
		}
	/next itr
/return

Sub SafeTarget(int targetId)
	/squelch /target id ${targetId}
	/delay 2s ${Target.ID} == ${targetId}
	/echo Targeted: ${Target.Name}
/return

Sub IgnoreArrayAdder(string Name)
	/varset ignoreAdderIndex ${Math.Calc[${ignoreAdderIndex}+1]}
	/if (${ignoreAdderIndex} > ${ignoredMobCount}) {
		/echo Ran out of room to add ignored npcs, increase ignoredMobCount. ignoreAdderIndex: ${ignoreAdderIndex}, ignoredMobCount: ${ignoredMobCount}
		/return
	}
	
	/varset IgnoreArray[${ignoreAdderIndex}] ${Name}
	
	/echo Ignore: ${Name}
/return
