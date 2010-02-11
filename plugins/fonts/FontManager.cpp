#include "FontManager.h"

#include "ifilesystem.h"
#include "ishaders.h"
#include "itextstream.h"
#include "iregistry.h"
#include "os/path.h"

#include "xmlutil/MissingXMLNodeException.h"
#include "generic/callback.h"

#include "FontLoader.h"

namespace fonts
{

namespace
{
	const char* MISSING_BASEPATH_NODE =
		"Failed to find \"/game/filesystem/fonts/basepath\" node \
in game descriptor";
	 
	const char* MISSING_EXTENSION_NODE =
		"Failed to find \"/game/filesystem/fonts/extension\" node \
in game descriptor";
}

FontManager::FontManager() :
	_curLanguage("english")
{}

const std::string& FontManager::getName() const
{
	static std::string _name(MODULE_FONTMANAGER);
	return _name;
}

const StringSet& FontManager::getDependencies() const
{
	static StringSet _dependencies;

	if (_dependencies.empty())
	{
		_dependencies.insert(MODULE_VIRTUALFILESYSTEM);
		_dependencies.insert(MODULE_XMLREGISTRY);
		_dependencies.insert(MODULE_SHADERSYSTEM);
	}

	return _dependencies;
}

void FontManager::initialiseModule(const ApplicationContext& ctx)
{
	globalOutputStream() << getName() << "::initialiseModule called" << std::endl;

	// Find installed fonts
	reloadFonts();
}

void FontManager::shutdownModule()
{
}

void FontManager::reloadFonts()
{
	_fonts.clear();

	xml::NodeList nlBasePath = GlobalRegistry().findXPath("game/filesystem/fonts/basepath");

	if (nlBasePath.empty())
	{
		throw xml::MissingXMLNodeException(MISSING_BASEPATH_NODE);
	}

	xml::NodeList nlExt = GlobalRegistry().findXPath("game/filesystem/fonts/extension");

	if (nlExt.empty())
	{
		throw xml::MissingXMLNodeException(MISSING_EXTENSION_NODE);
	}

	// Get the language from the registry
	_curLanguage = "english";

	// Load the DAT files from the VFS
	std::string path = os::standardPathWithSlash(nlBasePath[0].getContent()) + _curLanguage;
	std::string extension = nlExt[0].getContent();

	// Instantiate a visitor to traverse the VFS
	FontLoader loader(path, *this);
	GlobalFileSystem().forEachFile(path, extension, makeCallback1(loader), 2);

	globalOutputStream() << _fonts.size() << " fonts registered." << std::endl;
}

FontInfoPtr FontManager::findFontInfo(const std::string& name)
{
	FontMap::const_iterator found = _fonts.find(name);
	
	return (found != _fonts.end()) ? found->second: FontInfoPtr();
}

FontInfoPtr FontManager::findOrCreateFontInfo(const std::string& name)
{
	FontMap::iterator i = _fonts.find(name);
	
	if (i == _fonts.end())
	{
		FontInfoPtr font(new FontInfo(name, _curLanguage));

		// Doesn't exist yet, create now
		std::pair<FontMap::iterator, bool> result = _fonts.insert(
			FontMap::value_type(name, font)
		);

		i = result.first;
	}

	return i->second;
}

} // namespace fonts
