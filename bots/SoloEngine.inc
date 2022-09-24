#warning

#include BuyAA.inc
#include HudLib.inc
#include Ninjadvloot.inc

| Settings
#define AbilityTimerResetValue			1s
#define AggroRadius 					50
#define ApprovalCount 					10
#define CenterViewButton 				Home
#define GetInstanceTimerResetValue 		6m
#define MoveTimerResetValue 			2m
#define NavPointTimerResetValue 		15m
#define PCCheckRadiusDefaultValue		4000
#define SocialRadius 					30
#define XTargetCount 					20
#define ZoneWaitTimerResetValue 		3m

| Abilities - Discs
#define CraneStanceDisc 		"Crane Stance Rk. II"
#define CurseThirteenFingers	"Curse of the Thirteen Fingers"
#define DichotomicFormDiscId	49132
#define DichotomicFormDiscName	"Dichotomic Form"
#define EarthForceDisc 			"Earthforce Discipline Rk. III"
#define EchoMezDisc 			"Echo of Duplicity Rk. II"
#define RestDisc 				"Breather Rk. III"
#define TigerDisc		 		"Tiger's Poise Rk. II"
| Abilities - AA
#define DistantStrikeAlt 		"Distant Strike"
#define FPPAlt 					"Five Point Palm"
#define InfusionOfThunderAlt 	"Infusion of Thunder"
#define LotDAlt					"Lesson of the Devoted"
#define MovingMountainsAlt 		"Moving Mountains"
#define SecondSpireAlt 			"Fundament: Second Spire of the Sensei"
#define WaspAlt 				"Two-Finger Wasp Touch"
#define ZanFiAlt 				"Zan Fi's Whistle"
| Abilities - AA - Glyphs
#define DPSGlyphNameAlt			"Glyph of Destruction II"
#define WasteGlyphNameAlt		"Glyph of Fireworks I"
| Abilities - Items
#define ChestClickie			"Battleworn Stalwart Moon Soulforge Tunic"
#define CircleOfPowerClickie	"Rage of Rolfron"


#event InstanceBlocked 	"#*#A strange magical presence prevents you from entering#*#"
#event CannotSeeTarget 	"#*#You cannot see your target#*#"


Sub SetupSoloVars
	| General engine flags - not to be changed outside of engine
	/if (!${Defined[navTotal]}) {
		/declare navTotal 						int 	outer 	150
	}
	/declare aaCounter 							int 	outer 	0
	/declare aaCurrent 							int 	outer 	${Me.AAPoints}
	/declare adderIndex 						int 	outer 	0
	/declare aggroCount 						int 	outer 	0
	/declare aggroIdArray[XTargetCount] 		int 	outer
	/declare aggroListIterator 					int 	outer 	0
	/declare approvedPCArray[ApprovalCount] 	string 	outer
	/declare approvedPlayer 					int 	outer 	1
	/declare cannotSeeTarget					bool	outer	FALSE
	/declare combatAbilityTimer					timer	outer	AbilityTimerResetValue
	/declare deadGroupMemberID 					int 	outer 	0
	/declare enemyIdInRange 					int 	outer	0
	/declare getInstanceTimer					timer	outer 	GetInstanceTimerResetValue
	/declare gotStuck 							bool 	outer 	FALSE
	/declare gotWet 							bool 	outer 	FALSE
	/declare haterIndex 						int 	outer 	0
	/declare haveMerc 							bool 	outer 	FALSE
	/declare instanceBlocked 					bool 	outer 	FALSE
	/declare killRange 							int 	outer 	0
	/declare killRanges[${navTotal}] 			int 	outer
	/declare loopStartInvValue					int		outer	0
	/declare mercWinOpen 						bool 	outer
	/declare mobCount 							int 	outer 	0
	/declare moveTimer 							timer 	outer 	MoveTimerResetValue
	/declare navIndex 							int 	outer 	1
	/declare navInterrupt 						bool 	outer 	FALSE
	/declare navTimer 							timer 	outer 	NavPointTimerResetValue
	/declare navLoopCurrentRunTime				int		outer
	/declare navLoopMaxRunTime					int		outer	${Math.Calc[90*60].Int}
	/declare navLoopStartRunTime				int		outer
	/declare pcCheckRadius 						int 	outer 	PCCheckRadiusDefaultValue
	/declare player 							int 	outer 	1
	/declare pullAbilityTimer					timer	outer	AbilityTimerResetValue
	/declare sessionInvValue					int		outer	0
	/declare taskWinOpen						bool	outer	FALSE
	/declare unexpectedPC 						bool 	outer 	FALSE
	/declare useInterruptLoc 					bool 	outer 	FALSE
	/declare usesInstance 						bool 	outer 	FALSE
	/declare wetFightBool 						bool 	outer 	FALSE
	/declare wetPullBool 						bool 	outer 	FALSE
	/declare xloc 								int 	outer 	0
	/declare xLocs[${navTotal}] 				int 	outer
	/declare yloc 								int 	outer 	0
	/declare yLocs[${navTotal}] 				int 	outer
	/declare zoneName 							string 	outer 	${Zone.Name}
	/declare zoneWaitTimer 						timer 	outer
	
	| Convenience settings - can toggle outside of engine, but leave defaults as FALSE
	| Enables debug logging
	/declare debugMode 							bool 	outer 	FALSE
	| Pauses the macro when safe
	/declare pause 								bool 	outer 	FALSE
	| Will reload the currently running mac at the end of a cycle
	/declare resetMac 							bool 	outer 	FALSE
	| Will report stats at next Nav then disable flag
	/declare reportStats						bool	outer	FALSE
	
	| Custom flags
	| To burn glyphs once banked AA hits aaPointCap (burns a dps or waste glyph)
	/declare burnAA 							bool 	outer 	TRUE
	| At what point to burn an AA
	/declare aaPointCap 						int 	outer 	218
	| Manage switching between normal exp and AA if needed
	/declare manageExp							bool	outer	FALSE
	| What # normal exp cap to turn AA back on at. Going above 329 will never turn AA on.
	/declare expBound 							int 	outer 	329
	| TRUE to loot items, FALSE to ignore
	/declare shouldLoot 						bool 	outer 	TRUE
	| Will use LotD
	/declare useLesson 							bool 	outer 	TRUE
	
	
	| Define Merc expectation
	/if (${Me.Mercenary.State.Equal[ACTIVE]}) /varset haveMerc TRUE
	
	| Set Approved Players
	/varset approvedPCArray[1] ${Me.Name}
	/if (${Bool[${Group.GroupSize}]}) {
		/for approvedPlayer 1 to ${Group.GroupSize}
			/if (${Bool[${Group.Member[${approvedPlayer}]}]}) {
				/varset approvedPCArray[${Math.Calc[${approvedPlayer}+1]}] ${Group.Member[${approvedPlayer}].Name}
			}
		/next approvedPlayer
	}
	
	| To add special permissions, modify line below
	| /varset approvedPCArray[${Math.Calc[${Group.GroupSize}+2]}] SomeName
	
	| Setup Looting
	/call SetupAdvLootVars
	/varset useGlobalCache TRUE
	/grouproles set ${Me.Name} 5
	
	| Setup Session Inv Value
	/call ReportInventoryItemValue
	/varset loopStartInvValue ${heldValue}
	/varset sessionInvValue ${heldValue}
/return

Sub GetInstance(String QuestNPC, String requestPhrase, String enterPhrase, String instanceZoneName, String taskName)
	/if (!${usesInstance}) /varset usesInstance TRUE
	/taskremove all
	/doevents flush
	/moveto ID ${NearestSpawn[${QuestNPC}].ID}
	/call SafeTarget ${NearestSpawn[${QuestNPC}].ID}
	/delay 5s ${NearestSpawn[${QuestNPC}].Distance} < 10
	/varset taskWinOpen ${Window[TaskWnd].Open}
	/say ${requestPhrase}
	
	|If task creates a "Select Task" Window...
	/delay 5s ${Window[TaskSelectWnd].Open}
	/if (${Window[TaskSelectWnd].Open}) {
		/notify TaskSelectWnd TSEL_TaskList listselect ${Window[TaskSelectWnd].Child[TSEL_TaskList].List[${taskName}]}
		/delay 2s
		/notify TaskSelectWnd AcceptButton leftmouseup
	}
	
	/call ReportMoney
	/varset loopStartInvValue ${heldValue}
	
	/varset getInstanceTimer GetInstanceTimerResetValue
	/delay 25s
	:instanceBlockedLoop
		/if (${getInstanceTimer} <= 0) {
			/echo Failed to get instance, aborting...  - ${Time.Time24}
			/mqlog Failed to get instance, aborting...
			/call Exit
		}
		
		/delay 25s
		/if (!${taskWinOpen}) /call HideQuestWnd
		/say ${enterPhrase}
		/delay 5s
		/doevents InstanceBlocked
		/if (${instanceBlocked}) {
			/varset instanceBlocked FALSE
			/goto :instanceBlockedLoop
		}
	/call ZoneChangeWait "${instanceZoneName}" TRUE
	/if (!${taskWinOpen}) /call HideQuestWnd
/return

Sub RunNavs
	/if (${useLesson} && ${Me.AltAbilityReady[LotDAlt]}) {
		/echo Using LotD
		/alt act ${Me.AltAbility[LotDAlt].ID}
		/delay 1s
	}
	
	/varset navLoopStartRunTime ${Macro.RunTime}
	:navLoop
	/if (${resetMac} && ${usesInstance}) {
		/echo Resetting macro
	} else {
		/varset navLoopCurrentRunTime ${Math.Calc[${Macro.RunTime} - ${navLoopStartRunTime}]}
		/if (!${gotStuck} && ${Bool[${xLocs[${navIndex}]}]}) {
			/if (${navLoopCurrentRunTime} > ${navLoopMaxRunTime}) {
				/echo navLoopCurrentRunTime (${navLoopCurrentRunTime}) exceeded navLoopMaxRunTime (${navLoopMaxRunTime}), aborting.  Current Index: ${navIndex}
				/mqlog navLoopCurrentRunTime (${navLoopCurrentRunTime}) exceeded navLoopMaxRunTime (${navLoopMaxRunTime}), aborting.  Current Index: ${navIndex}
				/if (!${usesInstance}) /call Exit
			} else {
				/call NextNav
				/goto :navLoop
			}
		}
		/echo NavLoopRunTime completed in ${Math.Calc[(${navLoopCurrentRunTime}/3600)%24].Int}:${Math.Calc[(${navLoopCurrentRunTime}/60)%60].Int}:${Math.Calc[${navLoopCurrentRunTime}%60].Int}. GotStuck: ${gotStuck}
		/if (${gotStuck} && !${usesInstance}) {
			/mqlog NavLoopRunTime completed in ${Math.Calc[(${navLoopCurrentRunTime}/3600)%24].Int}:${Math.Calc[(${navLoopCurrentRunTime}/60)%60].Int}:${Math.Calc[${navLoopCurrentRunTime}%60].Int}. GotStuck: ${gotStuck}
			/call Exit
		}
		| This resetMac location is for non-instanced locations, so the nav loop had a chance to reset
		/if (${resetMac} && !${gotStuck} && !${usesInstance}) /mac ${Macro}
		/varset navIndex 1
		/varset gotStuck FALSE
	}
/return

Sub RemoveInstance(String zoneStatic)
	/taskremove all
	/call FeignDeath
	/call ZoneChangeWait "${zoneStatic}" FALSE
	/if (${resetMac}) /mac ${Macro}
/return

Sub ZoneChangeWait(String NewZone, Bool IsInstance)
	/echo Waiting to reach zone: ${NewZone}
	/varset zoneWaitTimer ZoneWaitTimerResetValue
	:CheckLoop
		/if (${Zone.Name.Equal[${NewZone}]} && ${Me.InInstance} == ${IsInstance}) {
			/echo Reached new zone: ${NewZone}
			/varset zoneName ${NewZone}
			/delay 2s
		} else {
			/call ZoneCheck
			/if (${zoneWaitTimer}==0) {
				/echo Timed out waiting to reach ${NewZone}
				/mqlog Timed out waiting to reach ${NewZone}
				/taskremove all
				/call FeignDeath
				/end
			}
			/delay 1s
			/goto :CheckLoop
		}
/return

Sub Event_InstanceBlocked
    /varset instanceBlocked TRUE
/return

Sub Event_CannotSeeTarget
	/varset cannotSeeTarget TRUE
/return

Sub NextNav
	/call MercManaCheck
	/call HasteCheck
	/call PauseCheck
	/call burnAACheck
	/echo Index:${navIndex}, Y:${yLocs[${navIndex}]} X:${xLocs[${navIndex}]}, RunTime ${Math.Calc[${Macro.RunTime}/86400].Int}:${Math.Calc[(${Macro.RunTime}/3600)%24].Int}:${Math.Calc[(${Macro.RunTime}/60)%60].Int}:${Math.Calc[${Macro.RunTime}%60].Int}
	/if (${reportStats}) {
		/echo Reporting stats:
		/echo -----------------------
		/call ReportMoney
		/varset reportStats FALSE
	}
	/squelch /moveto loc ${yLocs[${navIndex}]} ${xLocs[${navIndex}]}
	/nomodkey /keypress CenterViewButton
	/varset killRange ${killRanges[${navIndex}]}
	/squelch /mapfilter SpellRadius ${killRange}
	/varset navTimer NavPointTimerResetValue
	/varset moveTimer MoveTimerResetValue
	:moveTimerWait
		/squelch /target clear
		/call RunChecks
		/call StuckCheck
		/if (${gotStuck}) /return
		/if (${Me.Feigning}) /goto :moveTimerWait
		/if (!${gotWet}) {
			/call WetFightCheck
			/call WetPullCheck
			/if (${wetFightBool} || ${wetPullBool} || ${deadGroupMemberID} > 0) {
				/if (${debugMode}) /echo Try pull / fight
				/moveto
				/if (!${navInterrupt} && !${gotWet}) {
					/varset xloc ${Me.X}
					/varset yloc ${Me.Y}
					/varset useInterruptLoc TRUE
				}
				/call WaitTilDead
				/if (${deadGroupMemberID} > 0) {
					/call GetDeadGroupBody
				} else {
					/if (${useInterruptLoc} && !${gotWet}) {
						/if (${debugMode}) /echo Move to Interrupt Loc
						/moveto loc ${yloc} ${xloc}
					} else {
						/if (${debugMode}) /echo Continuing on to Nav Loc
						/moveto loc ${yLocs[${navIndex}]} ${xLocs[${navIndex}]}
					}
				}
				/varset navInterrupt TRUE
				/varset moveTimer MoveTimerResetValue
				/goto :moveTimerWait
			}
		}
		/if (${MoveTo.Moving}) {
			/goto :moveTimerWait
		}
		/call ResetWet
		/if (${deadGroupMemberID} > 0) {
			/goto :moveTimerWait
		}
	/varset useInterruptLoc FALSE
	/if (!${navInterrupt}) {
		/varset navIndex ${Math.Calc[${navIndex}+1]}
	} else {
		/varset navInterrupt FALSE
	}
/return

Sub WaitTilDead
	/if (${debugMode}) /echo WaitTilDead
	:killLoop
		/call RunChecks
		/if (${Me.Feigning}) /goto :killLoop
		/call WetFightCheck
		/call WetPullCheck
		
		/if (!${Me.CombatState.Equal[COMBAT]}) {
			/if (${debugMode}) /echo Not Combat
			/squelch /target clear
			/if (!${NearestSpawn[npc radius AggroRadius].ID}) /call HasteCheck
			/if (${gotWet} || ${gotStuck}) /return
			/call LootCheck
			/if (!${wetPullBool}) /return
			/call Pull
		} else {
			/if (!${wetFightBool} || ${gotWet} || ${gotStuck}) {
				/if (${debugMode}) /echo Abort combat wet
				/squelch /attack off
				/squelch /stick off
				/squelch /target clear
				/return
			}
			
			/if (${Target.Dead} || ${Target.Type.Equal[Corpse]}) {
				/squelch /target clear
			}
			
			/call AggroRadiusCount
			/if ((${Me.PctHPs} < 40 || ${aggroCount} > 1)) {
				/if (${Me.CombatAbilityReady[EarthForceDisc]} && !${Melee.DiscID}) /disc EarthForceDisc
			}
			
			/if (${haveMerc} && ${Mercenary.Class.Name.Equal[Cleric]}) {
				/if (${aggroCount} > 1 && ${Mercenary.Stance.NotEqual[REACTIVE]}) /stance reactive
				/if (${aggroCount} < 2 && ${Mercenary.Stance.NotEqual[BALANCED]}) /stance Balanced
			}
			
			| Recheck combat state here to lessen chance of race condition. This is not intended to replace the pull logic above, only to pick another target
			/if (!${Target.ID} && ${Me.CombatState.Equal[COMBAT]}) {
				/call Pull
			}
			
			| Check if mob is aggroed on Merc instead of coming to me, then engage
			/if (${haveMerc} && !${Me.Combat}) {
				/for haterIndex 1 to XTargetCount
					/if (${Me.XTarget[${haterIndex}].TargetType.Equal[Auto Hater]} && ${Me.XTarget[${haterIndex}].ID} && ${SpawnCount[npc radius AggroRadius loc ${Mercenary.X} ${Mercenary.Y} id ${Me.XTarget[${haterIndex}].ID}]} > 0) {
						/call SafeTarget ${Me.XTarget[${haterIndex}].ID}
						
						/if (${Me.TargetOfTarget.ID} != ${Me.ID}) {
							/echo Getting aggro away from Merc
							/if (!${Stick.Active}) /squelch /stick loose 10
							/if (!${Me.Combat}) /squelch /attack on
						}
					}
				/if (!${Me.Combat}) {
					/next haterIndex
				}
			}
			
			/if (${Target.ID} && ${Target.Distance} < AggroRadius) {
				/if (${combatAbilityTimer} == 0) {
					/if (${Me.AltAbilityReady[InfusionOfThunderAlt]}) /alt act ${Me.AltAbility[InfusionOfThunderAlt].ID}
					/if (${Me.AltAbilityReady[SecondSpireAlt]}) /alt act ${Me.AltAbility[SecondSpireAlt].ID}
					/if (${Me.AltAbilityReady[ZanFiAlt]}) /alt act ${Me.AltAbility[ZanFiAlt].ID}
					/if (${Me.CombatAbilityReady[DichotomicFormDiscName]}) /disc DichotomicFormDiscId
					/if (${Me.CombatAbilityReady[TigerDisc]}) /disc TigerDisc
					/if (${Cast.Ready[ChestClickie]}) /casting "ChestClickie"
					/if (${Me.AltAbilityReady[FPPAlt]}) /alt act ${Me.AltAbility[FPPAlt].ID}
					/if (${Me.AltAbilityReady[WaspAlt]}) /alt act ${Me.AltAbility[WaspAlt].ID}
					/if (${Me.CombatAbilityReady[CurseThirteenFingers]}) /disc CurseThirteenFingers
					/if ((${Me.PctHPs} < 40 || ${aggroCount} > 1)) {
						/if (${Cast.Ready[CircleOfPowerClickie]}) /casting "CircleOfPowerClickie"
						/if (${Me.CombatAbilityReady[CraneStanceDisc]}) /disc CraneStanceDisc
					}
					
					/varset combatAbilityTimer AbilityTimerResetValue
				}
				
				/if (!${Me.Feigning}) {
					/if (!${Stick.Active}) /squelch /stick loose 10
					/if (!${Me.Combat}) /squelch /attack on
				}
			}
		}
		
		/delay 1
		/if (${Me.CombatState.Equal[COMBAT]}) {
			/goto :killLoop
		}
	/if (${haveMerc} && ${Mercenary.Stance.NotEqual[BALANCED]} && ${Mercenary.Class.Name.Equal[Cleric]}) /stance Balanced
	/call LootCheck
/return

| Assumption: We know there's a mob that can be pulled
| Return: Pullable mob is engaged and within aggro radius for melee
| Except: Target gets wet, or wanders too far away
Sub Pull
	/if (${debugMode}) /echo Pull
	/call SmartTarget ${killRange}
	/delay 5
	/if (${Target.FeetWet}) /return
	
	/if (${Target.ID}) {
		/squelch /face fast nolook
		/doevents flush CannotSeeTarget
		:PullWaitMob
			/call RunChecks
			/if (${gotWet} || ${gotStuck}) /return
			/if (${Target.Distance} > AggroRadius) {
				/if (${Target.Distance} > ${killRange} || !${Target.LineOfSight}) {
					/if (${debugMode}) /echo Lost pull target..
					
					| If I lose LOS during pull, don't pull a second mob that I do have LOS for
					/if (!${Me.CombatState.Equal[COMBAT]}) {
						/squelch /target clear
						/return
					}
				}
				
				/if (!${Bool[${Me.Casting.ID}]} && ${pullAbilityTimer} == 0) {
					/call EchoSplit
					
					/if (${Target.Distance} > AggroRadius) {
						/if (${Me.AltAbilityReady[DistantStrikeAlt]}) {
							/if (${debugMode}) /echo Distant Strike
							/alt act ${Me.AltAbility[DistantStrikeAlt].ID}
						}
					}
					
					/if (${Target.Casting.ID} || ${Target.Distance} > AggroRadius) {
						/if (${Target.Level} < ${Me.Level} && ${Me.AltAbilityReady[MovingMountainsAlt]}) {
							/if (${debugMode}) /echo Moving Mountains
							/alt act ${Me.AltAbility[MovingMountainsAlt].ID}
						}
					}
					
					/varset pullAbilityTimer AbilityTimerResetValue
				}
				
				/delay 1s ${Me.CombatState.Equal[COMBAT]}
				/doevents CannotSeeTarget
				/if (${cannotSeeTarget} && !${Me.CombatState.Equal[COMBAT]}) {
					/echo Could not see target at Index:${navIndex}, y: ${Me.Y}, x: ${Me.X} - ${Time.Time24}
					/doevents flush CannotSeeTarget
					/squelch /face fast nolook
					/keypress forward hold
					/delay 1s
					/keypress forward
					/varset cannotSeeTarget FALSE
				}
				
				/varset enemyIdInRange ${NearestSpawn[npc range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius AggroRadius].ID}
				/if (${enemyIdInRange} && ${enemyIdInRange} != ${Target.ID} && !${Spawn[${enemyIdInRange}].Dead}) {
					/call SafeTarget ${enemyIdInRange}
					/echo Switched to closer target: ${Target.Name}
				}
				
				/goto :PullWaitMob
			}
		
		/squelch /attack on
	}
/return

Sub SmartTarget(int radius)
	/varset enemyIdInRange ${NearestSpawn[named range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius ${radius} los].ID}
	
	/if (${enemyIdInRange} && ${enemyIdInRange} != ${Target.ID} && !${Spawn[${enemyIdInRange}].Dead}) {
		/call SafeTarget ${enemyIdInRange}
	} else {
		/varset enemyIdInRange ${NearestSpawn[npc range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius ${radius} los].ID}
		/if (${enemyIdInRange} && ${enemyIdInRange} != ${Target.ID} && !${Spawn[${enemyIdInRange}].Dead}) {
			/call SafeTarget ${enemyIdInRange}
		}
	}
	
	/if (${Target.ID}) /echo Targeted ${Target.Name}
/return

Sub SafeTarget(int targetId)
	/squelch /target id ${targetId}
	/delay 2s ${Target.ID} == ${targetId}
/return

Sub EchoSplit
	/if (${SpawnCount[npc radius SocialRadius loc ${Target.X} ${Target.Y}]} > 1 && ${Target.Distance} < 200 && ${Me.CombatAbilityReady[EchoMezDisc]} && !${Me.CombatState.Equal[COMBAT]}) {
		/echo Echo Mezzing...
		/disc EchoMezDisc
		/delay 2s ${Bool[${Target.Mezzed}]}
		
		/declare originalTargetId int local ${Target.ID}
		/for haterIndex 1 to XTargetCount
			/if (${Me.XTarget[${haterIndex}].TargetType.Equal[Auto Hater]} && ${Me.XTarget[${haterIndex}].ID}) {
				/call SafeTarget ${Me.XTarget[${haterIndex}].ID}
				/delay 1s
				/if (!${Bool[${Target.Mezzed}]}) /return
			}
		/next haterIndex
		/if (${Bool[${Target.Mezzed}]}) /call SafeTarget ${originalTargetId}
		/delay 1s
	}
/return

Sub RunChecks
	| Critical checks
	/call DeathCheck
	/call ZoneCheck
	/call PcCheck
	/call GotWetCheck
	/call MercAliveCheck
	/call IsGroupMemberDeadCheck
	/if (!${gotStuck}) /call TookTooLongToReachNavCheck
	
	| Non critical checks
	/if (${manageExp}) /call ExpCheck
	| Keep FeignCheck last, others may feign on their checks
	/call FeignCheck
	| do NOT run StuckCheck inside this Sub, sometimes we let timer expire
	| do NOT run GetDeadGroupBody inside this Sub, we have to wait for when it's appropriate to get it
/return

Sub DeathCheck
	/if (${Me.Dead}) {
		/mqlog I died at Index:${navIndex}, y: ${Me.Y}, x: ${Me.X}
		/mqlog Mobs near me: ${SpawnCount[npc range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius 50]}, Named near me: ${SpawnCount[npc named range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius 50]}
		/call Exit
	}
/return 

Sub ZoneCheck
	/if (${Zone.Name.NotEqual[${zoneName}]}) {
		/mqlog Zone: ${Zone.Name}, expected ${zoneName}
		/call Exit
	}
/return

Sub FeignCheck
	/if (${Me.Feigning}) {
		/if (${Me.CombatState.Equal[COMBAT]}) {
			/for mobCount 1 to ${SpawnCount[npc radius ${killRange}]}
				/if (${NearestSpawn[${mobCount}, npc].Moving}) {
					/delay 1s
					/return
				}
			/next mobCount
		}
		/if (${Me.PctHPs} > 50) {
			/if (${Target.Distance} > AggroRadius) /squelch /target clear
			/stand
		}
		/varset moveTimer MoveTimerResetValue
	}
/return

Sub GotWetCheck
	/if (${Me.FeetWet} || ${Target.FeetWet}) {
		/if (!${gotWet}) {
			/if (${debugMode}) /echo Got Wet
			/varset gotWet TRUE
			/if (${haveMerc} && ${Mercenary.Stance.NotEqual[PASSIVE]}) /stance passive
		}
	}
/return

Sub MercAliveCheck
	/if (${haveMerc}) {
		/if (${Me.Mercenary.State.NotEqual[ACTIVE]}) {
			/echo Merc died at Index: ${navIndex}, y: ${Me.Y}, x: ${Me.X} - ${Time.Time24}
			/mqlog Merc died at Index: ${navIndex}, y: ${Me.Y}, x: ${Me.X}
			/call FeignDeath
			/varset mercWinOpen ${Window[MMGW_ManageWnd].Open}
			:mercLoop
				/nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
				/if (${Me.PctHPs} < 90 && ${Me.AbilityReady[Mend]}) /doability Mend
				/delay 10s
				/if (${Me.Mercenary.State.NotEqual[ACTIVE]}) /goto :mercLoop
			| Give merc a bit to buff up
			/delay 30s
			/varset moveTimer MoveTimerResetValue
			/squelch /target clear
			/if (!${mercWinOpen}) /nomodkey /notify MMGW_ManageWnd 0 close
		}
	}
/return

Sub MercManaCheck
	/if (${haveMerc}) {
		/if (${Mercenary.Class.Name.Equal[Cleric]} && ${Group.Member[${Mercenary.DisplayName}].PctMana} < 20) {
			/echo Cleric Merc ran low on mana, waiting...
			/call FeignDeath
			:mercManaLoop
				/delay 10s
				/if (${Me.Mercenary.State.Equal[ACTIVE]} && ${Group.Member[${Mercenary.DisplayName}].PctMana} < 95) /goto :mercManaLoop
			/echo Cleric Merc mana is above 95%, resuming...
			/varset moveTimer MoveTimerResetValue
			/squelch /target clear
			/call FeignCheck
		}
	}
/return

Sub IsGroupMemberDeadCheck
	/if (${Mercenary.Class.Name.Equal[Cleric]}) {
		/varset deadGroupMemberID 0
		/if (${Bool[${Group.GroupSize}]}) {
			/for approvedPlayer 1 to ${Group.GroupSize}
				/if (${Group.Member[${Math.Calc[${approvedPlayer}-1]}].Dead}) {
					/varset deadGroupMemberID ${Group.Member[${Math.Calc[${approvedPlayer}-1]}].ID}
					/return
				}
			/next approvedPlayer
		}
	}
/return

Sub GetDeadGroupBody
	/if (${Spawn[id ${deadGroupMemberID}].Distance3D} > 100) {
		/squelch /moveto ID ${deadGroupMemberID}
		/delay 5s (${Spawn[id ${deadGroupMemberID}].Distance3D} < 100)
		/squelch /moveto
		/call SafeTarget ${deadGroupMemberID}
		/corpsedrag
	}
/return

Sub ExpCheck
	| Intentionally split this first if so the else block is only comparing exp (didn't want to add exp (float) to an else if)
	/if (${Me.Exp} < ${expBound}) {
		/if (${Window[AAWindow].Child[AAW_PercentCount].Text.NotEqual[0%]}) {
			/if (${debugMode}) /echo Turning AA exp OFF
			/alternateadv off
		}
	} else /if (${Window[AAWindow].Child[AAW_PercentCount].Text.NotEqual[100%]}) {
		/if (${debugMode}) /echo Turning AA exp ON
		/alternateadv on
	}
/return

Sub burnAACheck
	/if (${Me.AAPoints} > ${aaCurrent}) /varcalc aaCounter ${aaCounter}+(${Me.AAPoints}-${aaCurrent})
	
	| Is it time to burn a glyph?
	/if (${burnAA} && ${Me.AAPoints} >= ${aaPointCap}) {
		/echo Burning AA
		
		| If for some reason we failed to buy glyphs earlier, or are just missing them, buy now
		/call BuyAA "DPSGlyphNameAlt"
		/call BuyAA "WasteGlyphNameAlt"
		
		| Burn a glyph
		/if (${Me.AltAbilityReady[DPSGlyphNameAlt]}) {
			| Burn DPS Glyph
			/alt act ${Me.AltAbility[DPSGlyphNameAlt].ID}
			| Buy DPS Glyph
			/delay 5s ${AltAbility[DPSGlyphNameAlt].CanTrain}
			/call BuyAA "DPSGlyphNameAlt"
		} else /if (${Me.AltAbilityReady[WasteGlyphNameAlt]}) {
			| Burn Waste Glyph if AA'd too fast to rebuy dps glyph
			/alt act ${Me.AltAbility[WasteGlyphNameAlt].ID}
			| Buy Waste Glyph
			/delay 5s ${AltAbility[WasteGlyphNameAlt].CanTrain}
			/call BuyAA "WasteGlyphNameAlt"
		}
		
		/delay 5s ${Me.AAPoints} < ${aaPointCap}
	}
	
	/varset aaCurrent ${Me.AAPoints}
/return

Sub WetFightCheck
	/if (${Me.CombatState.Equal[COMBAT]} && !${Target.FeetWet}) {
		/if (!${wetFightBool}) {
			|/if (${debugMode}) /echo wetFightBool TRUE
			/varset wetFightBool TRUE
		}
	} else {
		/if (${wetFightBool}) {
			|/if (${debugMode}) /echo wetFightBool FALSE
			/varset wetFightBool FALSE
		}
	}
/return

Sub WetPullCheck
	/varset enemyIdInRange ${NearestSpawn[npc range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius ${killRange} los].ID}
	
	/if (${enemyIdInRange} && !${Spawn[${enemyIdInRange}].FeetWet}) {
		| Moving targets can keep pathing away from us as we try to pull, creating a shuffle effect that goes on until the mob stops and we're able to pull it before it goes out of range again
		/if (${Spawn[${enemyIdInRange}].Moving} && ${Spawn[${enemyIdInRange}].Distance} > ${Math.Calc[${killRange}-30]}) {
			/varset enemyIdInRange 0
		}
	} else {
		/varset enemyIdInRange 0
	}

	/if (${enemyIdInRange}) {
		/if (!${wetPullBool}) {
			|/if (${debugMode}) /echo wetPullBool TRUE
			/varset wetPullBool TRUE
		}
	} else {
		/if (${wetPullBool}) {
			|/if (${debugMode}) /echo wetPullBool FALSE
			/varset wetPullBool FALSE
		}
	}
/return

Sub LootCheck
	/if (${shouldLoot}) {
		/if (${Me.UseAdvancedLooting}) {
			/call UseAdvLoot
		}
	}
/return

Sub StuckCheck
	/if (${moveTimer} <= 0) {
		/echo Got stuck at Index: ${navIndex}, y: ${Me.Y}, x: ${Me.X}, interrupt? ${useInterruptLoc} - ${Time.Time24}
		/mqlog Got stuck at Index: ${navIndex}, y: ${Me.Y}, x: ${Me.X}, interrupt? ${useInterruptLoc}
		/moveto
		/varset gotStuck TRUE
	}
/return

Sub TookTooLongToReachNavCheck
	/if (${navTimer} <= 0) {
		/echo Took too long to reach NavPoint: ${navIndex}, y: ${Me.Y}, x: ${Me.X}, interrupt? ${useInterruptLoc} - ${Time.Time24}
		/mqlog Took too long to reach NavPoint: ${navIndex}, y: ${Me.Y}, x: ${Me.X}, interrupt? ${useInterruptLoc}
		/moveto
		/varset gotStuck TRUE
	}
/return

Sub ResetWet
	/varset gotWet FALSE
	/if (${haveMerc} && ${Mercenary.Stance.NotEqual[BALANCED]} && ${Mercenary.Class.Name.Equal[Cleric]}) /stance Balanced
/return

Sub AggroRadiusCount
	| Reset counts
	/varset aggroCount 0
	/for haterIndex 1 to XTargetCount
		/varset aggroIdArray[${haterIndex}] NULL
	/next haterIndex
	
	| Start new count
	/for haterIndex 1 to XTargetCount
		/if (${Me.XTarget[${haterIndex}].TargetType.Equal[Auto Hater]} && ${Me.XTarget[${haterIndex}].ID}) {
			/if (${SpawnCount[npc radius AggroRadius id ${Me.XTarget[${haterIndex}].ID}]} > 0) {
				/call AggroListMapper ${Me.XTarget[${haterIndex}].ID}
			}
		}
	/next haterIndex
/return

Sub AggroListMapper(int Id)
	/for aggroListIterator 1 to XTargetCount
		/if (${aggroIdArray[${aggroListIterator}]} == ${Id}) /return
		/if (${aggroIdArray[${aggroListIterator}]} > 0) /next aggroListIterator
		/varset aggroCount ${Math.Calc[${aggroCount} + 1]}
		/varset aggroIdArray[${aggroListIterator}] ${Id}
/return

Sub PcCheck
	/call PcCheckCounter
	/if (${unexpectedPC}) {
		/call FeignDeath
		/echo Waiting for PC's to leave... ${Time.Time24}
		/mqlog Waiting for PC's to leave...
		:PcCheckLoop
			/call PcCheckCounter
			/if (${unexpectedPC}) {
				/delay 10s
				/goto :PcCheckLoop
			}
		/echo PC's gone... ${Time.Time24}
		/mqlog PC's gone...
	}
/return

Sub PcCheckCounter
	/varset unexpectedPC FALSE
	/if (${SpawnCount[pc]}>1) {
		/for player 1 to ${SpawnCount[pc]}
			/varset unexpectedPC TRUE
			/for approvedPlayer 1 to ApprovalCount
				/if (${NearestSpawn[${player}, pc].Name.Equal[${approvedPCArray[${approvedPlayer}]}]}) {
					/varset unexpectedPC FALSE
				} else {
					/next approvedPlayer
				}
		/if (!${unexpectedPC}) /next player
	}
/return

| To refresh all buffs, in between major areas
Sub BuffAndInvis
	/squelch /moveto pause
	:InvisLoop
		/if (${Me.CombatState.Equal[COMBAT]} || ${Me.Invis}) /goto :exitInvisloop
		/if (${Me.Casting.ID}) /goto :InvisLoop
		/if (!${Me.Invis} && ${FindItem[=Mask of Vanishing].ID}) {
			/casting "Mask of Vanishing"
			/delay 6s (${Me.Invis} || !${Me.Casting.ID} || ${Me.CombatState.Equal[COMBAT]})
			/goto :InvisLoop
		}
	:exitInvisloop
	/squelch /moveto unpause
/return

| To be used in between pulls
Sub HasteCheck
	/squelch /moveto pause
	/if (!${Me.CombatState.Equal[COMBAT]} && ${Me.CombatAbilityReady[RestDisc]} && (${Math.Calc[${Me.CurrentEndurance}/${Me.MaxEndurance}]} < 0.25) && ${Me.CurrentEndurance} > 50 && !${Melee.DiscID}) /disc RestDisc
	:HasteLoop
		/if (${Me.CombatState.Equal[COMBAT]} || ${Me.Invis}) /goto :exitHasteCheck
		/if (${Me.Casting.ID}) /goto :HasteLoop
		/if (${Me.Haste} < 170 && ${FindItem[=Lizardscale Plated Girdle].ID}) {
			/casting "Lizardscale Plated Girdle"
			/delay 4s (${Me.Haste} >= 170 || ${Me.Invis} || !${Me.Casting.ID} || ${Me.CombatState.Equal[COMBAT]})
			/goto :HasteLoop
		}
	:exitHasteCheck
	/squelch /moveto unpause
/return

Sub PauseCheck
	/if (${pause}) {
		/echo pausing...
		:pauseWait
			/if (${pause}) {
				/delay 1s
				/goto :pauseWait
			}
			/echo unpaused
	}
/return

Sub FeignDeath
	:feignRetry
		/if (!${Me.Feigning}) {
			/doability "Feign Death"
			| Do not exit this delay early, MQ2Melee will stand back up if it failed, and remove the Feign status
			/delay 2s
			/goto :feignRetry
		}
/return

Sub HideQuestWnd
	/if (${Window[TaskWnd].Open}) /nomodkey /keypress ALT+Q
/return

Sub ReportMoney
	/call ReportInventoryItemValue
	/call CalculateMoney "Money earned in this loop" ${Math.Calc[${heldValue}-${loopStartInvValue}]}
	/call CalculateMoney "Money earned in this session" ${Math.Calc[${heldValue}-${sessionInvValue}]}
/return

Sub Exit
	/taskremove all
	/text Exiting SoloEngine
	/exit
/return

Sub ArrayAdder(int y, int x, int thisKillRange)
	/varset adderIndex ${Math.Calc[${adderIndex}+1]}
	/if (${adderIndex} > ${navTotal}) {
		/echo Ran out of room to add nav points, increase navTotal. adderIndex: ${adderIndex}, navTotal: ${navTotal}
		/return
	}
	/varset xLocs[${adderIndex}] ${x}
	/varset yLocs[${adderIndex}] ${y}
	/if (${Bool[${thisKillRange}]}) {
		/varset killRanges[${adderIndex}] ${thisKillRange}
	} else /if (${adderIndex} > 1) {
		/varset killRanges[${adderIndex}] ${killRanges[${Math.Calc[${adderIndex}-1]}]}
	} else {
		/varset killRanges[${adderIndex}] 0
	}
/return

Sub SetupHudVars
	/call ResetHudArray
	/if (${Defined[HudArray]}) {
		/varset HudArray[1] aaCounter
		/varset HudArray[2] navLoopCurrentRunTime
		/varset HudArray[3] navTimer
		/varset HudArray[4] moveTimer
		/varset HudArray[5] pullAbilityTimer
		/varset HudArray[6] navInterrupt
	}
/return
