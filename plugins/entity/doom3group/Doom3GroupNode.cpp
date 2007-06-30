#include "Doom3GroupNode.h"

#include "Doom3GroupInstance.h"

namespace entity {

Doom3GroupNode::Doom3GroupNode(IEntityClassPtr eclass) :
	m_contained(
		eclass,
		*this,
		this, // Pass <this> as scene::Traversable::Observer* 
		InstanceSet::TransformChangedCaller(m_instances),
		InstanceSet::BoundsChangedCaller(m_instances),
		InstanceSetEvaluateTransform<Doom3GroupInstance>::Caller(m_instances)
	) 
{}

Doom3GroupNode::Doom3GroupNode(const Doom3GroupNode& other) :
	scene::Node(other),
	scene::Instantiable(other),
	scene::Cloneable(other),
	scene::Traversable::Observer(other),
	scene::GroupNode(other),
	Nameable(other),
	Snappable(other),
	TransformNode(other),
	scene::Traversable(other),
	EntityNode(other),
	Namespaced(other),
	ModelSkin(other),
	m_contained(
		other.m_contained,
		*this,
		this, // Pass <this> as scene::Traversable::Observer*
		InstanceSet::TransformChangedCaller(m_instances),
		InstanceSet::BoundsChangedCaller(m_instances),
		InstanceSetEvaluateTransform<Doom3GroupInstance>::Caller(m_instances)
	) 
{}

void Doom3GroupNode::addOriginToChildren() {
	m_contained.addOriginToChildren();
}

void Doom3GroupNode::removeOriginFromChildren() {
	m_contained.removeOriginFromChildren();
}

std::string Doom3GroupNode::name() const {
	return m_contained.getNameable().name();
}

void Doom3GroupNode::attach(const NameCallback& callback) {
	m_contained.getNameable().attach(callback);
}

void Doom3GroupNode::detach(const NameCallback& callback) {
	m_contained.getNameable().detach(callback);
}

scene::INodePtr Doom3GroupNode::clone() const {
	return scene::INodePtr(new Doom3GroupNode(*this));
}

void Doom3GroupNode::insertChild(scene::INodePtr child) {
	m_instances.insertChild(child);
}
void Doom3GroupNode::eraseChild(scene::INodePtr child) {
	m_instances.eraseChild(child);
}

scene::Instance* Doom3GroupNode::create(const scene::Path& path, scene::Instance* parent) {
	return new Doom3GroupInstance(path, parent, m_contained);
}

void Doom3GroupNode::forEachInstance(const scene::Instantiable::Visitor& visitor) {
	m_instances.forEachInstance(visitor);
}

void Doom3GroupNode::insert(const scene::Path& path, scene::Instance* instance) {
	m_instances.insert(path, instance);
}

scene::Instance* Doom3GroupNode::erase(const scene::Path& path) {
	return m_instances.erase(path);
}

void Doom3GroupNode::insert(scene::INodePtr node) {
	m_contained.getTraversable().insert(node);
}

void Doom3GroupNode::erase(scene::INodePtr node) {
	m_contained.getTraversable().erase(node);
}

void Doom3GroupNode::traverse(const Walker& walker) {
	m_contained.getTraversable().traverse(walker);
}

bool Doom3GroupNode::empty() const {
	return m_contained.getTraversable().empty();
}

// Snappable implementation
void Doom3GroupNode::snapto(float snap) {
	m_contained.snapto(snap);
}

// TransformNode implementation
const Matrix4& Doom3GroupNode::localToParent() const {
	return m_contained.getTransformNode().localToParent();
}

Entity& Doom3GroupNode::getEntity() {
	return m_contained.getEntity();
}

void Doom3GroupNode::setNamespace(INamespace& space) {
	m_contained.getNamespaced().setNamespace(space);
}

void Doom3GroupNode::attach(ModuleObserver& observer) {
	m_contained.getModelSkin().attach(observer);
}

void Doom3GroupNode::detach(ModuleObserver& observer) {
	m_contained.getModelSkin().detach(observer);
}

std::string Doom3GroupNode::getRemap(const std::string& name) const {
	return m_contained.getModelSkin().getRemap(name);
}

} // namespace entity
