#warning

#define ZoneWaitTimerResetValue 3m

#event InstanceBlocked "#*#A strange magical presence prevents you from entering#*#"

Sub SetupFollowVars
	/declare zoneName string outer ${Zone.Name}
	/declare zoneWaitTimer timer outer
	/declare instanceBlocked bool outer FALSE
	/declare master string outer ""
/return

Sub EnterInstance(String QuestNPC, String enterPhrase, String instanceZoneName)
	/doevents flush
	/if (${Zone.Name.NotEqual[${instanceZoneName}]}) {
		/moveto id ${NearestSpawn[${QuestNPC}].ID}
		/target id ${NearestSpawn[${QuestNPC}].ID}
		/delay 3s
		:getInstanceLoop
			/if (!${Bool[${Task.Title}]}) {
				/delay 3s
				/goto :getInstanceLoop
			}
		:instanceBlockedLoop
			/delay 25s
			/call HideQuestWnd
			/say ${enterPhrase}
			/delay 5s
			/doevents InstanceBlocked
			/if (${instanceBlocked}) {
				/varset instanceBlocked FALSE
				/goto :instanceBlockedLoop
			}
		/call ZoneChangeWait "${instanceZoneName}"
		/call HideQuestWnd
	} else {
		/echo Already in instance
	}
/return

Sub HideQuestWnd
	/if (${Window[TaskWnd].Open}) /nomodkey /keypress ALT+Q
/return

Sub ZoneChangeWait(String NewZone)
	/echo Waiting to reach zone: ${NewZone}
	/varset zoneWaitTimer ZoneWaitTimerResetValue
	:CheckLoop
		/if (${Zone.Name.Equal[${NewZone}]}) {
			/echo Reached new zone: ${NewZone}
			/varset zoneName ${NewZone}
			/delay 2s
		} else {
			/call ZoneCheck
			/if (${zoneWaitTimer}==0) {
				/echo Timed out waiting to reach ${NewZone}
				/mqlog Timed out waiting to reach ${NewZone}
				/end
			}
			/delay 1s
			/goto :CheckLoop
		}
	:mHereLoop
		/if (!${Spawn[${master}].ID}) {
			/delay 1s
			/goto :mHereLoop
		}
/return

Sub ZoneCheck
	/if (${Zone.Name.NotEqual[${zoneName}]}) {
		/mqlog Zone: ${Zone.Name}, expected ${zoneName}
		/call Exit
	}
/return

Sub Exit
	/taskremove all
	|/exit
	/end
/return

Sub FollowLoop
	:haveInstanceLoop
		/if (${Bool[${Task.Title}]}) {
			/if (${Me.Dead} && ${AdvPath.Following}) {
				/echo Died
				/afollow off
			} else /if (!${AdvPath.Following} && ${Spawn[${master}].ID}) {
				/target ${master}
				/follow
			}
			/delay 1s
			/goto :haveInstanceLoop
		}
/return

Sub RemoveInstance(String zoneStatic)
	/call ZoneChangeWait "${zoneStatic}"
/return

Sub Event_InstanceBlocked
    /varset instanceBlocked TRUE
/return
