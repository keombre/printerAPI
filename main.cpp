#include "rawprint.h"
#include "htmltoprinter.h"

#include <QApplication>
#include <QCommandLineParser>

bool checkPrinterStatus(QString printerName)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(printerName);
    auto state = QPrinterInfo(printer).state();
    if (state == QPrinter::Idle)
        return true;

    QTextStream(stdout) << "Printer " << printerName << " is not idle. Aborting.\n";
    return false;
}

bool htmlPrint(bool debug, QString printerName, bool preview, bool pdf, QString HTML, QString CSS = "")
{
    if (printerName.trimmed().isEmpty())
    {
        if (pdf)
        {
            QTextStream(stdout) << "Missing target file name\n";
            return false;
        }
        else
            printerName = QPrinterInfo::defaultPrinterName();
    }

    if (!checkPrinterStatus(printerName))
        return false;

    HTMLToPrinter htp(debug, printerName, pdf);

    if (CSS != "")
    {
        QTextStream(stdout) << "Loading CSS " << CSS << "\n";
        if (!htp.loadCSS(CSS))
            return false;
    }

    QTextStream(stdout) << "Loading HTML " << HTML << "\n";

    if (!htp.loadHTML(HTML))
        return false;

    if (preview) {
        QTextEdit textEdit;
        textEdit.setDocument(htp.getDocument());
        textEdit.show();
        return true;
    }

    htp.print();
    htp.~HTMLToPrinter();
    return true;
}

bool rawPrint(bool debug, QString pn, QString path)
{

    QString printerName = "";
    for (const auto & avaPr : QPrinterInfo::availablePrinters())
    {
        if (avaPr.printerName() == pn)
        {
            printerName = pn;
            break;
        }
    }
    if (printerName == "")
        printerName = QPrinterInfo::defaultPrinterName();

    if (!checkPrinterStatus(printerName))
        return false;

    return loadRaw(debug, path, printerName);
}

void PrintPrinterList()
{
    const QString states[] = {"Idle", "Active", "Aborted", "Error"};
    QTextStream(stdout) << "Default: '" << QPrinterInfo::defaultPrinterName() << "'\n";
    for (const auto & printer : QPrinterInfo::availablePrinters())
    {
        QTextStream(stdout) << printer.printerName() << ": " << states[printer.state()] << "\n";
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Keombre  Inc.");
    QCoreApplication::setApplicationName("printAPI");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QCoreApplication::translate("main", "Simple print connector."));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption listPrinters(QStringList() << "l" << "list",
            QCoreApplication::translate("main", "List names of available printers."));
    parser.addOption(listPrinters);

    QCommandLineOption printType(QStringList() << "t" << "type",
                QCoreApplication::translate("main", "Print type.\n  RAW: print file in raw format.\n  HTML: (implicit) format HTML and print visual\n  PDF: save to pdf (requires -p to specify file location)"),
                QCoreApplication::translate("main", "type"));
    parser.addOption(printType);

    QCommandLineOption debug(QStringList() << "d" << "debug",
            QCoreApplication::translate("main", "Be as verbose as possible."));
    parser.addOption(debug);

    QCommandLineOption preview(QStringList() << "preview",
            QCoreApplication::translate("main", "Preview html instead of printing. (has no effect if in RAW mode)"));
    parser.addOption(preview);

    parser.addPositionalArgument(
        QCoreApplication::translate("main", "FILE"),
        QCoreApplication::translate("main", "File name to be printed."));

    parser.addPositionalArgument(
        QCoreApplication::translate("main", "CSS"),
        QCoreApplication::translate("main", "Optional styles to format HTML. (has no effect if in RAW mode)"));

    QCommandLineOption printerName(QStringList() << "p" << "printer",
                QCoreApplication::translate("main", "Printer name. Default if empty."),
                QCoreApplication::translate("main", "printer"));
    parser.addOption(printerName);

    parser.process(app.arguments());

    // ---------------------------------

    if (parser.isSet(listPrinters))
    {
        PrintPrinterList();
        app.quit();
        return 0;
    }

    const QStringList requiredArguments = parser.positionalArguments();
    if (requiredArguments.size() < 1)
        parser.showHelp(1);

    bool pdf = false;

    if (parser.value(printType) == "RAW")
    {
        if (!rawPrint(parser.isSet(debug), parser.value(printerName), requiredArguments.at(0)))
        {
            app.quit();
            return 1;
        }
        else
        {
            app.quit();
            return 0;
        }
    }
    else if (parser.value(printType) == "PDF")
        pdf = true;

    bool printed = false;
    if (requiredArguments.size() == 2)
        printed = htmlPrint(
                    parser.isSet(debug),
                    parser.value(printerName),
                    parser.isSet(preview),
                    pdf,
                    requiredArguments.at(0),
                    requiredArguments.at(1));
    else
        printed = htmlPrint(
                    parser.isSet(debug),
                    parser.value(printerName),
                    parser.isSet(preview),
                    pdf,
                    requiredArguments.at(0));

    if (!printed)
    {
        app.quit();
        return 1;
    }
    else if (parser.isSet(preview))
        return app.exec();
    else
    {
        app.quit();
        return 0;
    }
}

// #include "main.moc"
