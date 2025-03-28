#ifndef HASHTAGCOMPLETER_H
#define HASHTAGCOMPLETER_H

#include <QCompleter>
#include <QStringListModel>

class HashtagCompleter : public QCompleter {
	Q_OBJECT
public:
	HashtagCompleter(QStringListModel* items, QObject* parent = nullptr) : QCompleter(items, parent) {}

	~HashtagCompleter() override = default;

	QString pathFromIndex(const QModelIndex& index) const override;
	QStringList splitPath(const QString& path) const override;
};

#endif	// HASHTAGCOMPLETER_H
