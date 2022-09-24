
Sub initCombatState
    /declare combatStatePaused          bool    outer   FALSE
    /declare combatStateAssistTargetId  int     outer   0
    /declare combatStateIsMainTank      bool    outer   FALSE
    /declare combatStateIsOffTank       bool    outer   FALSE
    /declare combatStateIsMainAssist    bool    outer   FALSE
    /declare combatStateIsMainHealer    bool    outer   FALSE
    /declare combatStateIsOffHealer     bool    outer   FALSE
    /declare combatStateIsPuller        bool    outer   FALSE
    /declare combatStateIsMezzer        bool    outer   FALSE

    /squelch /stick set feign on
    /squelch /stick set breakonwarp on
    /squelch /stick set breakdist 200

    /if (${Me.XTarget[1].TargetType.NotEqual[Target's Target]}) /xtarget set 1 targetstarget
	/if (${Raid.Members} > 0) {
		/if (${Me.XTarget[2].TargetType.NotEqual[Raid Assist 1 Target]}) /xtarget set 2 raidassist1target
	} else /if (${Group.GroupSize} > 1) {
		/if (${Me.XTarget[2].TargetType.NotEqual[Group Assist Target]}) /xtarget set 2 groupassisttarget
	} else {
		/if (${Me.XTarget[2].TargetType.NotEqual[Auto Hater]}) /xtarget set 2 autohater
	}
    /xtarget set 3 autohater
    /xtarget set 4 autohater
    /xtarget set 5 autohater

    /call insertPriorityQueue ${priorityCombat} "combatState_check" 20
/return

| Healing
| Tanking

| priority n-1
| fire off all ready abilities
Sub combatState_FireAbilities
    
/return FALSE

Sub combatState_smartTarget(int targetId, int radius)
    /if (!${targetId}) /return
    /if (!${radius}) /varset radius 90

	/declare enemyIdInRange int local ${NearestSpawn[named range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius ${radius} los].ID}
	/if (${enemyIdInRange} && ${enemyIdInRange} != ${Target.ID} && !${Spawn[${enemyIdInRange}].Dead}) {
		/call general_safeTarget ${enemyIdInRange}
	} else {
		/varset enemyIdInRange ${NearestSpawn[npc range ${Math.Calc[${Me.Level} - 20]} ${Math.Calc[${Me.Level} + 20]} radius ${radius} los].ID}
		/if (${enemyIdInRange} && ${enemyIdInRange} != ${Target.ID} && !${Spawn[${enemyIdInRange}].Dead}) {
			/call general_safeTarget ${enemyIdInRange}
		}
	}
	
	/if (${Target.ID}) /echo Targeted ${Target.Name}
    /varset combatStateAssistTargetId ${targetId}	
/return

Sub combatState_assist
    /if (${Me.XTarget[2].ID}) /call general_safeTarget ${Me.XTarget[2].ID}
	/if (${Target.Type.Equal[NPC]}) {
		/stick loose 10
		/delay 3s ${Target.Distance} < 10
		/stick off
		/attack on
		/if (${Target.Type.Equal[NPC]}) /squelch /face nolook
	}
/return

|################# CHECKS ############################

Sub cabby_combatState_check
    /if (${Me.CombatState.NotEqual[COMBAT]}) /return FALSE


    | Does MA have a target for me?
    | did I lose target? handle in a combat check with aquiring new target / assist
    | Are we out of combat?
    | Am I feigning and can stand?
    | do I need to stick?
    | do I need to pause followState?
/return FALSE

|################# COMMANDS ##########################

| Combat state could be on or off at this point
Sub cabby_engage(int targetId)
    /call general_safeTarget ${targetId}

    /if (${combatStateIsMainTank} || ${combatStateIsOffTank}) {
        /stick 10
    } else {
        /stick 10 behindonce
    }



/return TRUE
