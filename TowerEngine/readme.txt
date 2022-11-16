Tower Engine

--------------------------------------------------------------------------------
TOOLS 
--------------------------------------------------------------------------------

Version.txt
	Format is Major.Minor.Patch.BuildNumber

Crossbow
	Tool to automate testing and releases. Used to create release builds, manage versions, packge data, etc


--------------------------------------------------------------------------------
Engine Modules
--------------------------------------------------------------------------------

EngineCore
	Core engine features. Anything that doesn't justify its own module. Connects all the modules together.


--------------------------------------------------------------------------------
FILE TYPES
--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
NOTES
--------------------------------------------------------------------------------

-	DAE export
		Armatures
		- Roll of bones in blender must be 180.
		- There must be one root bone called Root.
		- All vertices must be part of a bone.


- 	There needs to be a junction (mklink /j) added at T:\AndroidProject\asset_pack_1\src\main called assets
	That is why this folder is ignored by git, to avoid duplicating the entire assets directory

	mklink /j assets "C:\Digital Archive\Game Development\Active\MeepMorp\Game\assets"

- 	Drop Tables

	When exporting from excel use CSV (Comman delimited)

	Drop tables are in csv format. Columns are as such
		item id, drop weight, (excel calculated drop chance), min amount given, max amount given

	item id is a composite key
		{item_group}.{id within group}

		item_group options are
			stackable, ability, monster_blueprint, drop_table

		examples
			stackable.metal
			stackable.training_disk
			ability.meltdown

-	Signing 
	Keystore info is in 1Password.
	Copy T:\Game\build_scripts\sign_android TEMPLATE.bat to T:\Game\build_scripts\sign_android.bat. And fill out the missing keystore destination and passwords.
