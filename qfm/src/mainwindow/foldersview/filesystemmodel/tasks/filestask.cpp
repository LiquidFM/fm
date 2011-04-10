#include "filestask.h"
#include "../../../../application.h"
#ifndef Q_OS_WIN
#  include <unistd.h>
#  include <sys/types.h>
#endif


FilesTask::FilesTask(Params *parameters) :
	m_params(parameters),
	m_handler(new DeleteHandler(this, m_params->receiver))
#ifndef Q_OS_WIN
    ,m_userId(getuid())
    ,m_groupId(getgid())
#endif
{
	Q_ASSERT(m_params);
	Q_ASSERT(m_params->fileSystemTree);
	Q_ASSERT(m_params->receiver);
	Q_ASSERT(m_params->receiver->thread() == QThread::currentThread());
}

FilesTask::~FilesTask()
{
	QMutexLocker locker(&m_params->m_mutex);

	if (m_handler)
	{
		m_handler->m_task = 0;
		m_handler->deleteLater();
	}
}

FileSystemInfo FilesTask::info(const QFileInfo &fileInfo)
{
	IconProvider &iconProvider = Application::instance()->iconProvider();
	iconProvider.lock();

	FileSystemInfo info(fileInfo);
    info.setIcon(iconProvider.iconProvider()->icon(fileInfo));
#ifdef Q_OS_WIN
    info.setPermissions(fileInfo.permissions());
#else
    info.setPermissions(translatePermissions(fileInfo, getuid(), getgid()));
#endif
    info.setDisplayType(iconProvider.iconProvider()->type(fileInfo));

    iconProvider.unlock();
    return info;
}

FileSystemInfo FilesTask::getInfo(const QFileInfo &fileInfo) const
{
	IconProvider &iconProvider = Application::instance()->iconProvider();

    FileSystemInfo info(fileInfo);
    info.setIcon(iconProvider.iconProvider()->icon(fileInfo));
#ifdef Q_OS_WIN
    info.setPermissions(fileInfo.permissions());
#else
    info.setPermissions(translatePermissions(fileInfo, m_userId, m_groupId));
#endif
    info.setDisplayType(iconProvider.iconProvider()->type(fileInfo));

    return info;
}

#ifndef Q_OS_WIN
QFile::Permissions FilesTask::translatePermissions(const QFileInfo &fileInfo, uint userId, uint groupId)
{
    QFile::Permissions permissions = fileInfo.permissions();
    QFile::Permissions p = permissions;
    p &= ~(QFile::ReadUser|QFile::WriteUser|QFile::ExeUser);

    if (permissions & QFile::ReadOther || (fileInfo.ownerId() == userId  && permissions & QFile::ReadOwner) ||
										  (fileInfo.groupId() == groupId && permissions & QFile::ReadGroup))
        p |= QFile::ReadUser;

    if (permissions & QFile::WriteOther || (fileInfo.ownerId() == userId  && permissions & QFile::WriteOwner) ||
										   (fileInfo.groupId() == groupId && permissions & QFile::WriteGroup))
        p |= QFile::WriteUser;

    if (permissions & QFile::ExeOther || (fileInfo.ownerId() == userId  && permissions & QFile::ExeOwner) ||
										 (fileInfo.groupId() == groupId && permissions & QFile::ExeGroup))
        p |= QFile::ExeUser;

    return p;
}
#endif