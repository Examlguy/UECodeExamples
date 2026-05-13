#include "MovementNodePathInterface.h"

bool IMovementNodePathInterface::GetIsNodeConnectedToPath_Implementation(AMovementNode* MovementNode)
{
	return false;
}

AMovementNode* IMovementNodePathInterface::GetNextNode_Implementation(AMovementNode* FromNode)
{
	return nullptr;
}

bool IMovementNodePathInterface::GetCanUsePath_Implementation()
{
	return true;
}
