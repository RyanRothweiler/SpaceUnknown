namespace ability {

#define QUALITY_VALUES_COUNT_MAX 100
#define MAX_QUALITY 100

	struct quality_value {
		range Range;
		string ID;
	};

	struct definition {
		string ID;
		string Name;
		string Description;
		ability_type Type;
		int64 ManaCost;
		int64 Cooldown;

		quality_value QualityValues[QUALITY_VALUES_COUNT_MAX];
		int32 QualityValuesCount;

		battle_events Events;

		loaded_image Icon;
	};

	struct quality_value_resolved {
		int64 Value;
		string ID;
	};

	struct instance {

		int64 Quality;
		quality_value_resolved QualityValuesResolved[QUALITY_VALUES_COUNT_MAX];
		string DescriptionResolved;

		ability::definition* Definition;
		monster_instance* Holder;

		// TODO maybe change how this works? Should these support references be here?
		ability::instance* SupportAbilities[10];
		int32 SupportsCount;

		// TODO put this in a battle state struct
		int64 CooldownCurrent;
		bool32 Running;
		battle_monster* Destination;

		s_void BattleState;

		// Quality values for display
		int64 GetQualityValueDisplay(char* ID)
		{
			string IDStr = ID;

			for (int i = 0; i < Definition->QualityValuesCount; i++) {
				if (QualityValuesResolved[i].ID == IDStr) {
					return QualityValuesResolved[i].Value;
				}
			}

			// Couldn't find that ID
			Assert(0)
			return 0;
		}

		// Quality values used for ability battle calculations
		real64 GetQualityValueCalc(char* ID)
		{
			return GetQualityValueDisplay(ID) * 0.01f;
		}
	};

	ability::instance* Create(string ID, float Quality = -1);
}