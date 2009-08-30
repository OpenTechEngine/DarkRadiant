#include "GenericEntityNode.h"

#include "math/frustum.h"

namespace entity {

GenericEntityNode::GenericEntityNode(const IEntityClassConstPtr& eclass) :
	EntityNode(eclass),
	m_contained(*this, 
		Node::TransformChangedCaller(*this))
{}

GenericEntityNode::GenericEntityNode(const GenericEntityNode& other) :
	EntityNode(other),
	Snappable(other),
	SelectionTestable(other),
	Cullable(other),
	Bounded(other),
	m_contained(other.m_contained, 
		*this, 
		Node::TransformChangedCaller(*this))
{}

void GenericEntityNode::construct()
{
	m_contained.construct();
}

// Snappable implementation
void GenericEntityNode::snapto(float snap) {
	m_contained.snapto(snap);
}

// EntityNode implementation
Entity& GenericEntityNode::getEntity() {
	return _entity;
}

void GenericEntityNode::refreshModel() {
	// nothing to do
}

// Bounded implementation
const AABB& GenericEntityNode::localAABB() const {
	return m_contained.localAABB();
}

// Cullable implementation
VolumeIntersectionValue GenericEntityNode::intersectVolume(
    const VolumeTest& test, const Matrix4& localToWorld) const
{
	return m_contained.intersectVolume(test, localToWorld);
}

void GenericEntityNode::testSelect(Selector& selector, SelectionTest& test) {
	m_contained.testSelect(selector, test, localToWorld());
}

scene::INodePtr GenericEntityNode::clone() const
{
	GenericEntityNodePtr node(new GenericEntityNode(*this));
	node->construct();

	return node;
}

void GenericEntityNode::renderSolid(RenderableCollector& collector, const VolumeTest& volume) const
{
	EntityNode::renderSolid(collector, volume);

	m_contained.renderSolid(collector, volume, localToWorld());
}

void GenericEntityNode::renderWireframe(RenderableCollector& collector, const VolumeTest& volume) const
{
	EntityNode::renderWireframe(collector, volume);

	m_contained.renderWireframe(collector, volume, localToWorld());
}

void GenericEntityNode::_onTransformationChanged()
{
	if (getType() == TRANSFORM_PRIMITIVE)
	{
		m_contained.revertTransform();
		
		m_contained.translate(getTranslation());
		m_contained.rotate(getRotation());

		m_contained.updateTransform();
	}
}

void GenericEntityNode::_applyTransformation()
{
	if (getType() == TRANSFORM_PRIMITIVE)
	{
		m_contained.revertTransform();
		
		m_contained.translate(getTranslation());
		m_contained.rotate(getRotation());

		m_contained.freezeTransform();
	}
}

} // namespace entity
