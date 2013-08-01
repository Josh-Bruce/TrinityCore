/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptPCH.h"
#include "Chat.h"
#include <time.h>

// Player auras to remove
enum Auras
{
	AURA_STEALTH							= 1784,
	AURA_VANISH								= 26889,
	AURA_PROWL								= 5215,
};

// Gossip menu text
#define GOSSIP_ALLIANCE_TOWN				"Alliance Town ->"
#define GOSSIP_ALLIANCE_STORMWIND			"Stormwind"
#define GOSSIP_ALLIANCE_IRONFORGE			"Ironforge"
#define GOSSIP_ALLIANCE_EXODAR				"Exodar"
#define GOSSIP_ALLIANCE_DARNASSUS			"Darnassus"

#define GOSSIP_HORDE_TOWN					"Horde Town ->"
#define GOSSIP_HORDE_ORGRIMMAR				"Orgrimmar"
#define GOSSIP_HORDE_SILVERMOON				"Silvermoon"
#define GOSSIP_HORDE_THUNDER_BLUFF			"Thunder Bluff"
#define GOSSIP_HORDER_UNDERCITY				"Undercity"

#define GOSSIP_NEUTRAL_TOWN					"Neutral Town ->"
#define GOSSIP_NEUTRAL_SHATTRATH			"Shattrath City"
#define GOSSIP_NEUTRAL_DALARAN				"Dalaran"

#define GOSSIP_EVENT_LOCATIONS				"Event Locations ->"
#define GOSSIP_SUMMIT_COURSE				"Summit Course"

#define GOSSIP_PLAYER_TOOLS					"Player Tools ->"
#define GOSSIP_PLAYER_UNBIND_INSTANCE		"Reset Instance Locks Cost: [5 VP]"
#define GOSSIP_PLAYER_COOLDOWN				"Reset Cooldowns Cost: [150 Gold]"

#define GOSSIP_PVP							"Alliance vs Horde PvP"

#define GOSSIP_MAIN_MENU					"<- Main Menu"

#define GOSSIP_COMBAT						"You're in combat!"
#define GOSSIP_NOT_VOTED					"You have not voted in the past 12 hours! Please go to www.immortalityreborn.com and vote to use the advanced functions of the TeleNPC!"
#define GOSSIP_NOT_ENOUGH_GOLD				"You do not have enough gold!"
#define GOSSIP_COOLDOWN_RESET				"Your cooldowns have been reset."

class tele_npc_vote : public CreatureScript
{
	public:
		tele_npc_vote() : CreatureScript("tele_npc_vote") { }

		bool OnGossipHello(Player* player, Creature* creature) OVERRIDE
		{
			// Check if player is in combat
			if (player->IsInCombat())
			{
				//player->GetSession()->SendNotification(GOSSIP_COMBAT);
				ChatHandler(player->GetSession()).PSendSysMessage(GOSSIP_COMBAT);
				return false;
			}

			// Check if the player has voted
			if (!player->HasVoted())
				ChatHandler(player->GetSession()).PSendSysMessage(GOSSIP_NOT_VOTED);

			// Remove auras from players
			if (player->HasAura(AURA_STEALTH))
				player->RemoveAura(AURA_STEALTH);
			if (player->HasAura(AURA_VANISH))
				player->RemoveAura(AURA_VANISH);
			if (player->HasAura(AURA_PROWL))
				player->RemoveAura(AURA_PROWL);

			if (player->HasVoted())
			{
				// Check for team
				if (player->GetTeam() == ALLIANCE)
					// Alliance only cities
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_ALLIANCE_TOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000);
				else
					// Horde only cities
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_HORDE_TOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2000);
				// Neutral cities
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_NEUTRAL_TOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3000);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_EVENT_LOCATIONS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4000);
				// Vote check here
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_PLAYER_TOOLS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5000);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GOSSIP_PVP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6000);
			}

			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
			return true;
		}

		bool OnGossipSelect(Player* player, Creature* creature, uint32 /* sender */, uint32 actions) OVERRIDE
		{
			// If not player, return false
			if (!player)
				return false;

			// Clear menu
			player->PlayerTalkClass->ClearMenus();

			// Switch over actions
			switch (actions)
			{
				// Main Menu
				case GOSSIP_ACTION_INFO_DEF:
					if (player->HasVoted())
					{
						if (player->GetTeam() == ALLIANCE)
							player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_ALLIANCE_TOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000);
						else
							player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_HORDE_TOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2000);
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_NEUTRAL_TOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3000);
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_EVENT_LOCATIONS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4000);
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_PLAYER_TOOLS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5000);
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GOSSIP_PVP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6000);
					}
					player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
					break;
				// Alliance Options
				case GOSSIP_ACTION_INFO_DEF + 1000:
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_ALLIANCE_STORMWIND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1001);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_ALLIANCE_IRONFORGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1002);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_ALLIANCE_EXODAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1003);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_ALLIANCE_DARNASSUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1004);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_MAIN_MENU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
					break;
				// Stormwind
				case GOSSIP_ACTION_INFO_DEF + 1001:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(0, -8960.14f, 516.266f, 96.3568f, 0.0f);
					break;
				// Ironforge
				case GOSSIP_ACTION_INFO_DEF + 1002:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(0, -4924.07f, -951.95f, 501.55f, 5.40f);
					break;
				// Exodar
				case GOSSIP_ACTION_INFO_DEF + 1003:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(530, -3954.20f, -11656.54f, -138.69f, 0.0f);
					break;
				// Darnassus
				case GOSSIP_ACTION_INFO_DEF + 1004:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(1, 9947.52f, 2482.73f, 1316.21f, 0.0f);
					break;
				// Horde Options
				case GOSSIP_ACTION_INFO_DEF + 2000:
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_HORDE_ORGRIMMAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2001);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_HORDE_SILVERMOON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2002);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_HORDE_THUNDER_BLUFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2003);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_HORDER_UNDERCITY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2004);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_MAIN_MENU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
					break;
				// Orgrimmar
				case GOSSIP_ACTION_INFO_DEF + 2001:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(1, 1552.5f, -4420.66f, 8.94802f, 0.0f);
					break;
				// Silvermoon
				case GOSSIP_ACTION_INFO_DEF + 2002:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(530, 9338.74f, -7277.27f, 13.7895f, 0.0f);
					break;
				// Thunder Bluff
				case GOSSIP_ACTION_INFO_DEF + 2003:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(1, -1290.0f, 147.033997f, 129.682007f, 4.919000f);
					break;
				// Undercity
				case GOSSIP_ACTION_INFO_DEF + 2004:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(0, 1819.71f, 238.79f, 60.5321f, 0.0f);
					break;
				// Neutral Options
				case GOSSIP_ACTION_INFO_DEF + 3000:
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_NEUTRAL_SHATTRATH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3001);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_NEUTRAL_DALARAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3002);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_MAIN_MENU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
					break;
				// Shattrath
				case GOSSIP_ACTION_INFO_DEF + 3001:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(530, -1850.209961f, 5435.821777f, -10.961435f, 3.403913f);
					break;
				// Dalaran
				case GOSSIP_ACTION_INFO_DEF + 3002:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(571, 5819.08f, 643.99f, 647.80f, 3.32f);
					break;
				// Event locations
				case GOSSIP_ACTION_INFO_DEF + 4000:
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_SUMMIT_COURSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4001);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_MAIN_MENU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
					break;
				// Summit course
				case GOSSIP_ACTION_INFO_DEF + 4001:
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(0, -11938.4f, -866.245f, 32.7849f, 4.55f);
					break;
				// Player tools
				case GOSSIP_ACTION_INFO_DEF + 5000:
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_PLAYER_UNBIND_INSTANCE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5001);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_PLAYER_COOLDOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5002);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_MAIN_MENU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
					break;
				// Reset player instances
				case GOSSIP_ACTION_INFO_DEF + 5001:
					player->CLOSE_GOSSIP_MENU();
					player->UnbindAllInstances();
					break;
				// Reset player cooldowns
				case GOSSIP_ACTION_INFO_DEF + 5002:
					player->CLOSE_GOSSIP_MENU();
					if (player->GetMoney() < 1500000)
						ChatHandler(player->GetSession()).PSendSysMessage(GOSSIP_NOT_ENOUGH_GOLD);
					else
					{
						player->ModifyMoney(-1500000);
						player->RemoveAllSpellCooldown();
						ChatHandler(player->GetSession()).PSendSysMessage(GOSSIP_COOLDOWN_RESET);
					}
					break;
				// Alliance vs Horde PvP
				case GOSSIP_ACTION_INFO_DEF + 6000:
					player->CLOSE_GOSSIP_MENU();
					// Razor Hill PvP For Alliance | Goldshire PvP For Horde
					if (player->GetTeam() == ALLIANCE)
						player->TeleportTo(1, 315.721f, -4743.4f, 10.4867f, 0.0f);
					else
						player->TeleportTo(0, -9464.0f, 62.0f, 56.0f, 0.0f);
					break;
			}

			return true;
		}
};

void AddSC_tele_npc_vote()
{
	new tele_npc_vote();
};