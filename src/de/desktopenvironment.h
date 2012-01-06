#ifndef DESKTOPENVIRONMENT_H_
#define DESKTOPENVIRONMENT_H_

#include <QtCore/QString>
#include "../filesystem/tools/filesystemfileinfo.h"


class DesktopEnvironment
{
public:
	enum Type
	{
		DE_Cde,
		DE_Kde,
		DE_4Dwm,
		DE_Gnome,
		DE_Unknown
	};

public:
	DesktopEnvironment();
	~DesktopEnvironment();

	Type type() const { return m_type; }
	FileSystem::FileInfo info(const QString &absoluteFilePath) const;

private:
	Type m_type;
#if defined(DESKTOP_ENVIRONMENT_IS_KDE)
	int m_version;
#endif
};

#endif /* DESKTOPENVIRONMENT_H_ */