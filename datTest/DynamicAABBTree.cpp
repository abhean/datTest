#include "DynamicAABBTree.h"

namespace base 
{
namespace spatial
{

//----------------------------------------------------------------------------
CDynamicAABBTree::CDynamicAABBTree(unsigned const _uMaxNodes)
	: m_vNodes(_uMaxNodes)
	, m_uRootIdx(uINVALID_INDEX)
	, m_uFreeListFirstIdx(uINVALID_INDEX)
{
	InitFreeList(0u);
}

//----------------------------------------------------------------------------
CDynamicAABBTree::~CDynamicAABBTree()
{
}

//----------------------------------------------------------------------------
CDynamicAABBTree::TNodeIndex CDynamicAABBTree::Insert(base::spatial::CAABox2D const & _rAABB, TUserData const & _rUserData)
{
	TNodeIndex uNewNodeIdx = CreateUserDataNode(_rAABB, _rUserData);
	if (VERIFY(uNewNodeIdx))
	{
		InsertLeaf(uNewNodeIdx);
	}

	return uNewNodeIdx;
}

//----------------------------------------------------------------------------
void CDynamicAABBTree::InsertLeaf(TNodeIndex _uLeafNodeIdx)
{
	if (m_uRootIdx == uINVALID_INDEX)
	{
		m_uRootIdx = _uLeafNodeIdx;
	}
	else
	{
		SNode& rLeafNode = m_vNodes[_uLeafNodeIdx];

		SNode* pInsertionRoot = GetNode(m_uRootIdx);
		while (!pInsertionRoot->IsLeaf())
		{
			TNodeIndex auChildNodesIdxs[2] = { pInsertionRoot->auChildrenIdxs[0], pInsertionRoot->auChildrenIdxs[1] };
			SNode*	   apChildNodes[2] = { GetNode(auChildNodesIdxs[0]), GetNode(auChildNodesIdxs[1]) };

			unsigned uBestChildIdx = 0;//SelectBestChild(rLeafNode.oAABB, apChildNodes[0]->oAABB, apChildNodes[1]->oAABB);
			pInsertionRoot = apChildNodes[uBestChildIdx];
		}

		SNode* pPrevRoot = GetNode(pInsertionRoot->uParentIdx);
		SNode& rNewInnerNode = CreateInnerNode();

	}
}

//----------------------------------------------------------------------------
void CDynamicAABBTree::InitFreeList(TNodeIndex const _uFirstFreeNodeIdx)
{
	unsigned const uMaxNodes = m_vNodes.size();
	TNodeIndex uLastNodeIdx = uMaxNodes - 1;

	for (TNodeIndex uNodeIdx = _uFirstFreeNodeIdx; uNodeIdx < uLastNodeIdx; ++uNodeIdx)
	{
		m_vNodes[uNodeIdx].uFreeListNextIdx = uNodeIdx + 1;
	}

	m_vNodes[uLastNodeIdx].uFreeListNextIdx = uINVALID_INDEX;
	m_uFreeListFirstIdx = _uFirstFreeNodeIdx;
}

//----------------------------------------------------------------------------
CDynamicAABBTree::TNodeIndex CDynamicAABBTree::CreateNode()
{
	TNodeIndex uNodeIndex = m_uFreeListFirstIdx;

	if (m_uFreeListFirstIdx != uINVALID_INDEX)
	{
		m_uFreeListFirstIdx = m_vNodes[m_uFreeListFirstIdx].uFreeListNextIdx;
	}

	return uNodeIndex;
}

//----------------------------------------------------------------------------

CDynamicAABBTree::TNodeIndex CDynamicAABBTree::CreateLeafNode(base::spatial::CAABox2D const & _rAABB, TUserData const & _rUserData)
{
	TNodeIndex uNodeIndex = CreateNode();

	if (VERIFY(uNodeIndex != uINVALID_INDEX))
	{
		SNode& rNode = m_vNodes[uNodeIndex];
		rNode.InitAsLeafNode(uINVALID_INDEX, _rAABB, _rUserData);
	}

	return uNodeIndex;
}

//----------------------------------------------------------------------------

CDynamicAABBTree::TNodeIndex CDynamicAABBTree::CreateInnerNode(base::spatial::CAABox2D const & _rAABB)
{
	TNodeIndex uNodeIndex = CreateNode();

	if (VERIFY(uNodeIndex != uINVALID_INDEX))
	{
		SNode& rNode = m_vNodes[uNodeIndex];
		rNode.InitAsInnerNode(uINVALID_INDEX, _rAABB, _rUserData);
	}

	return uNodeIndex;
}

//----------------------------------------------------------------------------
void CDynamicAABBTree::DestroyNode(TNodeIndex _uNodeIndex)
{
	CHECK(_uNodeIndex != uINVALID_INDEX && _uNodeIndex < m_vNodes.size());

	SNode& rNode = m_vNodes[_uNodeIndex];
	rNode.InitAsFreeListNode(m_uFreeListFirstIdx);
	m_uFreeListFirstIdx = _uNodeIndex;
}

//----------------------------------------------------------------------------


} // namespace spatial
} // namespace base
