Sub ResetHudArray
	/if (!${Defined[index]}) {
		/declare index int local
	}
	/if (${Defined[HudArray]}) {
		/for index 1 to ${HudArray.Size}
			/varset HudArray[${index}] HudArray
		/next index
	}
/return
