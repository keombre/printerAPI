#include "htmltoprinter.h"


HTMLToPrinter::HTMLToPrinter(bool debug, QString printer, bool pdf, QString margin)
    : m_page(new QTextDocument())
    , m_debug(debug)
{
    m_printer = new QPrinter(QPrinter::HighResolution);

    if (pdf)
    {
        m_printer->setOutputFormat(QPrinter::PdfFormat);
        m_printer->setOutputFileName(printer);
    } else if (!printer.trimmed().isEmpty())
        m_printer->setPrinterName(printer);

    m_printer->setPageSize(QPageSize(QPageSize::A4));
    if (margin.toLower() != "default")
    {
        QMarginsF margins = parseMargin(margin);
        m_printer->setFullPage(true);
        m_printer->setPageMargins(margins, QPageLayout::Point);
        m_page->setDocumentMargin(0);
    }

    m_page->setPageSize(m_printer->paperSize(QPrinter::Point));

    if (m_debug)
        QTextStream(stdout) << "Document margin: " << m_page->documentMargin() << "\n"
                            << "Page size: " << m_printer->pageRect().width() << ":" << m_printer->pageRect().height() << "\n";
}

HTMLToPrinter::~HTMLToPrinter()
{
    delete m_printer;
    delete m_page;
}

QMarginsF HTMLToPrinter::parseMargin(QString margin) const
{
    if (margin.trimmed().isEmpty())
        return QMarginsF(0, 0, 0, 0);
    else
    {
        QStringList list = margin.split(",", QString::SkipEmptyParts);
        if (list.length() != 4)
            return QMarginsF(0, 0, 0, 0);
        else
        {
            QList<unsigned int> r;
            for (int i = 0; i < 4; i++)
            {
                bool converted;
                unsigned int n = list[i].toUInt(&converted);
                if (!converted)
                    return QMarginsF(0, 0, 0, 0);
                r[i] = n;
            }
            return QMarginsF(r[3], r[0], r[1], r[2]);
        }
    }
}

bool HTMLToPrinter::loadFile(QString path, QString & data)
{
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly)) {
        QTextStream(stderr) << file.errorString() << "\n";
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    while(!in.atEnd())
        data.append(in.readLine());

    file.close();
    return true;
}

bool HTMLToPrinter::loadHTML(QString path)
{
    QString text;
    if (!loadFile(path, text))
        return false;

    if (m_debug)
        QTextStream(stdout) << "Loaded HTML: " << text << "\n";

    m_page->setHtml(text);

    return true;
}

bool HTMLToPrinter::loadCSS(QString path)
{
    QString text;
    if (!loadFile(path, text))
        return false;

    if (m_debug)
        QTextStream(stdout) << "Loaded CSS: " << text << "\n";

    m_page->setDefaultStyleSheet(text);

    return true;
}

void HTMLToPrinter::print()
{
    QTextStream(stdout) << "Sending file to printer " << QPrinterInfo(*m_printer).printerName() << "\n";
    m_page->print(m_printer);
    QTextStream(stdout) << "File sent\n";
}

QTextDocument * HTMLToPrinter::getDocument() const
{
    return m_page;
}
