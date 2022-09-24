| Ninjadvloot.inc v5.1 Beta 05/18/2015 Redguides.com 
| Author A_Druid_00 12/19/2005
| Based on original looting code of toomanynames
| The following updates were all individually posted and consolidated by Maskoi into the current version.
| Updated: Moeymoejoe added strip out comma in item names when accessing ini file 08/02/2006
| Updated: mystikule added sell option to ini file 01/07/2008
| Updated: drkrain for House of Thule 10/23/2010
| Updated: Maskoi with Sell Routines 11/27/2010
| Updated: Maskoi with /item keep/sell/destroy ini commands 07/21/2011
| Updated: Maskoi with Foraged item handling 08/31/2011
| Updated: Maskoi with Buying item handling 07/31/2012
| Updated: Maskoi with Assign different ini file for item handling 01/01/2013
| Updated: Crystane with Looting while on mount (no overshooting on corpserun), added some conditional checks to skip unecessary delay I found. Announcement routine reworked. 05/13/2014
| Updated: TreeHuginDruid hard coded 1 second delay between loot and corpse item retrieval to resolve loot latency issue.
| Updated: Maskoi added suppot for Advanced Looting system  05/18/2015
|
| Selling Contributions by
|   drkrain - TLInv item check
|   dewey2461 - Add sold items to ini file Event
|
| Requirements: MQ2Moveutils
| Usage: add the following to your macro for looting
|   #include Ninjadvloot.inc
|   /call SetupAdvLootVars
|   /call LootMobs
|
| Assign differnent ini files. Default is Loot.ini
|   /call SetupAdvLootVars "MyToonLoot.ini"
|   /call SetupAdvLootVars "LootMesa.ini"
|
| Commands
| Usage. Grab the item and put it on your cursor then use one of the following commands
|     /lootdestroy - Mark item on cursor to Destroy in Loot.ini file
|     /lootignore - Mark item on cursor to Ignore in Loot.ini file
|     /lootkeep - Mark item on cursor to Keep in Loot.ini file
|     /lootsell - Mark item on cursor to Sell in Loot.ini file
|     /sellstuff - Target a vendor and this command will sell any item in your inventory marked sell. 
|     /depositstuff - Target your guildbanker and this command will deposit any item in your inventory marked keep into the guild bank.

#warning

#Event CantLoot         "#*#may not loot this corpse#*#"
|#Event Sell             "#*#You receive#*# for the #1#(s)#*#"
#Event EditIniItem      "[MQ2] LootIniItem #1# #2#"
#Event SellStuff        "[MQ2] NinjadvLoot selling items to vendor"
#Event Broke            "#*#you cannot afford#*#" 
#Event Broke            "#*#you can't afford#*#" 
#Event Forage           "Your forage mastery has enabled you to find something else!"
#Event Forage           "You have scrounged up #*#"
#Event InventoryFull    "#*#Your inventory appears full!#*#"
#Event NullSlot         "#*#Invalid item slot 'null#*#"
#Event Lore             "#*#You cannot loot this Lore Item.#*#"
#Event MoneyReceived	"#*#You receive#1# from#*#"

Sub SetupAdvLootVars(string ChangeIniFile)

    /declare CantLootID                         int                 outer 
    /declare NinjadVersion                      string              outer   5.0
    /declare InventoryFull                      int                 outer   0
    /declare NinjaIniFile                       string              outer   Loot.ini
	/declare keys 								string 				outer 	${Ini[Loot.ini,Global]}
	/declare key								string				outer
	/declare keyIndex 							int 				outer
	/declare useGlobalCache						bool				outer	FALSE
	/declare lootOption							string				outer
	| -1 = undefined, 999999 = global
	/declare lootValue							int					outer	0
	/declare minValue							int					outer	25000
	/declare lootedValue						int					outer	0
	/declare platinum 							int 				outer 	0
	/declare gold 								int 				outer 	0
	/declare silver 							int 				outer 	0
	/declare copper 							int 				outer 	0
	/declare heldValue 							int 				outer 	0
	/declare spaceIndex 						int 				outer 	0
    /declare MasterCorpseList                   string              outer   
    /declare Debugloot                          int                 outer   0
    /declare GlobalCount                        int                 outer   5
	/declare overrideManualLoot					string				outer
	/declare _moneyReceived 					bool 				outer
    /if (${ChangeIniFile.Length}) {
        /varset NinjaIniFile ${ChangeIniFile}
        /echo Ninja Advance Loot in now writing all info to the ${ChangeIniFile} file.
    }
    /declare NALVersion                           string             outer   ${Ini[${NinjaIniFile},Settings,Version]}
    /if (!${Ini[${NinjaIniFile},Settings,Version].Length}) {
        /call CreateIni
        /ini "${NinjaIniFile}" "Settings" "Version" "0.0"
    }
    /if (${NALVersion.NotEqual[${NinjadVersion}]}) {
        /echo NINJADVLOOT: Version mismatch detected. Writing aliases.
        /noparse /squelch /alias /lootignore    /echo LootIniItem Ignore ${Cursor.Name}
        /noparse /squelch /alias /lootsell      /echo LootIniItem Sell ${Cursor.Name}
        /noparse /squelch /alias /lootkeep      /echo LootIniItem Keep ${Cursor.Name}
        /noparse /squelch /alias /lootdestroy   /echo LootIniItem Destroy ${Cursor.Name}
        /squelch /alias /sellstuff              /echo NinjadvLoot selling items to vendor
        /squelch /alias /depositstuff           /echo NinjadvLoot depositing items to guildbank
        /ini "${NinjaIniFile}" "Settings" "Version" "${NinjadVersion}"
    }

    | Loot ini settings
    /call NALLoadVar Settings AddNewSales        1                   int
    /call NALLoadVar Settings LootForage         1                   int
    /call NALLoadVar Settings LootMobs           TRUE                bool
    /call NALLoadVar Settings CorpseRadius       100                 int
    /call NALLoadVar Settings MobsTooClose       40                  int
    /call NALLoadVar Settings ReportLoot         FALSE               bool
    /call NALLoadVar Settings LootChannel        g                   string
    /call NALLoadVar Settings LootForageSpam     1                   int
    /call NALLoadVar Settings GlobalLootOn       1                   int
    /call NALLoadVar Settings LootForageSpam     1                   int
    /call NALLoadVar Settings CombatLooting      0                   int
/return

Sub OverrideManualLoot(string option)
	/if (${option.Length} > 0) /echo Overriding ManualLoot with: ${option}
	/varset overrideManualLoot ${option}
/return

Sub NALLoadVar(IniSection,IniVar,IniValue,VarType)
    /if (!${Defined[${IniVar}]} && ${Defined[VarType]}) /declare ${IniVar} ${VarType} outer
    /declare IniString string local ${Ini[${NinjaIniFile},${IniSection},${IniVar},NOTFOUND]}
    /varset ${IniVar} ${IniString}
    /if (${IniString.Equal["NOTFOUND"]}) {
        /if (${IniString.Equal["NOTFOUND"]}) /varset ${IniVar} ${IniValue}
        /ini "${NinjaIniFile}" "${IniSection}" "${IniVar}" "${${IniVar}}"
    } 
/return

| ****************  Loot Mobs Section ******************** |

Sub LootMobs
    /declare DeadCount int local ${SpawnCount[corpse radius ${CorpseRadius} zradius 50]}
    /if (${Debugloot}) /echo DEBUGLOOT There are ${DeadCount} corpses in range.
    /if (${Debugloot}) /echo DEBUGLOOT !${LootMobs} || ${SpawnCount[npc radius ${MobsTooClose} zradius 50]} || !${DeadCount} || ${Me.Combat} || !${Me.FreeInventory} || ${Me.CombatState.Equal[COMBAT]} && !${CombatLooting})
    /if (!${LootMobs} || ${SpawnCount[npc radius ${MobsTooClose} zradius 50]} || !${DeadCount} || ${Me.Combat} || !${Me.FreeInventory} || ${Me.CombatState.Equal[COMBAT]} && !${CombatLooting}) /return
    /declare i int local
    /declare CorpseList string local 
    /declare CorpseToLoot int local
    /declare ZDist float local ${Math.Distance[${Target.Z}:${Me.Z}]}
    /declare ClosestCorpse int local
    /declare CorpseCount int local 0
    /for i 1 to ${DeadCount}
        /varset ClosestCorpse ${NearestSpawn[${i},corpse radius ${CorpseRadius} zradius 50].ID}
        /if (!${Spawn[${ClosestCorpse}].Deity.ID}) {
            /varset CorpseList ${CorpseList}${ClosestCorpse}|
            /if (!${MasterCorpseList.Find[${ClosestCorpse}]}) /varset MasterCorpseList ${MasterCorpseList}${ClosestCorpse}|
            /varcalc CorpseCount ${CorpseCount}+1
        }
    /next i
    /if (${Debugloot}) /echo DEBUGLOOT Trying to loot ${CorpseCount} corpses. ID's ${CorpseList}
    /for i 1 to ${CorpseCount} 
		/varset CorpseToLoot ${CorpseList.Arg[${i},|]}
		/if (!${Defined[CorpseLocked${CorpseToLoot}]}) /declare CorpseLocked${CorpseToLoot} timer outer 0
		/if (${Spawn[${CorpseToLoot}].Deity.ID} || !${Spawn[${CorpseToLoot}].ID} || !${Spawn[${CorpseToLoot}].LineOfSight} || ${CorpseLocked${CorpseToLoot}} || ${Me.CombatState.Equal[COMBAT]} && !${CombatLooting}) /goto :SkipCorpse
		/if (${Target.ID} != ${CorpseToLoot}) /target id ${CorpseToLoot}
		/if (!${Me.Standing}) /stand
		/delay 3s ${Target.ID} == ${CorpseToLoot} && ${Me.Standing}
		/if (${Target.Distance} > 5) {
			/if (${Debugloot}) /echo DEBUGLOOT move to corpse
			/moveto mdist 10
			/moveto loc ${Target.Y} ${Target.X} ${Target.Z} 
			/if (${Me.Mount.ID}) {
				/delay 3s ${Math.Distance[${Target.Y},${Target.X}]}<25
				/keypress back
			}
			/delay 3s ${MoveTo.Stopped} && !${Me.Moving}
			/call ZCheck ${ZDist} 8
			/if (${Target.ID}) /face fast
		}
		/if (${Target.Distance} < 12) {
			/if (${Debugloot}) /echo DEBUGLOOT /Call LootCorpse
			/call LootCorpse ${Target.ID}
			/call RemoveCorpseID ${CorpseToLoot}
		}
        :SkipCorpse
    /next i
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub LootMobs 
/return

Sub LootCorpse(CorpseID)
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub LootCorpse
    /declare i              int    local
    /declare j              int    local
    /declare LootList       string local
    /declare FirstLetter    string local
    /declare GCheck            string local
    /call CheckCursor
    /varset CantLootID 0
    /if (${Debugloot}) /echo DEBUGLOOT /loot
    | Open Loot window
    /loot
    /if (${Debugloot}) /echo DEBUGLOOT /doevents CantLoot
    /delay 3s ${Window[LootWnd]}
    /delay 1s
    /doevents CantLoot
    /delay 3s ${CantLootID} || ${Window[LootWnd]}
    /if (${Target.ID}==${CantLootID} && !${Window[LootWnd]}) {
        /echo NINJADVLOOT: I can't loot ${Target.CleanName} ID:${CorpseID} right now.
        /if (${Defined[CorpseLocked${CorpseID}]}) /varset CorpseLocked${CorpseID} 150s
        /return
    }
    /if (${Debugloot}) /echo DEBUGLOOT Loot Window Open:${Window[LootWnd]} - # Loot Items:${Corpse.Items}
    /if (${Window[LootWnd].Open} && ${Corpse.Items}) {
        /declare lootTotal int  local
        :LootLag
        /varset lootTotal ${Corpse.Items}
             /if (${Debugloot}) /echo DEBUGLOOT :LootLag ${lootTotal}!=${Corpse.Items} LagCount${LagCount}
        /delay 1s ${lootTotal}!=${Corpse.Items}
        /if (${lootTotal}!=${Corpse.Items}) /goto :LootLag
        /echo NINJADVLOOT: Looting ${Target.CleanName} ID:${CorpseID}.
        /for i 1 to ${lootTotal}
            /varset FirstLetter ${Corpse.Item[${i}].Name.Left[1]}
			/varset lootOption ${Ini[${NinjaIniFile},"${FirstLetter}","${Corpse.Item[${i}]}"]}
            /if (${Corpse.Item[${i}].ID} && ${lootOption.Equal[NULL]}) {
				| Exact lookup failed, try Global partial match
				/call GlobalLookup "${Corpse.Item[${i}]}"
				/if (${lootOption.Length} <= 0 || ${lootOption.Equal[NULL]}) {
					| GlobalLookup failed. Found new item, tag as such
					/varset lootOption NewItem
					/echo NewItem: ${Corpse.Item[${i}].Name}
					/if (${Corpse.Item[${i}].NoDrop})  /ini "${NinjaIniFile}" "${FirstLetter}" "${Corpse.Item[${i}]}" NewItem
					/if (!${Corpse.Item[${i}].NoDrop}) /ini "${NinjaIniFile}" "${FirstLetter}" "${Corpse.Item[${i}]}" NewItem
				}
            }
            /if (${ReportLoot} && ${lootOption.NotEqual[Destroy]} && !${Me.FreeInventory} && (!${FindItemCount[=${Corpse.Item[${i}].Name}]} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && !${Corpse.Item[${i}].FreeStack})) || (${Corpse.Item[${i}].Lore} && ${FindItem[=${Corpse.Item[${i}]}].ID}) || ${lootOption.Equal[Ignore]}) /varset LootList ${LootList}${Corpse.Item[${i}]},
			/if (${lootOption.Equal[Ignore]} && !${Corpse.Item[${i}].NoDrop} && ${Corpse.Item[${i}].Value} >= ${minValue}) /echo You have ${Corpse.Item[${i}].Name} set to Ignore but is above your minimum value of ${minValue}
            /if ((${lootOption.Equal[Keep]} || ${lootOption.Equal[Sell]} || ${lootOption.Equal[NewItem]}) && (!${Corpse.Item[${i}].Lore} || !${FindItem[=${Corpse.Item[${i}]}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && ${Corpse.Item[${i}].FreeStack}))) {
				/if (${lootOption.Equal[Sell]}) {
					/if (${Corpse.Item[${i}].NoDrop}) {
						/echo You have ${Corpse.Item[${i}].Name} set to Sell but it is NoDrop
					} else {
						/varcalc lootedValue ${lootedValue}+${Corpse.Item[${i}].Value}
						/if (${Corpse.Item[${i}].Value} < ${minValue}) /echo You have ${Corpse.Item[${i}].Name} set to Sell but is under your minimum value of ${minValue}
					}
				}
				/call LootItem ${i} Keep Right
			}
            /if (${lootOption.Equal[Destroy]}) /call LootItem ${i} Destroy left
        /next i
        /if (${Corpse.Items}) {
            /if (${ReportLoot} && ${LootList.Length}) {
                /keypress /
                /delay 3
                /call Type "${LootChannel}"
                /keypress Space chat
                /notify LootWND LW_BroadcastButton leftmouseup 
                /delay 3
                /call Type " ,left on corpse."
                /keypress Enter chat
                /if (${Defined[CorpseLocked${CorpseID}]}) /varset CorpseLocked${CorpseID} 440s
            }
        }
    }
    :clickdoneffs
	/nomodkey /notify LootWnd LW_DoneButton leftmouseup
	/if (${Debugloot}) /echo DEBUGLOOT :clickdoneffs 
	/delay 5s !${Window[LootWnd].Open}
	/if (${Debugloot}) /echo DEBUGLOOT :clickdoneffs LootWin Open:${Window[LootWnd]}
    /if (${Window[LootWnd].Open}) /goto :clickdoneffs
    /call RemoveCorpseID ${CorpseID}
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub LootCorpse
/return

Sub GlobalLookup(string ItemName)
	/if (${useGlobalCache}) {
		/if (!${Defined[globalCache]}) {
			| Setup Global Cache
			/declare globalCache[${Math.Calc[${keys.Count[|]}-1]},2] string outer
			/for keyIndex 1 to ${Math.Calc[${keys.Count[|]}+1]}
				/varset key ${keys.Token[${keyIndex},"|"]}
				/if (${key.Length} > 0 && ${key.NotEqual[NULL]}) {
					/varset globalCache[${keyIndex},1] ${key}
					/varset globalCache[${keyIndex},2] ${Ini[${NinjaIniFile},Global,${globalCache[${keyIndex},1]}]}
				}
			/next keyIndex
		}
	
		/for keyIndex 1 to ${globalCache.Size[1]}
			/if (${ItemName.Find[${globalCache[${keyIndex},1]}]}) {
				/varset lootOption ${globalCache[${keyIndex},2]}
				/varset lootValue 999999
				/return
			}
		/next keyIndex
	} else {
		/for keyIndex 1 to ${Math.Calc[${keys.Count[|]}+1]}
		  /varset key ${keys.Token[${keyIndex},"|"]}
		  /if (${key.Length} > 0 && ${key.NotEqual[NULL]} && ${ItemName.Find[${key}]}) {
			 /varset lootOption ${Ini[Loot.ini,Global,${key}]}
			 /varset lootValue 999999
			 /return
		  }
		/next keyIndex
	}
/return

Sub Event_CantLoot
   /if (${Debugloot}) /echo DEBUGLOOT Enter Sub Event_CantLoot
   /varset CantLootID ${Target.ID}
   /if (${Debugloot}) /echo DEBUGLOOT  Event_CantLoot Can't loot ${Target.CleanName} ${Target.ID}
   /if (${Debugloot}) /echo DEBUGLOOT Leave Sub Event_CantLoot
/return

Sub RemoveCorpseID(CListID)
    /if (${Defined[CorpseLocked${CListID}]}) {
        /if (!${CorpseLocked${CListID}} && ${MasterCorpseList.Find[${CListID}]}) {
            /deletevar CorpseLocked${CListID}
            /varset MasterCorpseList ${MasterCorpseList.Replace[${CListID}|,]}
        }
    }
/return

Sub LootItem(int i,DoWhat,WhichButton)
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub LootItem ${i} ${DoWhat} ${WhichButton}
    /declare CorpseItemID int local ${Corpse.Item[${i}].ID}
    /nomodkey /shift /itemnotify ${Corpse.Item[${i}].InvSlot} ${WhichButton}mouseup
    /delay 5s ${Window[ConfirmationDialogBox].Open} || !${Corpse.Item[${i}].NoDrop}
    /if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
    /delay 5s ${Cursor.ID} || ${WhichButton.NotEqual[left]}
    /if (${DoWhat.Equal[Destroy]} && ${Cursor.ID}==${CorpseItemID}) /destroy
    /delay 5s !${Corpse.Item[${i}].ID}
    /if (${Cursor.ID}) /call CheckCursor
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub LootItem
/return

Sub CheckCursor
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub CheckCursor
    :ALauto_inv
    /if (${Cursor.ID}) {
        /if (${Cursor.NoDrop} && !${Me.FreeInventory}) /return
        /timed 5 /autoinventory
        /delay 2s !${Cursor.ID}
        /goto :ALauto_inv
    }
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub CheckCursor
/return

Sub ReportLootedValue
	/if (${lootedValue}>0) /call CalculateMoney "Loot Value" ${lootedValue}
/return

| **************** Foraged Items Section ******************** |

Sub Event_Forage
    /if (!${LootForage}) /return
    /declare ForagedItem string local
    :MoreToAutoInventory
        /if (${Cursor.ID}) {
            /varset ForagedItem ${Cursor.Name}
            /if (!${Select[${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"]},Ignore,Keep,Destroy,Sell,NewItem]}) {
                /if (${Cursor.ID}) /ini "${NinjaIniFile}" "${Cursor.Name.Left[1]}" "${Cursor.Name}" NewItem
                /if (${LootForageSpam}) /echo New foraged item >> ${Cursor.Name} << added to ini file. 
            }
            /if (${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Destroy]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Ignore]}) {
                /if (${Cursor.Name.Equal[${ForagedItem}]})     /destroy
                /delay 5 !${Cursor.ID}
                /if (${LootForageSpam}) /echo NINJADVLOOT --  Destroyed foraged ${ForagedItem}.
            } else /if ((${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Keep]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[Sell]} || ${Ini[${NinjaIniFile},"${Cursor.Name.Left[1]}","${Cursor.Name}"].Equal[NewItem]}) && (!${Cursor.Lore} || !${FindItem[=${Cursor.Name}].ID}) && (${Me.FreeInventory} || (${FindItemCount[=${Cursor.Name}]} && ${Cursor.Stackable} && ${Cursor.FreeStack}))) {
                /if (${LootForageSpam}) /echo NINJADVLOOT ++ Kept foraged ${ForagedItem}.
                /autoinventory
                /delay 5 !${Cursor.ID}
            }
        }
    /if (${Cursor.ID}) /goto :MoreToAutoInventory
/return

| **************** Search Loot Section ******************** |

Sub DoLootStuff(LootAction)
    /declare i int local
    /declare j int local
    /declare k int local
    /declare LootItem string
	/declare openedBag bool local
	
	/call NPC "${Target.CleanName}"
	| Top level inv has its own loop since ${FindItem[=${ItemToSell}].InvSlot} looks at top lev inv slots first in Sub SellToVendor for the item to sell.
	| Loop through top level inventory slots for sellable items
	/for k 1 to 10
        | Check if top level inv slot is not a container/bag
        /if (${InvSlot[pack${k}].Item.Container}==0) {
            | If top level inv slot is empty check for an item marked sell
            /if (${InvSlot[pack${k}].Item.ID}) {
                /varset LootItem ${InvSlot[pack${k}].Item}
                /if (${LootAction.Equal[sell]})  {
					/call ItemLookup "${LootItem}"
					/if (${lootOption.Equal[Sell]}) /call SellToVendor "${LootItem}"
                } else /if (${LootAction.Equal[deposit]}) {
					/call ItemLookup "${LootItem}"
                    /if (${lootOption.Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
                }
            }
        }
    /next k
    | Loop through top level inventory slots for containers
    /for i 1 to 10
        | Check if top level inv slot has a bag
        /if (${InvSlot[pack${i}].Item.Container}==0) /next i
		/varset openedBag FALSE
        | Set j 1 to number of bag slots and loop through slots looking for items marked sell
        /for j 1 to ${InvSlot[pack${i}].Item.Container}
            /if (${InvSlot[pack${i}].Item.Item[${j}].ID}) {
				/varset LootItem ${InvSlot[pack${i}].Item.Item[${j}].Name}
				/if (${LootAction.Equal[sell]})  {
					/call ItemLookup "${LootItem}"
					/if (${lootOption.Equal[Sell]}) {
						/if (!${Window[pack${i}].Open}) {
							/nomodkey /itemnotify pack${i} rightmouseup
							/varset openedBag TRUE
						}
						/call SellToVendor "${LootItem}"
					}
				}
				/if (${LootAction.Equal[deposit]}) {
					/call ItemLookup "${LootItem}"
					/if (${lootOption.Equal[Keep]}) /call DepositToGuildBank "${LootItem}"
				}
            }
        /next j
		/if (${openedBag} && ${Window[pack${i}].Open}) /nomodkey /itemnotify pack${i} rightmouseup
    /next i
    | Prevent spam from Sell event after selling items in Sub SellStuff
    /doevents flush Sell
    /if (${Window[MerchantWnd].Open}) {
        /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
    }
    /if (${Window[GuildBankWnd].Open}) {
        /keypress esc
    }
/return

Sub NPC(string NPCName)
    /if (${NPCName.Equal[null]} && ${Target.Type.Equal[npc]}) /varset NPCName ${Target.CleanName}
    /target npc ${NPCName}
    /delay 10
        /if (!${Select[${Target.Type},npc,pet]} || !${Target.ID} || ${Target.Type.Equal[pet]} && !${Target.CleanName.Find[familiar]}) {
        /echo Please target a vendor or guildbanker
        /return
    }
    /delay 1s
    /target id ${Spawn[${NPCName}].ID}
    /delay 3s ${Target.ID}==${Spawn[${NPCName}].ID}
    /if (${Target.Distance}>15) {
        /moveto id  ${Spawn[${NPCName}].ID}
        /delay 250 ${MoveTo.Stopped}
    }
    /face nolook
    /delay 1s
    /nomodkey /click right target
    /delay 3s
/return

Sub ItemLookup(string itemName)
	/varset lootOption ${Ini[${NinjaIniFile},"${itemName.Left[1]}","${itemName}"]}
	/varset lootValue -1
	/if (${lootOption.Equal[NULL]}) {
		| Exact lookup failed, try Global partial match
		/call GlobalLookup "${itemName}"
	} else /if (${lootOption.Find[|]} > 0) {
		/varset lootValue ${lootOption.Token[2,|]}
		/varset lootOption ${lootOption.Token[1,|]}
	}
/return

Sub AddNewItem(string itemName)
	/varset lootOption NewItem
	/echo NewItem: ${itemName}
	/ini "${NinjaIniFile}" "${itemName.Left[1]}" "${itemName}" NewItem
/return

| **************** Sell Loot Section ******************** |
Sub Event_Sell(string soldstr, string ItemName)
    /if ( ${AddNewSales} ) {
        /echo Setting ${ItemName} to sell
        /ini "${NinjaIniFile}" "${ItemName.Left[1]}" "${ItemName}" Sell
    }
/return

Sub SellToVendor(ItemToSell)
    /if (${Window[MerchantWnd].Open}) {
		/varset _moneyReceived FALSE
		:moneyReceivedLoop
		|:sell
        /if (${InvSlot[pack${Select[${FindItem[=${ItemToSell}].ItemSlot},23,24,25,26,27,28,29,30,31,32]}].Item.Container}) {
            /nomodkey /itemnotify in Pack${Math.Calc[${FindItem[=${ItemToSell}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${ItemToSell}].ItemSlot2}+1]} leftmouseup
        } else {
            /nomodkey /itemnotify ${FindItem[=${ItemToSell}].InvSlot} leftmouseup
        }
        /delay 5
        /nomodkey /shiftkey /notify merchantwnd MW_Sell_Button leftmouseup
        /delay 1s
		/doevents MoneyReceived
		|/if (${FindItemCount[=${ItemToSell}]}) /goto :sell
		/if (!${_moneyReceived}) /goto :moneyReceivedLoop
    }
/return

Sub Event_MoneyReceived(string line, string MoneyString)
	/varset _moneyReceived TRUE
	/call MoneyReceivedParser "${MoneyString}" "platinum"
	/call MoneyReceivedParser "${MoneyString}" "gold"
	/call MoneyReceivedParser "${MoneyString}" "silver"
	/call MoneyReceivedParser "${MoneyString}" "copper"
/return

Sub MoneyReceivedParser(string MoneyString, string moneyType)
	/if (${MoneyString.Find[${moneyType}]}) {
		/for spaceIndex 2 to 10
			/if (${MoneyString.Mid[${Math.Calc[${MoneyString.Find[${moneyType}]}-${spaceIndex}]},1].Equal[ ]} ) {
				/varcalc ${moneyType} ${${moneyType}}+${MoneyString.Mid[${Math.Calc[${MoneyString.Find[${moneyType}]}-${spaceIndex}]}, ${spaceIndex}-1]}
				/return
			}
		/next spaceIndex
	}
/return
			
Sub ReportSoldProfit
	/if (${copper} > 9) {
		/varcalc silver ${silver}+${Math.Calc[${copper}/10]}
		/varcalc copper ${copper}%10
	}
	/if (${silver} > 9) {
		/varcalc gold ${gold}+${Math.Calc[${silver}/10]}
		/varcalc silver ${silver}%10
	}
	/if (${gold} > 9) {
		/varcalc platinum ${platinum}+${Math.Calc[${gold}/10]}
		/varcalc gold ${gold}%10
	}
	
	/echo Received ${platinum} platinum ${gold} gold ${silver} silver ${copper} copper!
/return

| **************** Deposit Loot Section ******************** 
Sub DepositToGuildBank(ItemToDeposit)
    /if (${Window[GuildBankWnd].Open}) {
        :deposit
            /if (${Window[GuildBankWnd].Child[GBANK_DepositCountLabel].Text.Arg[2,:]}==0) {
                /echo Guildbank Deposited Items is full.
                /return
            }
            /if (${FindItem[=${ItemToDeposit}].InvSlot}<=22) /return
            /echo Depositing ${ItemToDeposit}
            /if (!${InvSlot[pack${Float[${Math.Calc[${Select[${FindItem[=${ItemToDeposit}].ItemSlot},22,23,24,25,26,27,28,29,30,31,32]}-1]}].Int}].Item.Container}) {
                /nomodkey /shift /itemnotify ${FindItem[=${ItemToDeposit}].InvSlot} leftmouseup
            } else {
                /nomodkey /shift /itemnotify in Pack${Math.Calc[${FindItem[=${ItemToDeposit}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${ItemToDeposit}].ItemSlot2}+1]} leftmouseup
            }
            /delay 5
            /nomodkey /notify GuildBankWnd GBANK_DepositButton leftmouseup
            /delay 2s
        /if (${FindItem[=${ItemToDeposit}].InvSlot}) /goto :deposit
    }
/return

| **************** Item set ini file event ******************** 
Sub Event_EditIniItem(ItemString,ItemAction,ItemName)
	/if (${Cursor.ID}) {
		/echo -- Setting ${ItemName} to ${ItemAction}
		/ini "${NinjaIniFile}" ${ItemName.Left[1]} "${ItemName}" ${ItemAction}
	} else {
		/echo No item on cursor.
	}
/return

| **************** Sell Stuff Event ******************** 
Sub Event_SellStuff
	/call DoLootStuff sell
/return
    
| **************** Deposit Stuff Event *******************
Sub Event_DepositStuff
	/call DoLootStuff deposit
/return

| **************** Broke Event ******************** 
Sub Event_Broke 
    /if (!${Window[MerchantWnd].Open}) /return
    /echo You are out of money! 
    /endmacro 
/return

| **************** Type Stuff  ******************** 
| Lifted from commonsubs.inc by Armysoldier    
Sub Type(InStr) 
    /declare char string local 
    /declare loopctr int local 
    /for loopctr 1 to ${InStr.Length} 
        /varset char ${InStr.Mid[${loopctr},1]} 
        /if (!${char.Length}) { 
            /nomodkey /keypress space chat 
        } else { 
            /nomodkey /keypress ${char} chat 
        } 
    /next loopctr 
/return 

| **************** Buy Stuff ********************  
Sub Buy(string ItemToBuy, int amount)
    /if (!${Select[${Target.Type},npc,pet]} || !${Target.ID} || ${Target.Type.Equal[pet]} && !${Target.CleanName.Find[familiar]}) /return
    /declare i int local
    | /echo Buying ${ItemToBuy}!
    /declare QTY int local
    /declare ListItem int local
    /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]}
    /delay 3
    /if (${FindItemCount[${ItemToBuy}]}>=${amount}) {
        /echo I already have ${FindItemCount[${ItemToBuy}]} ${ItemToBuy}.
        /return
    }   
    /varset ListItem ${Window[MerchantWnd].Child[ItemList].List[=${ItemToBuy},2]}
    /if (!${ListItem}) {
        /echo couldn't find ${ItemToBuy}
        /return
    } else {
        /notify MerchantWnd ItemList listselect ${ListItem}
        /delay 5       
    } i
    /echo Buying ${ItemToBuy} Till I get ${amount} 
    :buyLoop
    /doevents
    /if (${InventoryFull}) /return
    /if (${QTY}>0) {
        /if (${QTY}>19) {
            /Shiftkey /notify merchantwnd MW_Buy_Button leftmouseup
            /delay 5 ${FindItemCount[B${ItemToBuy}]}>=${QTY}
            /echo ${FindItemCount[${ItemToBuy}]}/${amount} ${ItemToBuy} in inventory
            /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]} 
            /if (${QTY}<=0) {
                /if (${Window[MerchantWnd].Open}) {
                    /echo Closing merchant window
                    /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
                }
                /return
            }
            /delay 5
            /if (${QTY}>19) /goto :buyLoop
        }
        /if (${QTY}>0 && ${QTY}<20) {
            /for i 1 to ${QTY} 
                /Ctrlkey /notify merchantwnd MW_Buy_Button leftmouseup
                /delay 1s ${FindItemCount[${ItemToBuy}]}>=${Math.Calc[${FindItemCount[${ItemToBuy}]}+${i}]}
                /echo Buying ${ItemToBuy} ${i} of ${QTY}
                /if (${i}>=${QTY}) {
                    /echo ${FindItemCount[${ItemToBuy}]} ${ItemToBuy} in inventory
                    /if (${Window[MerchantWnd].Open}) {
                        /echo Closing merchant window
                        /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
                    }
                    /return
                }
            /next i
            /varcalc QTY ${amount}-${FindItemCount[${ItemToBuy}]}
            
        }
    }
/return
    
Sub ZCheck(float Zcur, float Zdiff)
    | Move down if Z distance more than Zcheck due to levitation
    /if (${Zcur}>=${Zdiff}) {
        /keypress end hold
        /delay 10
        /keypress end
    }
/return

Sub Event_InventoryFull
    /echo Your Inventory is full. Turning off looting functions.
    /varset LootMobs FALSE
    /varset InventoryFull 1
/return

Sub CreateIni
    /if (${Ini[${NinjaIniFile}].Length}) /return
    /declare a local Settings|Global|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z
    /echo Creating ini file ${NinjaIniFile}
    /declare i int local 
    /declare j int local 
    /for i 1 to 27
        /ini ${NinjaIniFile} "${a.Arg[${i},|]}" "|===================================" "==================================|"
        /if (${i}==2) {
            /for j 1 to ${GlobalCount}
                /ini ${NinjaIniFile} "Global" "Global${j}" "Item Name|Ignore/Destroy/Keep/Sell"
            /next j
        }
    /next i
/return

Sub Event_NullSlot
        /echo NINJADVLOOT: Opening all bags to correct null slot
        /keypress OPEN_INV_BAGS
    /timed 30 /keypress CLOSE_INV_BAGS
/return

Sub UseAdvLoot
    /if (!${Me.FreeInventory}) /return
    /if (${Debugloot}) /echo Enter UseAdvLoot
    /if (${Me.UseAdvancedLooting}) {
        /if (${Debugloot}) /echo Start Loot Check
        /delay 1s ${Window[AdvancedLootWnd].Open}
		/if (!${Window[AdvancedLootWnd].Open}) {
			/if (${Debugloot}) /echo Nothing to loot
			/return
		}
		
        /declare ITPName string local
		/declare lootManualCount int local 0
		/declare lootCount int local ${Math.Calc[${AdvLoot.SCount}-${lootManualCount}]}
		/declare canLootShared bool local TRUE
        /if (${Raid.Members} && ${Raid.LootType} != 4) {
			/echo NINJADVLOOT: You are in a raid and the loot is not set to open
			/varset canLootShared FALSE
		} else /if (!${Raid.Members} && ${AdvLoot.SList[1].ID} && !${Group.MasterLooter.ID}) {
			/echo NINJADVLOOT: Please Assign a player the Master Looter Role in the group window.
			/varset canLootShared FALSE
		}
		
        /if (${canLootShared} && ${AdvLoot.SCount} > 0) {
			:sharedLoop
				/delay 5s !${AdvLoot.LootInProgress}
				
				/varcalc lootCount ${AdvLoot.SCount}-${lootManualCount}
				/if (${AdvLoot.SList[${lootCount}].ID} && ${AdvLoot.SList[${lootCount}].Name.NotEqual[NULL]}) {
					| Determine loot option
					/varset ITPName ${AdvLoot.SList[${lootCount}].Name}
					/if (${ITPName.Equal[NULL]}) /goto :sharedLoop
					/call ItemLookup "${ITPName}"
					/if (${lootOption.Length} <= 0 || ${lootOption.Equal[NULL]}) {
						| Found new item, tag as such
						/call AddNewItem "${ITPName}"
					}
					
					/if (${Debugloot}) /echo ${lootCount} ${ITPName}=${lootOption}
					/if (${lootOption.Equal[ManualLoot]} && ${overrideManualLoot.Length} > 0) /varset lootOption ${overrideManualLoot}
					
					/if (${lootOption.Equal[Sell]} && ${lootValue} < ${minValue} && ${lootValue} > -1) /varset lootOption Ignore
					
					/if (${lootOption.Equal[Ignore]}) {
						/call SafeLoot shared ${lootCount} leave
					} else /if ((${lootOption.Equal[NewItem]} && ${AdvLoot.SList[${lootCount}].NoDrop}) || ${lootOption.Equal[ManualLoot]}) {
						/echo ManualLoot: ${ITPName}
						/varcalc lootManualCount ${lootManualCount}+1
					| skip item if its lore and I have one
					} else /if (${FindItem[=${ITPName}].Lore} || ${FindItemBank[=${ITPName}].Lore}) {
						/echo ${ITPName} is Lore and I have one.
						/call SafeLoot shared ${lootCount} leave
					} else /if (${lootValue} < 0) {
						/call SafeLoot shared ${lootCount} giveto ${Me.Name}
					} else /if (${Select[${lootOption},Keep,Destroy,Sell,NewItem,ManualLoot]}) {
						/call SafeLoot shared ${lootCount} giveto ${Me.Name}
					} else {
						/echo Unknown Loot Option: ${lootOption}
						/return
					}
					/delay 4
				}
			/if (${AdvLoot.SCount} > ${lootManualCount}) /goto :sharedLoop
        }
		
        | Loot/destroy anything in personal list (in reverse order)
        /if (${AdvLoot.PCount}) {
			/varset lootManualCount 0
            :personalLoop
				/delay 5s !${AdvLoot.LootInProgress}
				
				/varset lootCount ${Math.Calc[${AdvLoot.PCount}-${lootManualCount}]}
				/if (${AdvLoot.PList[${lootCount}].ID}) {
					| Determine loot option
					/varset ITPName ${AdvLoot.PList[${lootCount}].Name}
					/if (${ITPName.Equal[NULL]}) /goto :personalLoop
					/call ItemLookup "${ITPName}"
					/if (${lootOption.Length} <= 0 || ${lootOption.Equal[NULL]}) {
						| Found new item, tag as such
						/call AddNewItem "${ITPName}"
					}
					
					/if (${Debugloot}) /echo ${lootCount} ${ITPName}=${lootOption}|${lootValue}
					/if (${lootOption.Equal[ManualLoot]} && ${overrideManualLoot.Length} > 0) /varset lootOption ${overrideManualLoot}
					/if (${lootOption.Equal[Sell]} && ${lootValue} < ${minValue} && ${lootValue} > -1) /varset lootOption Ignore
					
					/if (${lootOption.Equal[Ignore]}) {
						/if (${Debugloot}) /echo Leaving ${ITPName}
						/call SafeLoot personal ${lootCount} leave
					} else /if (${lootOption.Equal[ManualLoot]}) {
						/echo ManualLoot: ${ITPName}
						/varcalc lootManualCount ${lootManualCount}+1
					| skip item if its lore and i have one
					} else /if (${FindItem[=${ITPName}].Lore} || ${FindItemBank[=${ITPName}].Lore}) {
						/echo ${ITPName} is Lore and I have one.
						/call SafeLoot personal ${lootCount} leave
					} else /if (${lootValue} < 0 && !${AdvLoot.PList[${lootCount}].NoDrop}) {
						/call SafeLoot personal ${lootCount} loot
						/if (${Window[ConfirmationDialogBox].Open}) {
							/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
						}
						/call HandleItem "${ITPName}" Price
						/if (${Select[${lootOption},Destroy,Ignore]}) {
							/call HandleItem "${ITPName}" Destroy
						}
					} else /if (${Select[${lootOption},Keep,Destroy,Sell,NewItem,ManualLoot]}) {
						/if (${Debugloot}) /echo Looting ${ITPName}
						/call SafeLoot personal ${lootCount} loot
						/if (${Window[ConfirmationDialogBox].Open}) {
							/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
						}
						/if (${lootOption.Equal[Destroy]}) {
							/call HandleItem "${ITPName}" Destroy
						}
					} else {
						/echo Unknown Loot Option: ${lootOption}
						/return
					}
					
					/delay 4
				}
            /if (${AdvLoot.PCount} > ${lootManualCount}) /goto :personalLoop
        }
    }
    /if (${Debugloot})  /echo Leave UseAdvLoot
/return

Sub SafeLoot(listType, index, command, who)
	/if (!${AdvLoot.LootInProgress}) {
		/if (${who.Length}) /varset command ${command} ${who}
		/if (${Debugloot}) /echo advloot ${listType} ${index} ${command}
		/advloot ${listType} ${index} ${command}
	} else {
		/echo Loot was in progress, waiting...
	}
/return

Sub HandleItem(ItemName, DoWhat)
    /if (${Debugloot}) /echo DEBUGLOOT Enter Sub HandleItem ${DoWhat} ${ItemName}
    /declare HandleTimer timer local 10s
    /if (${DoWhat.Equal[Destroy]}) {
        :GetItem
            /squelch /nomodkey /shift /itemnotify "${ItemName}" leftmouseup
            /delay 2s ${Cursor.ID}
        /if (!${Cursor.ID} && ${HandleTimer}) /goto :GetItem
        /if (${Cursor.ID}==${FindItem[=${ItemName}].ID}) {
			/echo NINJADVLOOT: Destroying ${Cursor.Name}
			/destroy
			/delay 5
			/if (${Window[ConfirmationDialogBox].Open}) {
				/delay 10
				/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
			}
            /delay 2s !${Cursor.ID}
        }
    } else /if (${DoWhat.Equal[Price]}) {
		:FindItemLoop
			/if (!${FindItem[=${ItemName}].ID} && ${HandleTimer}) {
				/delay 5
				/goto :FindItemLoop
			}
		/if (!${FindItem[=${ItemName}].ID}) {
			/echo Could not find item to Price: ${ItemName}
			/return
		}
		/call ItemLookup "${ItemName}"
		/if (${lootOption.Length} <= 0 || ${lootOption.Equal[NULL]}) {
			| Found new item, tag as such
			/call AddNewItem "${ItemName}"
		}
		/varset lootValue ${FindItem[=${ItemName}].Value}
		/echo ${ItemName} priced at: ${lootValue}
		/ini "${NinjaIniFile}" "${ItemName.Left[1]}" "${ItemName}" ${lootOption}|${lootValue}
	}
    /if (${Debugloot}) /echo DEBUGLOOT Leave Sub HandleItem
/return

Sub ReportInventoryItemValue
	/declare i int local
	/declare j int local
	/declare itemName string local
	/varset heldValue 0
	
	| Top level inv has its own loop since ${FindItem[=${ItemToSell}].InvSlot} looks at top lev inv slots first in Sub SellToVendor for the item to sell.
	| Loop through top level inventory slots for sellable items
	/for i 1 to 10
		/varset itemName ${InvSlot[pack${i}].Item}
		/if (${InvSlot[pack${i}].Item.Container}==0 && ${InvSlot[pack${i}].Item.ID}) {
			/varset itemName ${InvSlot[pack${i}].Item.Name}
			/varset lootOption ${Ini[${NinjaIniFile},"${itemName.Left[1]}","${itemName}"]}
			/if (${lootOption.Find[Sell]}) {
				/varcalc heldValue ${heldValue}+(${InvSlot[pack${i}].Item.Value}*${InvSlot[pack${i}].Item.Stack})
			} else /if (${lootOption.Equal[NULL]}) {
				| GlobalLookup is costly, so do it only if first lookup fails
				/call GlobalLookup "${itemName}"
				/if (${lootOption.Find[Sell]}) {
					/varcalc heldValue ${heldValue}+(${InvSlot[pack${i}].Item.Value}*${InvSlot[pack${i}].Item.Stack})
				}
			}
		}
	/next i
	
	| Loop through top level inventory slots for containers
	/for i 1 to 10
		| Check if top level inv slot has a bag
		/if (${InvSlot[pack${i}].Item.Container}==0 || !${InvSlot[pack${i}].Item.Container}) /next i
		| Set j 1 to number of bag slots and loop through slots looking for items marked sell
		/for j 1 to ${InvSlot[pack${i}].Item.Container}
			/if (${InvSlot[pack${i}].Item.Item[${j}].ID}) {
				/varset itemName ${InvSlot[pack${i}].Item.Item[${j}].Name}
				/varset lootOption ${Ini[${NinjaIniFile},"${itemName.Left[1]}","${itemName}"]}
				/if (${lootOption.Find[Sell]}) {
					/varcalc heldValue ${heldValue}+(${InvSlot[pack${i}].Item.Item[${j}].Value}*${InvSlot[pack${i}].Item.Item[${j}].Stack})
				} else /if (${lootOption.Equal[NULL]}) {
					| GlobalLookup is costly, so do it only if first lookup fails
					/call GlobalLookup "${itemName}"
					/if (${lootOption.Equal[Sell]}) {
						/varcalc heldValue ${heldValue}+(${InvSlot[pack${i}].Item.Item[${j}].Value}*${InvSlot[pack${i}].Item.Item[${j}].Stack})
					}
				}
			}
		/next j
	/next i
	/call CalculateMoney "Held Value" ${heldValue}
/return

Sub CalculateMoney(String echoString, int moneyInCopper)
	/echo ${echoString}: ${Math.Calc[${moneyInCopper}/1000].Int} Platinum ${Math.Calc[(${moneyInCopper}/100)%10].Int} gold ${Math.Calc[(${moneyInCopper}/10)%10].Int} silver ${Math.Calc[${moneyInCopper}%10].Int} copper!
/return

