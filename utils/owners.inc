
| Path to owners file
| ownersFilePath - requires escaped path to file
|       - Example:
|          - Actual Path:   folder\owners.ini
|          - Required:      folder\\owners.ini
| ini file structure:
|   [Owners]
|   Owner1=name1
|   Owner2=name2
Sub initOwners(string ownersFp)
    /declare ownersFilePath string outer ${ownersFp}
    /declare ownersString string local ${Ini[${ownersFilePath},Owners]}

    /if (!${Defined[maxOwnerSize]}) {
		/declare maxOwnerSize    				int 	outer 	100
	}
    /declare owners[${maxOwnerSize}]            string  outer
    /declare ownersSize                         int     outer   0

    /declare ownersCount int local ${Math.Calc[${ownersString.Count[|]} - 1]} 
    /if (${ownersCount} < 1) {
        /varset ownersCount 0
        /echo No owners defined, will not be able to receive commands
        /echo Add owners with /addowner <name>
        /return
    }

    /declare i int local
    /for i 1 to ${ownersCount}
        /varset owners[${i}] ${Ini[${ownersFilePath},Owners,${ownersString.Arg[${i},|]}]}
        /varcalc ownersSize ${ownersSize} + 1
    /next i

    /if (${owners[1].Equal[NULL]}) {
        /echo No owners defined, will not be able to receive commands
        /return
    }
    
    /call printOwners
/return

Sub addOwner(string newOwner)
    /call isOwner ${newOwner}
    /if (${Macro.Return}) {
        /echo (${newOwner}) is already an owner
        /return
    }

    /varcalc ownersSize ${ownersSize} + 1
    /ini ${ownersFilePath} Owners Owner${ownersSize} ${newOwner}
    
    /varset owners[${ownersSize}] ${newOwner}
    /echo Added new Owner (${newOwner})
/return

Sub removeOwner(string removeOwner)
    /declare tempOwners[${maxOwnerSize}] string local
    /declare i int local
    /declare j int local 1
    /declare foundCount int local 0

    | Copy owners out except the one to remove
    /for i 1 to ${ownersSize}
        /if (${owners[${i}].NotEqual[${removeOwner}]}) {
            /varset tempOwners[${j}] ${owners[${i}]}
            /varcalc j ${j} + 1
        } else {
            /varcalc foundCount ${foundCount} + 1
        }
    /next i

    | If didn't find owner, don't modify anything
    /if (!${foundCount}) {
        /echo Unable to find owner to remove (${removeOwner})
        /return
    }

    | Remove the last owner definition from the file and variable
    /for i 1 to ${foundCount}
        /ini ${ownersFilePath} Owners Owner${ownersSize} NULL
        /varset owners[${ownersSize}]
        /varcalc ownersSize ${ownersSize} - 1
    /next i

    | Re-add the owners back to the file and variable
    /for i 1 to ${ownersSize}
        /ini ${ownersFilePath} Owners Owner{$ownersSize} ${tempOwners{i}}
        /varset owners[${i}] ${tempOwners[${i}]}
    /next i

    /echo Removed Owner (${removeOwner})
/return

Sub isOwner(string speaker)
    /declare i int local
    /if (${owners.Size} > 0) {
        /for i 1 to ${owners.Size}
            /if (${owners[${i}].Equal[${speaker}]}) /return TRUE
        /next i
    }
/return FALSE

Sub printOwners
    /declare result string local My Owners: [
    /if (${owners.Size} > 0) {
        /declare i int local
        /for i 1 to ${owners.Size}
            /if (${i} == 1) {
                /varset result ${result}${owners[${i}]}
            } else {
                /if (${owners[${i}].NotEqual[NULL]} && ${owners[${i}].Length} > 0) {
                    /varset result ${result}, ${owners[${i}]}
                }
            }
        /next i
    }
    /varset result ${result}]
    /echo ${result}
/return
