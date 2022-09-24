#warning

Sub BuyAA(String name)
	/if (${AltAbility[${name}].CanTrain}) {
		/echo Buying AA: ${name}
		| Me.AltAbility will return NULL if at rank 0, which requires purchase via .Index
		/if (${Me.AltAbility[${name}].CanTrain}) {
			/alt buy ${AltAbility[${name}].NextIndex}
		} else {
			/alt buy ${AltAbility[${name}].Index}
		}
	}
/return
