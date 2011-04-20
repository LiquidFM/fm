#include "settings.h"
#include "dialog/settingsdialog.h"
#include <QApplication>


Settings::Settings(const QString &title, const Argument &arg) :
	m_dialog(0),
	m_title(title),
	m_storage(QApplication::applicationDirPath().append(QString::fromLatin1("/settings.ini")), QSettings::IniFormat),
	m_settings(arg.first),
	m_constraints(arg.second)
{
	makeSettingsCache();
}

Settings::~Settings()
{
	qDeleteAll(m_settings);
	qDeleteAll(m_constraints);
}

QVariant Settings::readValue(const qint32 &settingId)
{
	QVariant value;
	CacheEntry cacheEntry;

	if ((cacheEntry = m_cache.value(settingId)).option != 0)
	{
		m_storage.beginGroup(cacheEntry.groupPath);

		if ((value = m_storage.value(cacheEntry.option->title())).isNull())
			m_storage.setValue(cacheEntry.option->title(), value = cacheEntry.option->defaultValue());

		m_storage.endGroup();
	}

	return value;
}

void Settings::exec(QWidget *parent)
{
	SettingsDialog dialog(m_title, m_settings, m_constraints, parent);
	readSettingsValues();
	m_dialog = &dialog;

	if (dialog.exec() == QDialog::Accepted)
		writeSettingsValues();
}

QVariant Settings::readValueFromEditor(const qint32 &settingId) const
{
	QVariant value;
	CacheEntry cacheEntry;

	if ((cacheEntry = m_cache.value(settingId)).option != 0)
		value = cacheEntry.option->editorValue();

	return value;
}

void Settings::makeSettingsCache()
{
	GroupPathCache groupPathCache;

	for (SettingsList::size_type i = 0, size = m_settings.size(); i < size; ++i)
		makeSettingsCache(m_settings.at(i)->title(), groupPathCache, m_settings.at(i));
}

void Settings::makeSettingsCache(const QString &groupPath, GroupPathCache &groupPathCache, ConfigOptionGroup *group)
{
	ConfigOption *option;

	for (ConfigOptionGroup::size_type i = 0, size = group->size(); i < size; ++i)
		if ((option = group->at(i))->type() == ConfigOption::Value)
		{
			Q_ASSERT_X(!m_cache.contains(static_cast<ConfigOptionValue*>(option)->id()), "Settings::makeSettingsCache", "Settings can not have same id");
			m_cache[static_cast<ConfigOptionValue*>(option)->id()] = CacheEntry(static_cast<ConfigOptionValue*>(option), groupPath);
		}
		else
			makeSettingsCache(QString(groupPath).append(QChar('.')).append(option->title()), groupPathCache, static_cast<ConfigOptionGroup*>(option));
}

void Settings::readSettingsValues()
{
	for (SettingsList::size_type i = 0, size = m_settings.size(); i < size; ++i)
		readSettingsValues(m_settings.at(i)->title(), m_settings.at(i));
}

void Settings::readSettingsValues(const QString &path, ConfigOptionGroup *group)
{
	QVariant value;
	ConfigOption *option;
	m_storage.beginGroup(path);

	for (ConfigOptionGroup::size_type i = 0, size = group->size(); i < size; ++i)
		if ((option = group->at(i))->type() == ConfigOption::Value)
		{
			if ((value = m_storage.value(option->title())).isNull())
				m_storage.setValue(option->title(), value = static_cast<ConfigOptionValue*>(option)->defaultValue());

			static_cast<ConfigOptionValue*>(option)->setLoadedEditorValue(value);
		}
		else
		{
			m_storage.endGroup();
			readSettingsValues(QString(path).append(QChar(L'.')).append(option->title()), static_cast<ConfigOptionGroup*>(option));
			m_storage.beginGroup(path);
		}

	m_storage.endGroup();
}

void Settings::writeSettingsValues()
{
	for (SettingsList::size_type i = 0, size = m_settings.size(); i < size; ++i)
		writeSettingsValues(m_settings.at(i)->title(), m_settings.at(i));
}

void Settings::writeSettingsValues(const QString &path, ConfigOptionGroup *group)
{
	ConfigOption *option;
	m_storage.beginGroup(path);

	for (ConfigOptionGroup::size_type i = 0, size = group->size(); i < size; ++i)
		if ((option = group->at(i))->type() == ConfigOption::Value)
			m_storage.setValue(option->title(), static_cast<ConfigOptionValue*>(option)->editorValue());
		else
		{
			m_storage.endGroup();
			writeSettingsValues(QString(path).append(QChar(L'.')).append(option->title()), static_cast<ConfigOptionGroup*>(option));
			m_storage.beginGroup(path);
		}

	m_storage.endGroup();
}