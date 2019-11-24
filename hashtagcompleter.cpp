#include "hashtagcompleter.h"

#include <QLineEdit>

QString HashtagCompleter::pathFromIndex(const QModelIndex& index) const {
    QString path = QCompleter::pathFromIndex(index);
    QString text = static_cast<QLineEdit*>(widget())->text();

    int pos = text.lastIndexOf(' ');
    if (pos >= 0) path = text.left(pos) + " " + path;

    return path;
}

QStringList HashtagCompleter::splitPath(const QString& path) const {
    int pos = path.lastIndexOf(' ') + 1;

    //    while (pos < path.length() && path.at(pos) == QLatin1Char(' ')) pos++;

    return QStringList(path.mid(pos));
}
