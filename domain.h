#include <set>
#include <vector>
#include <string>
#include <map>

enum EffectType {
	AddEffect,
	DeleteEffect
};

class Literal {
	public:
		Literal(std::string name);
		std::string getName();
	private:
		std::string mName;
};

class Effect {
	public:
		Effect(Literal* literal, EffectType type, bool known);
	private:
		Literal* mLiteral;
		EffectType mType;
		bool mKnown;
};

class Precondition {
	public:
		Precondition(Literal* literal, bool value, bool known);
	private:
		Literal* mLiteral;
		bool mValue; bool mKnown; };

class Action {
	public:
		Action(std::string name);
		void AddEffect(Effect* effect);
		void AddPrecondition(Precondition* precondition);
	private:
		std::string mName;
		std::vector<Effect*> mEffects;
		std::vector<Precondition*> mPreconditions;
};

class Domain {
	public:
		Domain();
		Domain(const char* filename);
		Literal* LookupLiteral(std::string name);
		Action* LookupAction(std::string name);
		void AddGoal(Precondition* goal);
		void InitialState(Effect* initialState);
	private:
		std::vector<Action*> mActions;
		std::vector<Precondition*> mGoals;
		std::vector<Effect*> mInitialState;
		std::map<std::string, Literal*> mLiteralDirectory;
		std::map<std::string, Action*> mActionDirectory;
};
