CFG ?= default
-include config.$(CFG)


ifeq ($(findstring $(LNG), DUTCH ENGLISH FRENCH GERMAN ITALIAN POLISH RUSSIAN RUSSIAN_GOLD),)
$(error LNG must be set to one of DUTCH, ENGLISH, FRENCH, GERMAN, ITALIAN, POLISH, RUSSIAN or RUSSIAN_GOLD. Copy config.template to config.default and uncomment one of the languages)
endif

ifndef SGPDATADIR
$(warn No SGPDATADIR specified, make lowercase will not work)
endif


BINARY    ?= ja2
BUILDDIR  ?= build/$(CFG)
PREFIX    ?= /usr/local
MANPREFIX ?= $(PREFIX)

INSTALL         ?= install
INSTALL_PROGRAM ?= $(INSTALL) -m 555 -s
INSTALL_MAN     ?= $(INSTALL) -m 444
INSTALL_DATA    ?= $(INSTALL) -m 444


SDL_CONFIG  ?= sdl-config
ifndef CFLAGS_SDL
CFLAGS_SDL  := $(shell $(SDL_CONFIG) --cflags)
endif
ifndef LDFLAGS_SDL
LDFLAGS_SDL := $(shell $(SDL_CONFIG) --libs)
endif


CFLAGS += $(CFLAGS_SDL)
CFLAGS += -I src/game
CFLAGS += -I src/game/Tactical
CFLAGS += -I src/game/Strategic
CFLAGS += -I src/game/Editor
CFLAGS += -I src/game/Res
CFLAGS += -I src/game/Laptop
CFLAGS += -I src/game/Utils
CFLAGS += -I src/game/TileEngine
CFLAGS += -I src/game/TacticalAI
CFLAGS += -I src/sgp

#CFLAGS += -Wall
#CFLAGS += -W
CFLAGS += -Wpointer-arith
CFLAGS += -Wreturn-type
CFLAGS += -Wunused-label
CFLAGS += -Wunused-variable
CFLAGS += -Wwrite-strings

CFLAGS += -DJA2


ifdef WITH_DEMO
  CFLAGS += -DJA2DEMO
endif

ifdef WITH_DEMO_ADS
  ifndef WITH_DEMO
    $(error WITH_DEMO_ADS needs WITH_DEMO)
  endif
  CFLAGS += -DJA2DEMOADS
endif

ifdef WITH_FIXMES
  CFLAGS += -DWITH_FIXMES
endif

ifdef WITH_MAEMO
  CFLAGS += -DWITH_MAEMO
endif

ifdef WITH_SOUND_DEBUG
  CFLAGS += -DWITH_SOUND_DEBUG
endif

ifdef _DEBUG
  CFLAGS += -D_DEBUG
  ifndef JA2TESTVERSION
    JA2TESTVERSION := yes
  endif
endif

ifdef JA2TESTVERSION
  CFLAGS += -DJA2TESTVERSION
  ifndef JA2BETAVERSION
  	JA2BETAVERSION := yes
  endif
endif

ifdef JA2BETAVERSION
CFLAGS += -DJA2BETAVERSION -DSGP_DEBUG -DFORCE_ASSERTS_ON -DSGP_VIDEO_DEBUGGING
endif

ifdef JA2EDITOR
CFLAGS += -DJA2EDITOR
endif

CFLAGS += -D$(LNG)

CFLAGS += -DSGPDATADIR=\"$(SGPDATADIR)\"

CCFLAGS += $(CFLAGS)
CCFLAGS += -std=gnu99
CCFLAGS += -Werror-implicit-function-declaration
CCFLAGS += -Wimplicit-int
CCFLAGS += -Wmissing-prototypes

CXXFLAGS += $(CFLAGS)

LDFLAGS += $(LDFLAGS_SDL)
LDFLAGS += -lm

ifdef WITH_ZLIB
LDFLAGS += -lz
endif

SRCS :=

SRCS += src/sgp/Button_Sound_Control.cc
SRCS += src/sgp/Button_System.cc
SRCS += src/sgp/Config.cc
SRCS += src/sgp/Container.cc
SRCS += src/sgp/Cursor_Control.cc
SRCS += src/sgp/Debug.cc
SRCS += src/sgp/FileMan.cc
SRCS += src/sgp/Font.cc
SRCS += src/sgp/HImage.cc
SRCS += src/sgp/ImpTGA.cc
SRCS += src/sgp/Input.cc
SRCS += src/sgp/LibraryDataBase.cc
SRCS += src/sgp/Line.cc
SRCS += src/sgp/MemMan.cc
SRCS += src/sgp/MouseSystem.cc
SRCS += src/sgp/PCX.cc
SRCS += src/sgp/Random.cc
SRCS += src/sgp/SGP.cc
SRCS += src/sgp/SGPStrings.cc
SRCS += src/sgp/STCI.cc
SRCS += src/sgp/Shading.cc
SRCS += src/sgp/Smack_Stub.cc
SRCS += src/sgp/SoundMan.cc
SRCS += src/sgp/TranslationTable.c
SRCS += src/sgp/VObject.cc
SRCS += src/sgp/VObject_Blitters.cc
SRCS += src/sgp/VSurface.cc
SRCS += src/sgp/Video.cc

SRCS += src/game/AniViewScreen.cc
SRCS += src/game/Credits.cc
SRCS += src/game/Fade_Screen.cc
SRCS += src/game/GameInitOptionsScreen.cc
SRCS += src/game/GameLoop.cc
SRCS += src/game/GameScreen.cc
SRCS += src/game/GameSettings.cc
SRCS += src/game/GameVersion.cc
SRCS += src/game/HelpScreen.cc
SRCS += src/game/Init.cc
SRCS += src/game/Intro.cc
SRCS += src/game/JA2_Splash.cc
SRCS += src/game/JAScreens.cc
SRCS += src/game/Laptop/AIM.cc
SRCS += src/game/Laptop/AIMArchives.cc
SRCS += src/game/Laptop/AIMFacialIndex.cc
SRCS += src/game/Laptop/AIMHistory.cc
SRCS += src/game/Laptop/AIMLinks.cc
SRCS += src/game/Laptop/AIMMembers.cc
SRCS += src/game/Laptop/AIMPolicies.cc
SRCS += src/game/Laptop/AIMSort.cc
SRCS += src/game/Laptop/BobbyR.cc
SRCS += src/game/Laptop/BobbyRAmmo.cc
SRCS += src/game/Laptop/BobbyRArmour.cc
SRCS += src/game/Laptop/BobbyRGuns.cc
SRCS += src/game/Laptop/BobbyRMailOrder.cc
SRCS += src/game/Laptop/BobbyRMisc.cc
SRCS += src/game/Laptop/BobbyRShipments.cc
SRCS += src/game/Laptop/BobbyRUsed.cc
SRCS += src/game/Laptop/BrokenLink.cc
SRCS += src/game/Laptop/CharProfile.cc
SRCS += src/game/Laptop/EMail.cc
SRCS += src/game/Laptop/Files.cc
SRCS += src/game/Laptop/Finances.cc
SRCS += src/game/Laptop/Florist.cc
SRCS += src/game/Laptop/Florist_Cards.cc
SRCS += src/game/Laptop/Florist_Gallery.cc
SRCS += src/game/Laptop/Florist_Order_Form.cc
SRCS += src/game/Laptop/Funeral.cc
SRCS += src/game/Laptop/History.cc
SRCS += src/game/Laptop/IMPVideoObjects.cc
SRCS += src/game/Laptop/IMP_AboutUs.cc
SRCS += src/game/Laptop/IMP_Attribute_Entrance.cc
SRCS += src/game/Laptop/IMP_Attribute_Finish.cc
SRCS += src/game/Laptop/IMP_Attribute_Selection.cc
SRCS += src/game/Laptop/IMP_Begin_Screen.cc
SRCS += src/game/Laptop/IMP_Compile_Character.cc
SRCS += src/game/Laptop/IMP_Confirm.cc
SRCS += src/game/Laptop/IMP_Finish.cc
SRCS += src/game/Laptop/IMP_HomePage.cc
SRCS += src/game/Laptop/IMP_MainPage.cc
SRCS += src/game/Laptop/IMP_Personality_Entrance.cc
SRCS += src/game/Laptop/IMP_Personality_Finish.cc
SRCS += src/game/Laptop/IMP_Personality_Quiz.cc
SRCS += src/game/Laptop/IMP_Portraits.cc
SRCS += src/game/Laptop/IMP_Text_System.cc
SRCS += src/game/Laptop/IMP_Voices.cc
SRCS += src/game/Laptop/Insurance.cc
SRCS += src/game/Laptop/Insurance_Comments.cc
SRCS += src/game/Laptop/Insurance_Contract.cc
SRCS += src/game/Laptop/Insurance_Info.cc
SRCS += src/game/Laptop/Laptop.cc
SRCS += src/game/Laptop/Mercs.cc
SRCS += src/game/Laptop/Mercs_Account.cc
SRCS += src/game/Laptop/Mercs_Files.cc
SRCS += src/game/Laptop/Mercs_No_Account.cc
SRCS += src/game/Laptop/Personnel.cc
SRCS += src/game/Laptop/Store_Inventory.cc
SRCS += src/game/LoadSaveEMail.cc
SRCS += src/game/LoadSaveTacticalStatusType.cc
SRCS += src/game/Loading_Screen.cc
SRCS += src/game/MainMenuScreen.cc
SRCS += src/game/MessageBoxScreen.cc
SRCS += src/game/Options_Screen.cc
SRCS += src/game/SaveLoadGame.cc
SRCS += src/game/SaveLoadScreen.cc
SRCS += src/game/Screens.cc
SRCS += src/game/Strategic/AI_Viewer.cc
SRCS += src/game/Strategic/Assignments.cc
SRCS += src/game/Strategic/Auto_Resolve.cc
SRCS += src/game/Strategic/Campaign_Init.cc
SRCS += src/game/Strategic/Creature_Spreading.cc
SRCS += src/game/Strategic/Game_Clock.cc
SRCS += src/game/Strategic/Game_Event_Hook.cc
SRCS += src/game/Strategic/Game_Events.cc
SRCS += src/game/Strategic/Game_Init.cc
SRCS += src/game/Strategic/Hourly_Update.cc
SRCS += src/game/Strategic/LoadSaveSectorInfo.cc
SRCS += src/game/Strategic/LoadSaveStrategicMapElement.cc
SRCS += src/game/Strategic/LoadSaveUndergroundSectorInfo.cc
SRCS += src/game/Strategic/MapScreen.cc
SRCS += src/game/Strategic/Map_Screen_Helicopter.cc
SRCS += src/game/Strategic/Map_Screen_Interface.cc
SRCS += src/game/Strategic/Map_Screen_Interface_Border.cc
SRCS += src/game/Strategic/Map_Screen_Interface_Bottom.cc
SRCS += src/game/Strategic/Map_Screen_Interface_Map.cc
SRCS += src/game/Strategic/Map_Screen_Interface_Map_Inventory.cc
SRCS += src/game/Strategic/Map_Screen_Interface_TownMine_Info.cc
SRCS += src/game/Strategic/Meanwhile.cc
SRCS += src/game/Strategic/Merc_Contract.cc
SRCS += src/game/Strategic/Player_Command.cc
SRCS += src/game/Strategic/PreBattle_Interface.cc
SRCS += src/game/Strategic/Queen_Command.cc
SRCS += src/game/Strategic/QuestText.cc
SRCS += src/game/Strategic/Quest_Debug_System.cc
SRCS += src/game/Strategic/Quests.cc
SRCS += src/game/Strategic/Scheduling.cc
SRCS += src/game/Strategic/Strategic.cc
SRCS += src/game/Strategic/StrategicMap.cc
SRCS += src/game/Strategic/Strategic_AI.cc
SRCS += src/game/Strategic/Strategic_Event_Handler.cc
SRCS += src/game/Strategic/Strategic_Merc_Handler.cc
SRCS += src/game/Strategic/Strategic_Mines.cc
SRCS += src/game/Strategic/Strategic_Movement.cc
SRCS += src/game/Strategic/Strategic_Movement_Costs.cc
SRCS += src/game/Strategic/Strategic_Pathing.cc
SRCS += src/game/Strategic/Strategic_Status.cc
SRCS += src/game/Strategic/Strategic_Town_Loyalty.cc
SRCS += src/game/Strategic/Strategic_Turns.cc
SRCS += src/game/Strategic/Town_Militia.cc
SRCS += src/game/Sys_Globals.cc
SRCS += src/game/Tactical/Air_Raid.cc
SRCS += src/game/Tactical/Animation_Cache.cc
SRCS += src/game/Tactical/Animation_Control.cc
SRCS += src/game/Tactical/Animation_Data.cc
SRCS += src/game/Tactical/ArmsDealerInvInit.cc
SRCS += src/game/Tactical/Arms_Dealer_Init.cc
SRCS += src/game/Tactical/Auto_Bandage.cc
SRCS += src/game/Tactical/Boxing.cc
SRCS += src/game/Tactical/Bullets.cc
SRCS += src/game/Tactical/Campaign.cc
SRCS += src/game/Tactical/Civ_Quotes.cc
SRCS += src/game/Tactical/Dialogue_Control.cc
SRCS += src/game/Tactical/DisplayCover.cc
SRCS += src/game/Tactical/Drugs_And_Alcohol.cc
SRCS += src/game/Tactical/End_Game.cc
SRCS += src/game/Tactical/Enemy_Soldier_Save.cc
SRCS += src/game/Tactical/FOV.cc
SRCS += src/game/Tactical/Faces.cc
SRCS += src/game/Tactical/Gap.cc
SRCS += src/game/Tactical/Handle_Doors.cc
SRCS += src/game/Tactical/Handle_Items.cc
SRCS += src/game/Tactical/Handle_UI.cc
SRCS += src/game/Tactical/Interface.cc
SRCS += src/game/Tactical/Interface_Control.cc
SRCS += src/game/Tactical/Interface_Cursors.cc
SRCS += src/game/Tactical/Interface_Dialogue.cc
SRCS += src/game/Tactical/Interface_Items.cc
SRCS += src/game/Tactical/Interface_Panels.cc
SRCS += src/game/Tactical/Interface_Utils.cc
SRCS += src/game/Tactical/Inventory_Choosing.cc
SRCS += src/game/Tactical/Items.cc
SRCS += src/game/Tactical/Keys.cc
SRCS += src/game/Tactical/LOS.cc
SRCS += src/game/Tactical/LoadSaveBullet.cc
SRCS += src/game/Tactical/LoadSaveMercProfile.cc
SRCS += src/game/Tactical/LoadSaveObjectType.cc
SRCS += src/game/Tactical/LoadSaveRottingCorpse.cc
SRCS += src/game/Tactical/LoadSaveSoldierCreate.cc
SRCS += src/game/Tactical/LoadSaveSoldierType.cc
SRCS += src/game/Tactical/LoadSaveVehicleType.cc
SRCS += src/game/Tactical/Map_Information.cc
SRCS += src/game/Tactical/Merc_Entering.cc
SRCS += src/game/Tactical/Merc_Hiring.cc
SRCS += src/game/Tactical/Militia_Control.cc
SRCS += src/game/Tactical/Morale.cc
SRCS += src/game/Tactical/OppList.cc
SRCS += src/game/Tactical/Overhead.cc
SRCS += src/game/Tactical/PathAI.cc
SRCS += src/game/Tactical/Points.cc
SRCS += src/game/Tactical/QArray.cc
SRCS += src/game/Tactical/Real_Time_Input.cc
SRCS += src/game/Tactical/Rotting_Corpses.cc
SRCS += src/game/Tactical/ShopKeeper_Interface.cc
SRCS += src/game/Tactical/SkillCheck.cc
SRCS += src/game/Tactical/Soldier_Add.cc
SRCS += src/game/Tactical/Soldier_Ani.cc
SRCS += src/game/Tactical/Soldier_Control.cc
SRCS += src/game/Tactical/Soldier_Create.cc
SRCS += src/game/Tactical/Soldier_Find.cc
SRCS += src/game/Tactical/Soldier_Init_List.cc
SRCS += src/game/Tactical/Soldier_Profile.cc
SRCS += src/game/Tactical/Soldier_Tile.cc
SRCS += src/game/Tactical/Spread_Burst.cc
SRCS += src/game/Tactical/Squads.cc
SRCS += src/game/Tactical/Strategic_Exit_GUI.cc
SRCS += src/game/Tactical/Structure_Wrap.cc
SRCS += src/game/Tactical/Tactical_Save.cc
SRCS += src/game/Tactical/Tactical_Turns.cc
SRCS += src/game/Tactical/TeamTurns.cc
SRCS += src/game/Tactical/Turn_Based_Input.cc
SRCS += src/game/Tactical/UI_Cursors.cc
SRCS += src/game/Tactical/Vehicles.cc
SRCS += src/game/Tactical/Weapons.cc
SRCS += src/game/Tactical/World_Items.cc
SRCS += src/game/TacticalAI/AIList.cc
SRCS += src/game/TacticalAI/AIMain.cc
SRCS += src/game/TacticalAI/AIUtils.cc
SRCS += src/game/TacticalAI/Attacks.cc
SRCS += src/game/TacticalAI/CreatureDecideAction.cc
SRCS += src/game/TacticalAI/DecideAction.cc
SRCS += src/game/TacticalAI/FindLocations.cc
SRCS += src/game/TacticalAI/Knowledge.cc
SRCS += src/game/TacticalAI/Medical.cc
SRCS += src/game/TacticalAI/Movement.cc
SRCS += src/game/TacticalAI/NPC.cc
SRCS += src/game/TacticalAI/PanicButtons.cc
SRCS += src/game/TacticalAI/Realtime.cc
SRCS += src/game/TileEngine/Ambient_Control.cc
SRCS += src/game/TileEngine/Buildings.cc
SRCS += src/game/TileEngine/Environment.cc
SRCS += src/game/TileEngine/Exit_Grids.cc
SRCS += src/game/TileEngine/Explosion_Control.cc
SRCS += src/game/TileEngine/Fog_Of_War.cc
SRCS += src/game/TileEngine/Interactive_Tiles.cc
SRCS += src/game/TileEngine/Isometric_Utils.cc
SRCS += src/game/TileEngine/LightEffects.cc
SRCS += src/game/TileEngine/Lighting.cc
SRCS += src/game/TileEngine/LoadSaveExplosionType.cc
SRCS += src/game/TileEngine/LoadSaveLightEffect.cc
SRCS += src/game/TileEngine/LoadSaveLightSprite.cc
SRCS += src/game/TileEngine/LoadSaveRealObject.cc
SRCS += src/game/TileEngine/LoadSaveSmokeEffect.cc
SRCS += src/game/TileEngine/Map_Edgepoints.cc
SRCS += src/game/TileEngine/Overhead_Map.cc
SRCS += src/game/TileEngine/Phys_Math.cc
SRCS += src/game/TileEngine/Physics.cc
SRCS += src/game/TileEngine/Pits.cc
SRCS += src/game/TileEngine/Radar_Screen.cc
SRCS += src/game/TileEngine/RenderWorld.cc
SRCS += src/game/TileEngine/Render_Dirty.cc
SRCS += src/game/TileEngine/Render_Fun.cc
SRCS += src/game/TileEngine/SaveLoadMap.cc
SRCS += src/game/TileEngine/Simple_Render_Utils.cc
SRCS += src/game/TileEngine/Smell.cc
SRCS += src/game/TileEngine/SmokeEffects.cc
SRCS += src/game/TileEngine/Structure.cc
SRCS += src/game/TileEngine/SysUtil.cc
SRCS += src/game/TileEngine/Tactical_Placement_GUI.cc
SRCS += src/game/TileEngine/TileDat.cc
SRCS += src/game/TileEngine/TileDef.cc
SRCS += src/game/TileEngine/Tile_Animation.cc
SRCS += src/game/TileEngine/Tile_Cache.cc
SRCS += src/game/TileEngine/Tile_Surface.cc
SRCS += src/game/TileEngine/WorldDat.cc
SRCS += src/game/TileEngine/WorldDef.cc
SRCS += src/game/TileEngine/WorldMan.cc
SRCS += src/game/Utils/Animated_ProgressBar.cc
SRCS += src/game/Utils/Cinematics.cc
SRCS += src/game/Utils/Cursors.cc
SRCS += src/game/Utils/Debug_Control.cc
SRCS += src/game/Utils/Encrypted_File.cc
SRCS += src/game/Utils/Event_Manager.cc
SRCS += src/game/Utils/Event_Pump.cc
SRCS += src/game/Utils/Font_Control.cc
SRCS += src/game/Utils/MapUtility.cc
SRCS += src/game/Utils/MercTextBox.cc
SRCS += src/game/Utils/Message.cc
SRCS += src/game/Utils/Multi_Language_Graphic_Utils.cc
SRCS += src/game/Utils/Music_Control.cc
SRCS += src/game/Utils/PopUpBox.cc
SRCS += src/game/Utils/Quantize.cc
SRCS += src/game/Utils/STIConvert.cc
SRCS += src/game/Utils/Slider.cc
SRCS += src/game/Utils/Sound_Control.cc
SRCS += src/game/Utils/Text.cc
SRCS += src/game/Utils/Text_Input.cc
SRCS += src/game/Utils/Text_Utils.cc
SRCS += src/game/Utils/Timer_Control.cc
SRCS += src/game/Utils/Utilities.cc
SRCS += src/game/Utils/WordWrap.cc
SRCS += src/game/Utils/_DutchText.cc
SRCS += src/game/Utils/_EnglishText.cc
SRCS += src/game/Utils/_FrenchText.cc
SRCS += src/game/Utils/_GermanText.cc
SRCS += src/game/Utils/_ItalianText.cc
SRCS += src/game/Utils/_JA25EnglishText.cc
SRCS += src/game/Utils/_JA25GermanText.cc
SRCS += src/game/Utils/_JA25RussianText.cc
SRCS += src/game/Utils/_PolishText.cc
SRCS += src/game/Utils/_RussianText.cc

ifdef JA2EDITOR
SRCS += src/game/Editor/Cursor_Modes.cc
SRCS += src/game/Editor/EditScreen.cc
SRCS += src/game/Editor/Edit_Sys.cc
SRCS += src/game/Editor/EditorBuildings.cc
SRCS += src/game/Editor/EditorItems.cc
SRCS += src/game/Editor/EditorMapInfo.cc
SRCS += src/game/Editor/EditorMercs.cc
SRCS += src/game/Editor/EditorTerrain.cc
SRCS += src/game/Editor/Editor_Callbacks.cc
SRCS += src/game/Editor/Editor_Modes.cc
SRCS += src/game/Editor/Editor_Taskbar_Creation.cc
SRCS += src/game/Editor/Editor_Taskbar_Utils.cc
SRCS += src/game/Editor/Editor_Undo.cc
SRCS += src/game/Editor/Item_Statistics.cc
SRCS += src/game/Editor/LoadScreen.cc
SRCS += src/game/Editor/MessageBox.cc
SRCS += src/game/Editor/NewSmooth.cc
SRCS += src/game/Editor/PopupMenu.cc
SRCS += src/game/Editor/Road_Smoothing.cc
SRCS += src/game/Editor/Sector_Summary.cc
SRCS += src/game/Editor/SelectWin.cc
SRCS += src/game/Editor/SmartMethod.cc
SRCS += src/game/Editor/Smooth.cc
SRCS += src/game/Editor/Smoothing_Utils.cc
endif

SRCS := $(sort $(SRCS))
OBJS := $(patsubst %, $(BUILDDIR)/%.o, $(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
DIRS := $(sort $(dir $(OBJS)))

# Make build directories
DUMMY := $(shell mkdir -p $(DIRS))

Q ?= @

all: $(BUILDDIR)/$(BINARY)

-include $(DEPS)

$(BUILDDIR)/$(BINARY): $(OBJS)
	@echo '===> LD $@'
	$(Q)$(CXX) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.c
	@echo '===> CC $<'
	$(Q)$(CC) $(CCFLAGS) -c -MP -MMD -o $@ $<

$(BUILDDIR)/%.o: %.cc
	@echo '===> CXX $<'
	$(Q)$(CXX) $(CXXFLAGS) -c -MP -MMD -o $@ $<

clean distclean:
	@echo '===> CLEAN'
	$(Q)rm -fr $(BUILDDIR)

install: $(BUILDDIR)/$(BINARY)
	@echo '===> INSTALL'
	$(Q)$(INSTALL) -d $(PREFIX)/bin $(MANPREFIX)/man/man6 $(PREFIX)/share/applications $(PREFIX)/share/pixmaps
	$(Q)$(INSTALL_PROGRAM) $(BUILDDIR)/$(BINARY) $(PREFIX)/bin
	$(Q)$(INSTALL_MAN) ja2.6 $(MANPREFIX)/man/man6
	$(Q)$(INSTALL_DATA) ja2-stracciatella.desktop $(PREFIX)/share/applications
	$(Q)$(INSTALL_DATA) src/Res/jagged3.ico $(PREFIX)/share/pixmaps/jagged2.ico

deinstall:
	@echo '===> DEINSTALL'
	$(Q)rm $(PREFIX)/bin/$(BINARY)
	$(Q)rm $(MANPREFIX)/man/man6/ja2.6
	$(Q)rm $(PREFIX)/share/applications/ja2-stracciatella.desktop
	$(Q)rm $(PREFIX)/share/pixmaps/jagged2.ico


lowercase:
	$(Q)for i in \
		"$(SGPDATADIR)"/Data/*.[Ss][Ll][Ff] \
		"$(SGPDATADIR)"/Data/TILECACHE/*.[Jj][Ss][Dd] \
		"$(SGPDATADIR)"/Data/TILECACHE/*.[Ss][Tt][Ii]; \
	do \
		lower="`dirname "$$i"`/`basename "$$i" | LANG=C tr '[A-Z]' '[a-z]'`"; \
		[ "$$i" = "$$lower" ] || mv "$$i" "$$lower"; \
	done
