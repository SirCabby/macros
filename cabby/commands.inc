#include cabby_inc\priorities.inc

| Returns TRUE if command accepted, FALSE if not
Sub scheduleCommand(speaker, priority, command, who)
    /if (${who.Length} && !${Me.Name.Equal[${who}]}) {
        /return FALSE
    }
    /call isOwner ${speaker}
    /declare isOwner bool local ${Macro.Return}
    
    /if (${isOwner}) /call insertPriorityQueue ${priority} "${command}"
/return TRUE


|###################################################
|###############   BCCOMMANDS   ####################
|###################################################

#event cabby_do "<#1#> do #2#"
#event cabby_do "<#1#> #3# do #2#"
#event cabby_do "[#1#(msg)] do #2#"

Sub event_cabby_do(Line, speaker, command, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "do ${command}" ${who}
/return

|---------------------------------------------------

#event cabby_followMe "<#1#> followme"
#event cabby_followMe "<#1#> #2# followme"
#event cabby_followMe "[#1#(msg)] followme"

Sub event_cabby_followMe(Line, speaker, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "followMe ${speaker}" ${who}
/return

|---------------------------------------------------

#event cabby_stopFollow "<#1#> stopfollow"
#event cabby_stopFollow "<#1#> #2# stopfollow"
#event cabby_stopFollow "[#1#(msg)] stopfollow"

Sub event_cabby_stopFollow(Line, speaker, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "stopFollow" ${who}
/return

|---------------------------------------------------

#event cabby_move2me "<#1#> m2m"
#event cabby_move2me "<#1#> #2# m2m"
#event cabby_move2me "[#1#(msg)] m2m"

Sub event_cabby_move2me(Line, speaker, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "move2me ${speaker}" ${who}
/return

|---------------------------------------------------

#event cabby_clickZone "<#1#> clickzone"
#event cabby_clickZone "<#1#> #2# clickzone"
#event cabby_clickZone "[#1#(msg)] clickzone"

Sub event_cabby_clickZone(Line, speaker, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "clickZone" ${who}
/return

|---------------------------------------------------

#event cabby_sendTell "<#1#> sendtell #2# #3#"
#event cabby_sendTell "<#1#> #4# sendtell #2# #3#"
#event cabby_sendTell "[#1#(msg)] sendtell #2# #3#"

Sub event_cabby_sendTell(Line, speaker, to, message, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "sendTell ${to} ${message}" ${who}
/return

|---------------------------------------------------

#event cabby_engage "<#1#> engage #2#"
#event cabby_engage "<#1#> #3# engage #2#"
#event cabby_engage "[#1#(msg)] engage #2#"

Sub event_cabby_engage(Line, speaker, targetId, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "engage ${targetId}" ${who}
/return

|---------------------------------------------------

#event cabby_refresh "<#1#> refresh"
#event cabby_refresh "<#1#> #2# refresh"
#event cabby_refresh "[#1#(msg)] refresh"

Sub event_cabby_refresh(Line, speaker, who)
    /call scheduleCommand ${speaker} ${priorityImmediate} "refresh" ${who}
/return

|###################################################
|###############   COMMANDS   ######################
|###################################################

#bind chelp /chelp

Sub Bind_chelp(Line)
    /if (${Line.Length} < 1 || ${Line.Arg[1].Equal[help]}) {
        /call chelpHelp
    } else /if (${Line.Equal[commands]}) {
        /echo Command list:
        /echo /addowner
        /echo /ccfg
        /echo /chelp
        /echo /clonecfg
        /echo /debug
        /echo /relaytellsto
        /echo /removeowner
        /echo /showowners
        /echo /showqueue
    } else /if (${Line.Equal[bccommands]}) {
        /echo Leverage these using EQBC's /bc(*) commands
        /echo Example: /bca /clickzone
        /echo EQBC commands:
        /echo clickzone
        /echo do <command>
        /echo engage
        /echo followme
        /echo m2m
        /echo sendtell <tellTo> <message>
        /echo stopfollow
    } else {
        /call chelpHelp
    }
/return

Sub chelpHelp
    /echo (/chelp) Cabby Help Menu
    /echo Pick a help topic. Options: [commands,bccommands]
/return

|---------------------------------------------------

#bind debug /debug

Sub Bind_debug(Line)
    /if (${Line.Length} < 1 || ${Line.Arg[1].Equal[help]}) {
        /call debugHelp
    } else /if (${Line.Arg[1].Equal[All]}) {
        /if (${DebugAll}) {
            /echo Debug All (OFF)
            /varset DebugAll FALSE
        } else {
            /echo Debug All (ON)
            /varset DebugAll TRUE
        }
    } else /if (${Line.Arg[1].Equal[Setup]}) {
        /if (${DebugSetup}) {
            /echo Debug Setup (OFF)
            /varset DebugSetup FALSE
        } else {
            /echo Debug Setup (ON)
            /varset DebugSetup TRUE
        }
    } else /if (${Line.Arg[1].Equal[PriorityQueue]}) {
        /if (${DebugPriorityQueue}) {
            /echo Debug PriorityQueue (OFF)
            /varset DebugPriorityQueue TRUE
        } else {
            /echo Debug PriorityQueue (ON)
            /varset DebugPriorityQueue TRUE
        }
    } else /if (${Line.Arg[1].Equal[Set]}) {
        /if (${DebugSet}) {
            /echo Debug DebugSet (OFF)
            /varset DebugSet TRUE
        } else {
            /echo Debug DebugSet (ON)
            /varset DebugSet TRUE
        }
    } else {
        /call debugHelp
    }
/return

Sub debugHelp
    /echo (/debug) Toggles debug outputs for different sections
    /echo Debug toggles:
    /echo All
    /echo PriorityQueue
    /echo Set
    /echo Setup
/return

|---------------------------------------------------

#bind addOwner /addowner

Sub Bind_addOwner(Line)
    /if (${Line.Length} < 1 || ${Line.Arg[1].Equal[help]}) {
        /echo (/addowner) Adds (name) as this char's owner
        /echo Usage: /addOwner <name>
    } else {
        /call addOwner ${Line.Arg[1]}
    }
/return

|---------------------------------------------------

#bind removeOwner /removeowner

Sub Bind_removeOwner(Line)
    /if (${Line.Length} < 1 || ${Line.Arg[1].Equal[help]}) {
        /echo (/removeowner) Removes (name) as this char's owner
        /echo Usage: /removeOwner <name>
    } else {
        /call removeOwner ${Line.Arg[1]}
    }
/return

|---------------------------------------------------

#bind showOwners /showowners

Sub Bind_showOwners(Line)
    /call printOwners
/return

|---------------------------------------------------

#bind relayTellsTo /relaytellsto

Sub Bind_relayTellsTo(Line)
    /if (${Line.Length} < 1 || ${Line.Arg[1].Equal[help]}) {
        /echo (/relaytellsto) Sends any tells sent to this character to (name)
        /echo Usage: /relaytellsto <name>
        /echo Disable: /relaytellsto off
    } else /if (${Line.Arg[1].Equal[off]}) {
        /call setRelayTellsTo NULL
    } else {
        /call setRelayTellsTo ${Line.Arg[1]}
    }
/return

|---------------------------------------------------

#bind ccfg /ccfg

Sub Bind_ccfg(section, key)
    /declare result string local ${Ini[${configFilePath}]}
    /declare searchCount int local ${Math.Calc[${result.Count[|]} - 1]}
    /declare search string local
    /declare cfgecho string local
    /declare resultCount int local 0
    /declare results[20] string local
    /declare i int local

    | Read Section Info
    /if (${section.Length} < 1 || ${section.Equal[help]}) {
        /echo (/ccfg) Shows config values
        /echo -- Usage: /ccfg <optional section name/#>,<optional key name/#/ALL>
        /echo -- Examples: /ccfg ... /ccfg 1 ... /ccfg 1 2 
        /echo -- Examples: /ccfg General Version
        /echo Use ALL as param to list all keys and values
        /echo -- Examples: /ccfg All
        /echo -- Examples: /ccfg Owners All

        | Split up results into array
        /varcalc searchCount ${result.Count[|]} - 1
        /for i 1 to ${searchCount}
            /varcalc resultCount ${resultCount} + 1
            /varset results[${resultCount}] ${result.Arg[${i},|]}
        /next i
    } else /if (${section.Equal[All]}) {
        | Use recursion to deal with "All" as section input
        /for i 1 to ${searchCount}
            /call Bind_ccfg ${i} All
        /next i
        /return
    } else {
        | Look up keys for a specific section
        /varset cfgecho ${section}
        /if (${Int[${section}]}) {
            /varset search ${result.Arg[${Int[${section}]},|]}
        } else {
            /varset search ${section}
        }
        /varset result ${Ini[${configFilePath},${search}]}

        | Read Key Info
        | Find the value for the key if supplied
        /if (${key.Length} > 0 || ${Int[${key}]}) {
            /varset cfgecho ${cfgecho},${key}

            | Print all values
            /if (${key.Equal[All]}) {
                /declare keys string local ${result}
                /declare keyCount int local ${Math.Calc[${result.Count[|]} - 1]}
                /declare baseSearch string local ${search}
                /varset result

                | Read each key
                /declare thisKey string local
                /declare thisResult string local
                /for i 1 to ${keyCount}
                    /varset thisKey ${keys.Arg[${i},|]}
                    /varset search ${baseSearch},${thisKey}
                    /varset result ${Ini[${configFilePath},${search}]}
                    /varcalc resultCount ${resultCount} + 1
                    /varset results[${resultCount}] ${thisKey}: ${result}
                /next i
                /varset search ${baseSearch},${key}
            } else {
                /if (${Int[${key}]}) {
                    /varset search ${search},${result.Arg[${Int[${key}]},|]}
                } else {
                    /varset search ${search},${key}
                }
                /varset result ${key}: ${Ini[${configFilePath},${search}]}

                /varcalc searchCount ${result.Count[|]} - 1
                /for i 1 to ${searchCount}
                    /varcalc resultCount ${resultCount} + 1
                    /varset results[${resultCount}] ${result.Arg[${i},|]}
                /next i
            }
        }
    }

    /echo cfg (${cfgecho}): ${search}
    /for i 1 to ${resultCount}
        /echo -- [${i}]: ${results[${i}]}
    /next i
/return

|---------------------------------------------------

#bind showqueue /showqueue

Sub Bind_showqueue(Line)
    /call printQueue
/return

|---------------------------------------------------

#bind copycfg /copycfg

Sub Bind_copycfg(name, section)
    /if (${name.Length} < 1 || ${name.Equal[help]} || ${section.Length} < 1) {
        /echo (/copycfg) Copies configuration from another character to this character
        /echo -- Usage: /copycfg <name> <section> <optional - key>
        /echo -- Available sections include "All", or any config section found using `/ccfg`
        /echo -- Example: /copycfg MyOtherCharName All
        /echo -- Example: /copycfg MyOtherCharName General
        /echo -- Example: /copycfg MyOtherCharName General Version
    } else {
        /echo name(${name}) section(${section})
        /call copyConfig ${name} "${section}"
    }
/return

|###################################################
|#################   EVENTS   ######################
|###################################################

#event cabby_invited "#1# invites you to join a group."

Sub event_cabby_invited(Line, speaker)
    /call scheduleCommand ${speaker} ${priorityImmediate} "invited"
/return

|---------------------------------------------------

#event cabby_tellToMe "#1# tells you, #2#"

Sub event_cabby_tellToMe(Line, speaker, message)
    /if (${configRelayTellsTo.Length} > 0 && ${configRelayTellsTo.NotEqual[NULL]} && ${SpawnCount[npc ${speaker}]} < 1) /call insertPriorityQueue ${priorityImmediate} "tellToMe ${speaker} told me: ${message}"
/return

|---------------------------------------------------

#event cabby_tellNotOnline "#1# is not online at this time."

Sub event_cabby_tellNotOnline(Line, to)
    /if (${configRelayTellsTo.Length} > 0) /call insertPriorityQueue ${priorityImmediate} "tellNotOnline ${to}"
/return

|---------------------------------------------------

#event cabby_groupChanged "#1# has joined the group."
#event cabby_groupChanged "#1# has left the group."

Sub event_cabby_groupChanged(Line, newMember)
    /call insertPriorityQueue ${priorityImmediate} "groupChanged"
/return TRUE

|---------------------------------------------------

#event cabby_newMainTank "#1# is now Main Tank"

Sub event_cabby_newMainTank(Line, newMainTank)
    /call insertPriorityQueue ${priorityImmediate} "newMainTank ${newMainTank}"
/return

|---------------------------------------------------

#event cabby_newMainAssist "#1# is now Main Assist"

Sub event_cabby_newMainAssist(Line, newMainAssist)
    /call insertPriorityQueue ${priorityImmediate} "newMainAssist ${newMainAssist}"
/return

|---------------------------------------------------

#event cabby_newPuller "#1# is now Puller"

Sub event_cabby_newPuller(Line, newPuller)
    /call insertPriorityQueue ${priorityImmediate} "newPuller ${newPuller}"
/return

|---------------------------------------------------

#event cabby_removedGroup "You have been removed from the group."

Sub event_cabby_removedGroup(Line)
    /call insertPriorityQueue ${priorityImmediate} "removedGroup"
/return

|---------------------------------------------------
