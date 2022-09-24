#include cabby_inc\set.inc

| FIFO Queue with priority modification and SQS-style visibility timers
| Entries are sorted by priority level (lower is more priority, closer out)
| Each entry contains a priority level and a string value to remove
| If a new entry enters the queue, it is placed last within the priority level
| Example priority in the queue: 1, 1, 3, 4, 4, 5, etc...
| Note the defined definition for an empty value at top of file
| JobKeys are unique identifiers (ints) that are used to find a specific job in the queue
| Timers cannot be in arrays, so they have a predictable naming pattern of pqTimer${key}
|   - these are used to mark jobs as invisible if they have a cooldown for processing
Sub initPriorityQueue
    /if (!${Defined[maxPriorityQueueSize]}) {
		/declare maxPriorityQueueSize    				int 	outer 	200
	}
    /declare priorityQueueSize                          int     outer   0
    /declare priorityQueue[${maxPriorityQueueSize}]     int	    outer   0
    /declare jobQueue[${maxPriorityQueueSize}]          string	outer
    /declare jobKey[${maxPriorityQueueSize}]            int     outer   0

    | Current Job State
    /declare currentJobKey                              int     outer

    | Debug
    /if (!${Defined[DebugAll]}) {
		/declare DebugAll  				                bool 	outer 	FALSE
	}
    /if (!${Defined[DebugPriorityQueue]}) {
		/declare DebugPriorityQueue  				    bool 	outer 	FALSE
	}
    /declare debugHighestPriorityCount                  int     outer   0       | Keeps track of the highest the queue size has ever grown to
/return

| Sets the next visible job key for execution in ${currentJobKey}
Sub setNextPriorityQueue
    /if (${priorityQueueSize} > 0) {
        /declare i int local
        /for i 1 to ${priorityQueueSize}
            /call isPriorityQueueKeyVisible ${jobKey[${i}]}
            /if (${Macro.Return}) {
                /varset currentJobKey ${jobKey[${i}]}
                /return
            }
        /next i
    }
    /varset currentJobKey -1
/return

| Returns the index of the job key
Sub getPriorityQueueKeyIndex(string key)
    /if (${priorityQueueSize} < 1) /return -1

    /declare i int local
    /for i 1 to ${priorityQueueSize}
        /if (${jobKey[${i}]} == ${key}) /return ${i}
    /next i
/return -1

| Creates a unique key to be assigned to a specific task
Sub generatePriorityQueueKey
    /declare i int local
    /for i 1 to ${maxPriorityQueueSize}
        /call setContains jobKey ${i}
        /if (!${Macro.Return}) /return ${i}
    /next i
    /echo Unable to find a unique key! Priority Queue might be full
/return -1

Sub getPriorityQueueKeyTimerName(string key)
/return pqTimer${key}

| Keys that have active timers are not yet visible and will be skipped until timer runs out
Sub isPriorityQueueKeyVisible(string key)
    /call getPriorityQueueKeyTimerName ${key}
    /declare result bool local FALSE
    /if (${${Macro.Return}.Value} == 0) /varset result TRUE
/return ${result}

| Reset the timer for the current key
Sub resetPriorityQueueTimer
    /call getPriorityQueueKeyTimerName ${currentJobKey}
    /varset ${Macro.Return} ${${Macro.Return}.OriginalValue}
/return

| Returns TRUE if new job takes higher priority
| onlyUnique - do not insert if job of same type already exists
Sub insertPriorityQueue(int priority, string job, string timerAmount, bool onlyUnique)
    /declare indexToInsert int local
    /declare indexToShift int local

    | Do not insert if job of same type already exists
    /if (${onlyUnique}) {
        /call setContains jobQueue ${job}
        /if (${Macro.Return}) /return FALSE
    }

    | Determine where to insert
    /declare i int local
    /for i 1 to ${maxPriorityQueueSize}
        /if (${priorityQueue[${i}]} == 0 || ${priorityQueue[${i}]} > ${priority}) {
            /varset indexToInsert ${i}
            /break
        }
    /next i

    | Debug
    /if (${DebugAll} || ${DebugPriorityQueue}) /echo Inserting Priority(${priority}) job at Index(${indexToInsert}): ${job}

    | Move all lower priority jobs back 1 slot in the queue
    /for indexToShift ${priorityQueueSize} downto ${indexToInsert}
        /varset priorityQueue[${Math.Calc[${indexToShift} + 1]}] ${priorityQueue[${indexToShift}]}
        /varset jobQueue[${Math.Calc[${indexToShift} + 1]}] ${jobQueue[${indexToShift}]}
        /varset jobKey[${Math.Calc[${indexToShift} + 1]}] ${jobKey[${indexToShift}]}
    /next indexToShift

    | Insert new job
    /varset priorityQueue[${indexToInsert}] ${priority}
    /varset jobQueue[${indexToInsert}] ${job}
    /varcalc priorityQueueSize ${priorityQueueSize} + 1
    /call generatePriorityQueueKey
    /varset jobKey[${indexToInsert}] ${Macro.Return}
    /call getPriorityQueueKeyTimerName ${Macro.Return}
    /declare timerName string local ${Macro.Return}
    /if (!${Defined[${timerName}]}) {
        /declare ${timerName} timer outer
    }
    /varset ${timerName} ${timerAmount}

    | Debug
    /if (${DebugAll} || ${DebugPriorityQueue}) {
        /if (${priorityQueueSize} > ${debugHighestPriorityCount}) /varset debugHighestPriorityCount ${priorityQueueSize}
    }

    | Is new entry of higher priority?
    /call getPriorityQueueKeyIndex ${currentJobKey}
    /declare result bool local
    /varset result ${indexToInsert} < ${Macro.Return}
/return ${result}

| Removes the item in the queue at index
Sub removePriorityQueue(int index)
    /if (${index} > ${priorityQueueSize}) {
        /echo Cannot remove index(${index}) from priority queue as it is empty. Current queue size: ${priorityQueueSize}
        /return
    }

    /if (${DebugAll} || ${DebugPriorityQueue}) /echo Removing Priority(${priorityQueue[${index}]}) job with key(${jobKey[${index}]}) at Index(${index}): ${jobQueue[${index}]}

    /declare i int local
    /for i ${index} to ${priorityQueueSize}
        /varset priorityQueue[${i}] ${priorityQueue[${Math.Calc[${i} + 1]}]}
        /varset jobQueue[${i}] ${jobQueue[${Math.Calc[${i} + 1]}]}
        /varset jobKey[${i}] ${jobKey[${Math.Calc[${i} + 1]}]}
    /next i
    /if (${priorityQueueSize} > 0) /varcalc priorityQueueSize ${priorityQueueSize} - 1
/return

Sub completePriorityQueueByJobKey(string jobKeyToComplete)
    /call getPriorityQueueKeyIndex ${jobKeyToComplete}
    /if (${Macro.Return} > 0) /call removePriorityQueue ${Macro.Return}
/return

| Removes the current job
Sub completePriorityQueue
    /call completePriorityQueueByJobKey ${currentJobKey}
/return

Sub printQueue
    /echo Priority Queue:
    /echo ===============
    /if (${priorityQueueSize} > 0) {
        /declare index int local
        /declare pqTimerName string local
        /for index 1 to ${priorityQueueSize}
            /call getPriorityQueueKeyTimerName ${jobKey[${index}]}
            /varset pqTimerName ${Macro.Return}
            /echo Index(${index}) Priority(${priorityQueue[${index}]}) Key(${jobKey[${index}]}) Timer(${${pqTimerName}}) Job: ${jobQueue[${index}]}
        /next index
    }
    /echo ===============
/return
