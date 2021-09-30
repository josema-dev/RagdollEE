/******************************************************************************/
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

};
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
