#ifndef FILESYSTEMIFILEOPERATIONS_H_
#define FILESYSTEMIFILEOPERATIONS_H_

#include <QtCore/QModelIndex>
#include "../filesystem_ns.h"


FILE_SYSTEM_NS_BEGIN

class Node;

class IFileOperations
{
public:
	virtual ~IFileOperations() {}

	virtual void remove(const QModelIndex &index) = 0;
	virtual void copy(const QModelIndex &index, Node *destination) = 0;
	virtual void move(const QModelIndex &index, Node *destination) = 0;
	virtual void createFolder(const QString &name) = 0;
	virtual void createFile(const QString &name) = 0;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMIFILEOPERATIONS_H_ */
