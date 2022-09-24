#warning

|Sub main
|	/declare RaidPulseTimer timer outer
|	/declare MobID int outer 0
|   :loop
|	/call RaidPulse
|	/delay 1s
|	/goto :loop
|/return

Sub RaidPulse
	/varset RaidPulseTimer 1s
	/if ( ${Zone.ID}==729 && ${Task.Title.Equal[Two Sides of the Stone]} ) /call VOX
/return
