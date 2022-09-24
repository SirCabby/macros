
Sub setContains(string set, value)
    /declare i int local
    /for i 1 to ${${set}.Size}
        /if (${Int[${value}]}) {
            /if (${${set}[${i}]} == ${value}) /return TRUE
        } else /if (${${set}[${i}].Equal[${value}]}) {
            /return TRUE
        }
    /next i
/return FALSE

Sub setPrintFull(string set)
    /declare result string local FullArray (${set}): [
    /declare i int local
    /for i 1 to ${${set}.Size}
        /if (${i} == 1) {
            /varset result ${result}${${set}[${i}]}
        } else {
            /varset result ${result}, ${${set}[${i}]}
        }
    /next i
    /varset result ${result}]
    /echo ${result}
/return

|##################################################
|################# INT ############################
|##################################################

Sub setIntAdd(string set, int value)
    /if (${Debug} || ${DebugSet}) /echo Adding ${value}
    /call setContains ${set} ${value}
    /if (${Macro.Return} == TRUE) /return
    /declare i int local
    /for i 1 to ${${set}.Size}
        /if (!${${set}[${i}]}) {
            /varset ${set}[${i}] ${value}
            /return
        }
    /next i
    /echo Set (${set}) is full, cannot add ${value}
/return

Sub setIntRemove(string set, int value)
    /if (${Debug} || ${DebugSet}) /echo Removing ${value}
    /call setContains ${set} ${value}
    /if (${Macro.Return} == FALSE) /return
    /declare i int local
    /for i 1 to ${${set}.Size}
        /if (${${set}[${i}]} == ${value}) {
            /varset ${set}[${i}]
            /return
        }
    /next i
/return

Sub setIntPrint(string set)
    /declare result string local Set (${set}): [
    /declare i int local
    /declare first bool local TRUE
    /for i 1 to ${${set}.Size}
        /if (${first}) {
            /if (${${set}[${i}]}) {
                /varset result ${result}${${set}[${i}]}
                /varset first FALSE
            }
        } else {
            /if (${${set}[${i}]}) /varset result ${result}, ${${set}[${i}]}
        }
    /next i
    /varset result ${result}]
    /echo ${result}
/return

|##################################################
|################# STRING #########################
|##################################################

Sub setStringAdd(string set, string value)
    /if (${Debug} || ${DebugSet}) /echo Adding ${value}
    /call setContains ${set} ${value}
    /if (${Macro.Return} == TRUE) /return
    /declare i int local
    /for i 1 to ${${set}.Size}
        /if (${${set}[${i}].Length} < 1) {
            /varset ${set}[${i}] ${value}
            /return
        }
    /next i
    /echo Set (${set}) is full, cannot add ${value}
/return

Sub setStringRemove(string set, string value)
    /if (${Debug} || ${DebugSet}) /echo Removing ${value}
    /call setContains ${set} ${value}
    /if (${Macro.Return} == FALSE) /return
    /declare i int local
    /for i 1 to ${${set}.Size}
        /if (${${set}[${i}].Equal[${value}]}) {
            /varset ${set}[${i}]
            /return
        }
    /next i
/return

Sub setStringPrint(string set)
    /declare result string local Set (${set}): [
    /declare i int local
    /declare first bool local TRUE
    /for i 1 to ${${set}.Size}
        /if (${first}) {
            /if (${${set}[${i}].Length} > 0) {
                /varset result ${result}${${set}[${i}]}
                /varset first FALSE
            }
        } else {
            /if (${${set}[${i}].Length} > 0) /varset result ${result}, ${${set}[${i}]}
        }
    /next i
    /varset result ${result}]
    /echo ${result}
/return
