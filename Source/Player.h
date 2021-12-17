#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "MyRagdoll.h"

class Player : public Game::Chr
{
public:
	MyRagdoll ragdoll;

	virtual void ragdollValidate() override;
	virtual void ragdollEnable() override;
	virtual void ragdollDisable() override;
	virtual Bool ragdollBlend() override;
	virtual Vec    pos() override;
	virtual void   pos(C Vec& pos) override;
	virtual void memoryAddressChanged() override;
	virtual Bool update() override;
	virtual void disable() override;
	virtual void  enable() override;
	virtual Bool save(File& f) override;
	virtual Bool load(File& f) override;
	void updateAnimation();

	static void AlignFeet(Chr& chr, Flt step); // align character feet by 'step'=0..1 factor
	static Flt AnimTime(Chr& chr); // get global animation time for the character according to current time and character's 'unique' value
};

#endif //__PLAYER_H__
