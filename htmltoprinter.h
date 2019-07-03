#ifndef HTMLTOPRINTER_H
#define HTMLTOPRINTER_H

#include <QFile>
#include <QTextStream>
#include <QPrinter>
#include <QPrinterInfo>
#include <QTextDocument>
#include <QTextEdit>
#include <QPageSize>
#include <QSizeF>

#include <functional>

using namespace std;
using namespace std::placeholders;

class HTMLToPrinter : public QObject
{
    Q_OBJECT
public:
    explicit HTMLToPrinter(bool, QString, bool);
    ~HTMLToPrinter();

    bool loadHTML(QString);
    bool loadCSS(QString);
    void print();
    QTextDocument * getDocument() const;

private:
    QTextDocument * m_page;
    QString m_inputPath;
    QPrinter * m_printer;
    bool m_debug;
    bool loadFile(QString, QString&);
};

#endif // HTMLTOPRINTER_H
