
Sub initFollowState
    /declare followStatePaused  bool    outer FALSE
    /declare followTargetName   string  outer
    /declare followCheckJobKey  int     outer 0
    /declare followCheckX       int     outer 0
    /declare followCheckY       int     outer 0
    /declare followCheckZ       int     outer 0
    /declare followCheckZone    int     outer ${Zone.ID}
/return

Sub followState_updateMyLoc
    /varset followCheckX ${Me.X}
    /varset followCheckY ${Me.Y}
    /varset followCheckZ ${Me.Z}
    /varset followCheckZone ${Zone.ID}
/return

Sub followState_Pause
    /if (!${followStatePaused}) {
        /varset followStatePaused TRUE
        /afollow off
    }
/return

Sub followState_Resume
    /varset followStatePaused FALSE
/return

Sub followState_findFollowTarget
    /declare retryTimer timer local 600
    :waitForFollowTarget
        | TODO need to evaluate priorities here
        /delay 5
        /if (!${Bool[${Spawn[pc ${followTargetName}].ID}]} && ${retryTimer}) /goto :waitForFollowTarget
/return

|################# CHECKS ############################

| Am I failing to follow?
Sub cabby_followState_check
    /if (${followStatePaused}) /return FALSE
    /if (${followCheckJobKey} == 0) /varset followCheckJobKey ${currentJobKey}

    | Resume following if some other command had previously paused it
    /if (!${AdvPath.Following}) {
        /call followState_findFollowTarget
        | If I cannot resume follow, kill the follow check
        /call cabby_followMe ${followTargetName} TRUE
        /if (!${AdvPath.Following}) {
            /varset followCheckJobKey 0 | stop this next call from double-killing this task
            /call cabby_stopFollow
            /return TRUE
        }
    }

    | Am I stuck? Am I far away from follow target and my loc isn't changing much?
    /if (${Spawn[pc ${followTargetName}].Distance3D} > 200 && ${Math.Distance[${followCheckY},${followCheckX},${followCheckZ}]} < 100) {
        /bc I appear to be stuck, aborting follow
        /varset followCheckJobKey 0 | stop this next call from double-killing this task
        /call cabby_stopFollow
        /return TRUE
    }

    | Things are fine, update loc
    /call followState_updateMyLoc 
/return FALSE

|################# COMMANDS ##########################

Sub cabby_followMe(string followTarget, bool skipInsert)
    /target ${followTarget} radius 200
    /delay 1s ${Target.Name.Equal[${followTarget}]}
    /if (${Target.Name.Equal[${followTarget}]}) {
        /afollow on
        /if (!${skipInsert}) /call insertPriorityQueue ${priorityFollowing} "followState_check" 50 TRUE
        /varset followTargetName ${followTarget}
    } else {
        /bc Unable to find ${followTarget} to follow
    }
/return TRUE

Sub cabby_stopFollow
    /afollow off
    /varset followStatePaused FALSE
    /varset followTargetName
    /varset followCheckX 0
    /varset followCheckY 0
    /varset followCheckZ 0
    /varset followCheckZone 0

    /if (${followCheckJobKey} != 0) /call completePriorityQueueByJobKey ${followCheckJobKey}
    /varset followCheckJobKey 0
/return TRUE

Sub cabby_move2me(string moveTarget)
    /call cabby_stopFollow
    /target ${moveTarget} radius 200
    /delay 1s ${Target.Name.Equal[${moveTarget}]}
    /if (${Target.Name.Equal[${moveTarget}]}) {
        /squelch /moveto ID ${Target.ID}
    } else {
        /bc Unable to find ${moveTarget} to move to
    }
/return TRUE

Sub cabby_clickZone
    /call followState_Pause
    /doortarget
    /if (${DoorTarget.Distance} < 100) {
        /squelch /moveto loc ${DoorTarget.Y} ${DoorTarget.X} dist 10
        /declare retryTimer timer local 100
        :retryClickDoor
            | TODO need to evaluate priorities here
            /squelch /click left door
            /delay 3s
            /if (${followCheckZone} == ${Zone.ID} && ${retryTimer}) /goto :retryClickDoor
        /if (${followCheckZone} == ${Zone.ID}) {
            /bc Failed to click into zone, aborting follow...
            /call cabby_stopFollow
        } else {
            /call followState_findFollowTarget
            /if (!${Bool[${Spawn[pc ${followTargetName}].ID}]}) {
                /bc Failed to find following target after zoning for 1 minute, aborting follow...
                /call cabby_stopFollow
            } else {
                /call followState_Resume
            }
        }
    } else {
        /bc Click target was too far away for me
        /call cabby_stopFollow
    }
/return TRUE
