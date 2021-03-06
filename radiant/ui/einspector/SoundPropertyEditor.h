#ifndef SOUNDPROPERTYEDITOR_H_
#define SOUNDPROPERTYEDITOR_H_

#include "PropertyEditor.h"

namespace ui
{

/**
 * Property editor for selecting a sound shader.
 */
class SoundPropertyEditor
: public PropertyEditor
{
private:
	// Keyvalue to set
	std::string _key;

private:

	void _onBrowseButton();

public:

	// Default constructor for the map
	SoundPropertyEditor() { }

	// Main constructor
	SoundPropertyEditor(Entity* entity,
					    const std::string& name,
					    const std::string& options);

	// Clone method for virtual construction
	IPropertyEditorPtr createNew(Entity* entity,
								const std::string& name,
								const std::string& options)
	{
		return PropertyEditorPtr(
			new SoundPropertyEditor(entity, name, options)
		);
	}
};

}

#endif /*SOUNDPROPERTYEDITOR_H_*/
