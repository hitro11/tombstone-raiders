
#ifndef __T2BOXCONTROLLER_H__
#define __T2BOXCONTROLLER_H__

#include "TSCharacter.h"
#include "TSNode.h"
#include "TSTriggers.h"
#include "MMCharacter.h"

namespace Tombstone {
enum {
    kControllerBox = 'GBOX'
};
}

namespace MMGame {
// Controllers only control a model or node. Or so it looks like.
// TODO: Will need to double check after done other assignmenTombstone and have free time to verify and develop minimal world.
// TODO: Build doxygen and see what I need to reimplement as a CharacterController subclass and different responsisbilities and things I can do to Nodes.
	// No idea how to add a Shape subnode for the CharacterController subclass...
class T2BoxController : public Tombstone::RigidBodyController {  // CharacterController is subclass of RigidBodyController is subclass of Controller.
 private:
    Tombstone::Transform4D originalTransform;
    T2BoxController(const T2BoxController& other);
 public:
    T2BoxController();
    ~T2BoxController();

    static bool ValidNode(const Tombstone::Node *node);

    void PreprocessController();
    void MoveController();

	void Pack(Tombstone::Packer& data, unsigned_int32 packFlags) const;
	void Unpack(Tombstone::Unpacker& data, unsigned_int32 unpackFlags);

    Tombstone::RigidBodyStatus HandleNewRigidBodyContact(const Tombstone::RigidBodyContact *contact, Tombstone::RigidBodyController *other);

};
}

#endif  // __T2BOXCONTROLLER_H__
