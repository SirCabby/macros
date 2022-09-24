#include cabby_inc\config.inc
#include cabby_inc\commands.inc
#include cabby_inc\owners.inc
#include cabby_inc\priorityQueue.inc
#include cabby_inc\classes\cabbyMonk.inc
#include cabby_inc\states\combatState.inc
#include cabby_inc\states\followingState.inc
#include cabby_inc\states\groupState.inc

Sub cabbySetup
    /if (${DebugAll} || ${DebugSetup}) /echo Starting Cabby Setup...
    /call pluginSetup
    /call initConfig ${configFilePath}
    /call initOwners ${configFilePath}
    /call initPriorityQueue
    /call priorityQueueSetup
    /call classActionsSetup
    /if (${DebugAll} || ${DebugSetup}) /echo Finished Cabby Setup
/return

Sub pluginSetup
    | MQ2EQBC specific setup
    /call checkPlugin MQ2EQBC
    /if (!${EQBC.Connected}) {
        /bccmd connect
        /delay 5s ${EQBC.Connected}
        /if (!${EQBC.Connected}) {
            /echo Could not connect to EQBCS. Aborting...
            /end
        }
    }
    /if (${EQBC.Setting[localecho]}) /bccmd set localecho off
    
    /call checkPlugin MQ2MoveUtils
    /call checkPlugin MQ2AdvPath
    /call checkPlugin MQ2Rez
    /call checkPlugin MQ2Twist
    /call checkPlugin MQ2Melee
    /call checkPlugin MQ2Cast
/return

Sub checkPlugin(string pluginName)
    /if (!${Plugin[${pluginName}].Name.Length}) {
		/echo Plugin ${pluginName} is not loaded, attempting to resolve...
		/plugin ${pluginName}
		/delay 10s ${Plugin[${pluginName}].Name.Length}
		/if (!${Plugin[${pluginName}].Name.Length}) {
			/echo This script requires ${pluginName}, aborting...
            /end
		}
	}
/return

Sub priorityQueueSetup
    /declare currentJobCommand  string  outer
    /declare currentJobArgs     string  outer
    /declare currentJobString   string  outer
/return

Sub classActionsSetup
    /call initGeneral
    /if (${Me.Class.ShortName.Equal[MNK]}) {
        /call initMonk
    }
/return
