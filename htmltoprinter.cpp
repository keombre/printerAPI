#include "htmltoprinter.h"


HTMLToPrinter::HTMLToPrinter(bool debug, QString printer)
    : m_page(new QTextDocument())
    , m_debug(debug)
{
    m_printer = new QPrinter(QPrinter::HighResolution);
    if (printer != "")
        m_printer->setPrinterName(printer);

    m_printer->setFullPage(true);
    m_printer->setPageMargins(QMarginsF(0, 0, 0, 0));
    m_printer->setPageSize(QPageSize(QPageSize::A4));

    m_page->setPageSize(m_printer->paperSize(QPrinter::Point));
    m_page->setDocumentMargin(0);

    if (m_debug)
        QTextStream(stdout) << "Document margin: " << m_page->documentMargin() << "\n"
                            << "Page size: " << m_printer->width() << ":" << m_printer->height() << "\n";
}

HTMLToPrinter::~HTMLToPrinter()
{
    delete m_printer;
    delete m_page;
}

bool HTMLToPrinter::loadFile(QString path, QString & data) {
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

bool HTMLToPrinter::loadHTML(QString path) {
    QString text;
    if (!loadFile(path, text))
        return false;

    if (m_debug)
        QTextStream(stdout) << "Loaded HTML: " << text << "\n";

    m_page->setHtml(text);

    return true;
}

bool HTMLToPrinter::loadCSS(QString path) {
    QString text;
    if (!loadFile(path, text))
        return false;

    if (m_debug)
        QTextStream(stdout) << "Loaded CSS: " << text << "\n";

    m_page->setDefaultStyleSheet(text);

    return true;
}

void HTMLToPrinter::print() {
    QTextStream(stdout) << "Sending file to printer " << QPrinterInfo(*m_printer).printerName() << "\n";
    m_page->print(m_printer);
    QTextStream(stdout) << "File sent\n";
}

QTextDocument * HTMLToPrinter::getDocument() const {
    return m_page;
}
