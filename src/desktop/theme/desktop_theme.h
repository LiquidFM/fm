#ifndef DESKTOP_THEME_H_
#define DESKTOP_THEME_H_

#include <QtGui/QIcon>
#include <QtCore/QByteArray>
#include "../desktop_ns.h"


DESKTOP_NS_BEGIN

class Theme
{
public:

#if defined(Q_OS_UNIX)
	enum Type
	{
		DE_Cde,
		DE_Kde,
		DE_4Dwm,
		DE_Gnome,
		DE_Unknown
	};
#elif defined(Q_OS_WIN32)
	enum Type
	{
		DE_Win32
	};
#endif

	enum IconSize
	{
		Small = 16
	};

public:
	Theme();
	~Theme();

	static Theme *current();

	const QByteArray &name() const { return m_name; }

	QIcon processingIcon(int iconSize = Small) const;
	QIcon cancelingIcon(int iconSize = Small) const;

	QIcon copyActionIcon(int iconSize = Small) const;
	QIcon cutActionIcon(int iconSize = Small) const;
	QIcon pasteActionIcon(int iconSize = Small) const;
	QIcon propertiesActionIcon(int iconSize = Small) const;

	QIcon packActionIcon(int iconSize = Small) const;
	QIcon unpackActionIcon(int iconSize = Small) const;

	QIcon searchIcon(int iconSize = Small) const;
	QIcon openDataIcon(int iconSize = Small) const;

	QIcon missingIcon(int iconSize = Small) const;

	QIcon driveIcon(const char *iconName, int iconSize = Small) const;
	QIcon driveIcon(const QByteArray &iconName, int iconSize = Small) const;
	QIcon driveHarddisk(int iconSize = Small) const;
	QIcon driveOptical(int iconSize = Small) const;
	QIcon driveRemovableMedia(int iconSize = Small) const;
	QIcon driveRemovableMediaUsb(int iconSize = Small) const;
	QIcon driveRemovableMediaUsbPendrive(int iconSize = Small) const;

	QIcon mediaFlashMemoryStick(int iconSize = Small) const;
	QIcon mediaFlashSdMmc(int iconSize = Small) const;
	QIcon mediaFlashSmartMedia(int iconSize = Small) const;
	QIcon mediaFlash(int iconSize = Small) const;
	QIcon mediaFloppy(int iconSize = Small) const;
	QIcon mediaOpticalAudio(int iconSize = Small) const;
	QIcon mediaOpticalBluRay(int iconSize = Small) const;
	QIcon mediaOpticalData(int iconSize = Small) const;
	QIcon mediaOpticalDvdVideo(int iconSize = Small) const;
	QIcon mediaOpticalDvd(int iconSize = Small) const;
	QIcon mediaOpticalMixedCd(int iconSize = Small) const;
	QIcon mediaOpticalRecordable(int iconSize = Small) const;
	QIcon mediaOpticalVideo(int iconSize = Small) const;
	QIcon mediaOptical(int iconSize = Small) const;
	QIcon mediaTape(int iconSize = Small) const;

private:
	Type m_type;
	QByteArray m_name;
};

DESKTOP_NS_END

#endif /* DESKTOP_THEME_H_ */
