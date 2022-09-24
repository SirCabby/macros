#warning

sub SortArray(String unsortedArrayName, String compareSubName)
	/declare i int local
	/declare j int local
	/declare temp string local
	
	/for i 1 to ${Math.Calc[${${unsortedArrayName}.Size} - 1]}
		/for j ${Math.Calc[${i} + 1]} to ${${unsortedArrayName}.Size}
			/if (${${compareSubName}[${${unsortedArrayName}[${i}]}, ${${unsortedArrayName}[${j}]}]}) {
				/varset temp ${${unsortedArrayName}[${i}]}
				/varset ${unsortedArrayName}[${i}] ${${unsortedArrayName}[${j}]}
				/varset ${unsortedArrayName}[${j}] ${temp}
			}
		/next j
	/next i
/return

sub AlphabeticalStringComparer(String a, string b)
	/varset a ${TrimStringQuotes[${a}]}
	/varset b ${TrimStringQuotes[${b}]}
		
	/if (${b.Length} == 0) /return FALSE
	/if (${a.Length} == 0) /return TRUE
/return (${a.Compare[${b}]} > 0)

sub TrimStringQuotes(String str)
	/if (${str.Left[1].Compare[a]} < 0) /varset str ${str.Right[${Math.Calc[${str.Length} - 1]}]}
	/if (${str.Right[1].Compare[a]} < 0) /varset str ${str.Left[${Math.Calc[${str.Length} - 1]}]}
/return ${str}

sub PrintArray(String arrayName)
	/declare index int local
	
	/for index 1 to ${${arrayName}.Size}
		/echo ${${arrayName}[${index}]}
	/next index
/return
