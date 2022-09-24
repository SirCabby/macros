
Sub initConfig(string filePath)
    /if (!${Defined[configFilePath]}) {
        /declare configFilePath string outer ${filePath}
    } 
    /varset configFilePath ${filePath}
    /call initConfigFile
    /call loadConfigFile
/return

Sub initConfigFile
    /if (!${Ini[${configFilePath}].Length}) {
        /echo Running first time setup...

        | General
        /ini ${configFilePath} General "Version" "0.1"
        /ini ${configFilePath} General "RelayTellsTo" "NULL"

        | Debug (Defaults)
        /ini ${configFilePath} Debug "All" FALSE
        /ini ${configFilePath} Debug "PriorityQueue" FALSE
        /ini ${configFilePath} Debug "Set" FALSE
        /ini ${configFilePath} Debug "Setup" FALSE

        | Owners - Create empty Section
        /ini ${configFilePath} Owners "Owner1" "NULL"
        /ini ${configFilePath} Owners "Owner1" NULL

        /echo First time setup completed
    }
/return

Sub loadConfigFile
    /if (${Defined[configVersion]}) {
        /varset configVersion ${Ini[${configFilePath},General,Version]}
    } else {
        /declare configVersion string outer ${Ini[${configFilePath},General,Version]}
    }

    /if (${Defined[configRelayTellsTo]}) {
        /varset configRelayTellsTo ${Ini[${configFilePath},General,RelayTellsTo]}
    } else {
        /declare configRelayTellsTo string outer ${Ini[${configFilePath},General,RelayTellsTo]}
    }

    /if (${Defined[DebugAll]}) {
        /varset DebugAll ${Ini[${configFilePath},Debug,All]}
    } else {
        /declare DebugAll bool outer ${Ini[${configFilePath},Debug,All]}
    }

    /if (${Defined[DebugSetup]}) {
        /varset DebugSetup ${Ini[${configFilePath},Debug,Setup]}
    } else {
        /declare DebugSetup bool outer ${Ini[${configFilePath},Debug,Setup]}
    }

    /if (${Defined[DebugPriorityQueue]}) {
        /varset DebugPriorityQueue ${Ini[${configFilePath},Debug,PriorityQueue]}
    } else {
        /declare DebugPriorityQueue bool outer ${Ini[${configFilePath},Debug,PriorityQueue]}
    }

    /if (${Defined[DebugSet]}) {
        /varset DebugSet ${Ini[${configFilePath},Debug,Set]}
    } else {
        /declare DebugSet bool outer ${Ini[${configFilePath},Debug,Set]}
    }
/return

Sub copyConfig(string name, string section, string key)
    /declare copyFromFilePath string local ${configFilePath.Replace[${Me.Name},${name}]}
    
    | Get config to copy
    /declare result string local ${Ini[${copyFromFilePath},${section}]}
    /if (${result.Equal[NULL]}) {
        /echo Could not find config for character(${name}) with section(${section})
        /return
    }

    | Copy ALL sections - recursive calls

    | Copy a specific section
    | Section not found escape

    | Copy a single key-value
    | key not found escape



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
            }}}}











    | Delete existing section
    /ini ${configFilePath} ${section} NULL NULL

    /endmacro
/return

Sub setRelayTellsTo(string tellto)
    /varset configRelayTellsTo ${tellto}
    /ini ${configFilePath} General "RelayTellsTo" "${tellto}"
    /if (${tellto.Equal[NULL]}) {
        /echo Disabled tell relay
    } else {
        /echo Relaying tells to (${tellto})
    }
/return
