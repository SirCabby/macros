
Sub initGroupState
    /call cabby_groupChanged
    /if (${Bool[${Group.GroupSize}]}) {
        /call event_cabby_newMainTank "unused" "${Group.MainTank}"
        /call event_cabby_newMainAssist "unused" "${Group.MainAssist}"
        /call event_cabby_newPuller "unused" "${Group.Puller}"
    }
/return

Sub general_isInZone(string name)
    /if (${name.Equal[${Me.Name}]}) /return TRUE
    /declare memberIndex int local ${Group.Member[${name}].Index}
    /if (${memberIndex} == 0) /return FALSE
/return ${Group.Member[${memberIndex}].Present}

Sub general_isTank(string name)
    /declare nameClass string local ${Spawn[pc ${name}].Class.ShortName}
    /if (${nameClass.Equal[WAR]} || ${nameClass.Equal[PAL]} || ${nameClass.Equal[SHD]}) /return TRUE
/return FALSE

Sub general_isHealer(string name)
    /declare nameClass string local ${Spawn[pc ${name}].Class.ShortName}
    /if (${nameClass.Equal[CLR]} || ${nameClass.Equal[DRU]} || ${nameClass.Equal[SHM]}) /return TRUE
/return FALSE

Sub general_isPuller(string name)
    /declare nameClass string local ${Spawn[pc ${name}].Class.ShortName}
    /if (${nameClass.Equal[MNK]} || ${nameClass.Equal[BRD]}) /return TRUE
/return FALSE

| Return TRUE if name2 is better
| Prioritize Warriors, otherwise higher level
| TODO: make smarter using ac / hp scoring, need way to read this
Sub general_tankComparer(string name1, string name2)
    /declare class1 string local ${Spawn[pc ${name1}].Class.ShortName}
    /declare class2 string local ${Spawn[pc ${name2}].Class.ShortName}
    /declare level1 int local ${Spawn[pc ${name1}].Level}
    /declare level2 int local ${Spawn[pc ${name2}].Level}
    /if (${class1.Equal[${class2}]}) {
        /if (${level1} < ${level2}) /return TRUE
        /return FALSE
    }
    /if (${class2.Equal[WAR]}) /return TRUE
    /if (${level1} < ${level2}) /return TRUE
/return FALSE

| Return TRUE if name2 is better
| Prioritize Warriors, otherwise higher level
Sub general_pullerComparer(string name1, string name2)
    /declare class1 string local ${Spawn[pc ${name1}].Class.ShortName}
    /declare class2 string local ${Spawn[pc ${name2}].Class.ShortName}
    /declare level1 int local ${Spawn[pc ${name1}].Level}
    /declare level2 int local ${Spawn[pc ${name2}].Level}
    /if (${class1.Equal[${class2}]}) {
        /if (${level1} < ${level2}) /return TRUE
        /return FALSE
    }
    /if (${class2.Equal[BRD]}) /return TRUE
    /if (${level1} < ${level2}) /return TRUE
/return FALSE

|################# CHECKS ############################

| Do leader tasks for new member joining, recheck if not in zone yet
| TODO: handle deaths and when to call this outside of group members joining
Sub cabby_updateGroupRoles_check
    /if (${Group.AnyoneMissing}) /return FALSE

    | Find Main Tank / Assist
    /declare groupIndex int local
    /declare bestTank string local
    /declare thisMember string local 
    /for groupIndex 0 to ${Math.Calc[${Group.GroupSize} - 1]}
        /varset thisMember ${Group.Member[${groupIndex}].Name}
        /call general_isTank ${thisMember}
        /if (${Macro.Return}) {
            /if (${bestTank.Length} < 1) {
                /varset bestTank ${thisMember}
                /continue
            }
            /call general_tankComparer ${bestTank} ${thisMember}
            /if (${Macro.Return}) /varset bestTank ${thisMember}
        }
    /next groupIndex
    /if (${bestTank.Length} > 0) {
        /grouproles set ${bestTank} 1
        /grouproles set ${bestTank} 2
    }

    | Find Puller
    /declare bestPuller string local
    /for groupIndex 0 to ${Math.Calc[${Group.GroupSize} - 1]}
        /varset thisMember ${Group.Member[${groupIndex}].Name}
        /call general_isPuller ${thisMember}
        /if (${Macro.Return}) {
            /if (${bestPuller.Length} < 1) {
                /varset bestPuller ${thisMember}
                /continue
            }
            /call general_pullerComparer ${bestPuller} ${thisMember}
            /if (${Macro.Return}) /varset bestPuller ${thisMember}
        }
    /next groupIndex
    /if (${bestPuller.Length} > 0) /grouproles set ${bestPuller} 3
/return TRUE

|################# COMMANDS ##########################

Sub cabby_invited
    /invite
/return TRUE

Sub cabby_groupChanged
    | Re-assign roles automatically if necessary
    /if (${Group.Leader.Name.Equal[${Me.Name}]}) {
        /call cabby_updateGroupRoles_check
        /if (!${Macro.Return}) {
            /call insertPriorityQueue ${priorityMisc} "updateGroupRoles_check" 50 TRUE
        }
    }

    | Heal / buff stuff here maybe
/return TRUE

Sub cabby_newMainTank(string newMainTank)
    /declare wasMainTank bool local ${combatStateIsMainTank}
    /varset combatStateIsMainTank ${newMainTank.Equal[${Me.Name}]}
    /if (${combatStateIsMainTank}) {
        /varset combatStateIsOffTank FALSE
        /if (!${wasMainTank}) /bc Tanking...
    } else {
        /call general_isTank ${Me.Name}
        /if (${Macro.Return}) {
            /bc Offtanking...
            /if (${wasMainTank}) /varset combatStateIsOffTank TRUE
        }
    }
/return TRUE

Sub cabby_newMainAssist(string newMainAssist)
    /declare wasAssist bool local ${combatStateIsMainAssist}
    /varset combatStateIsMainAssist ${newMainAssist.Equal[${Me.Name}]}
    /if (${combatStateIsMainAssist} && !${wasAssist}) /bc I am now the MA
/return TRUE

Sub cabby_newPuller(string newPuller)
    /declare wasPuller bool local ${combatStateIsPuller}
    /varset combatStateIsPuller ${newPuller.Equal[${Me.Name}]}
    /if (${combatStateIsPuller} && !${wasPuller}) /bc I am now the Puller
/return TRUE

Sub cabby_removedGroup
    /varset combatStateIsMainTank FALSE
    /varset combatStateIsOffTank FALSE
    /varset combatStateIsMainAssist FALSE
    /varset combatStateIsPuller FALSE
/return TRUE
