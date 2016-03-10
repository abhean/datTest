#ifndef BASE_SPATIAL_DYNAMICAABBTREE_
#define BASE_SPATIAL_DYNAMICAABBTREE_

// Dynamic AABB Tree
// @NOTE[egarcia]: Based on Bullet/Box2D implementations and:
// http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/
// http://allenchou.net/2014/02/game-physics-broadphase-dynamic-aabb-tree/
#include "Base/spatial/AABox2D.h"
#include "Base/global/RntVector.h"
#include "Base/global/DebugDefines.h"

namespace base 
{
namespace spatial
{

class CDynamicAABBTree
{
public:
	
	// Types
	typedef unsigned TNodeIndex;
	typedef void* TUserData;

	// Constants
	static TNodeIndex const uINVALID_INDEX = static_cast<TNodeIndex>(-1);

	// Initialization / destruction
	CDynamicAABBTree(unsigned _uMaxNodes);
	~CDynamicAABBTree();

	// Insertion/Removing/Update
	TNodeIndex Insert(base::spatial::CAABox2D const& _rAABB, TUserData const& _rUserData);

	// Notifications from user data
	void OnUserDataMoved(TNodeIndex _uNodeIndex, base::math::CVector2 const& _vDelta);
	void OnUserDataAABBModified(TNodeIndex _uNodeIndex);

	// Queries

private:

	// ================================================
	// Auxiliary types
	// ================================================

	// STRUCT: SNode
	// Size 32 bytes
	struct SNode
	{
		// Initialization / destruction
		void InitAsLeafNode(TNodeIndex const _uParentIdx, base::spatial::CAABox2D const& _rAABB, TUserData const& _rUserData);
		void InitAsInnerNode(TNodeIndex const _uParentIdx, base::spatial::CAABox2D const& _rAABB, TNodeIndex const _uLeftChildIdx, TNodeIndex const _uRightChildIdx);
		void InitAsFreeListNode(TNodeIndex const _uNextNodeIdx);

		// Getters
		bool IsLeaf() const { return auChildrenIdxs[0] == uINVALID_INDEX;  }

		//-------------------------------------------
		// Variables
		//-------------------------------------------

		// AABB
		base::spatial::CAABox2D oAABB;

		// Parent / Free list idx
		union
		{
			TNodeIndex uParentIdx;
			TNodeIndex uFreeListNextIdx; // free list
		};

		// Children indices / user data
		union
		{
			TNodeIndex auChildrenIdxs[2];
			void* pUserData;
		};
		STATIC_ASSERT(sizeof(TNodeIndex) == sizeof(void*), "size of TNodeIndex must be equal to void*");
	};
	STATIC_ASSERT(sizeof(SNode) <= 32, "Check SNode size");

	// ================================================
	// Nodes Methods
	// ================================================
	TNodeIndex GetNodeIndex(SNode const* _pNode) { return _pNode ? _pNode - m_vNodes.data() : NULL; }
	SNode* GetNode(TNodeIndex const _uIndex) { return _uIndex != uINVALID_INDEX ? &m_vNodes[_uIndex] : NULL; }

	// ================================================
	// Tree Update
	// ================================================
	void InsertLeaf(TNodeIndex _uLeafNodeIdx);

	// ================================================
	// Free List Management Methods
	// ================================================
	void InitFreeList(TNodeIndex _uFirstFreeNodeIdx);

	TNodeIndex CreateNode();
	void DestroyNode(TNodeIndex _uNodeIndex);

	TNodeIndex CreateLeafNode(base::spatial::CAABox2D const & _rAABB, TUserData const & _rUserData);

	// ================================================
	// Variables
	// ================================================
	base::global::CRntVector<SNode> m_vNodes;

	TNodeIndex m_uRootIdx;
	TNodeIndex m_uFreeListFirstIdx;
};

} // namespace spatial
} // namespace base


#endif // BASE_SPATIAL_DYNAMICAABBTREE_