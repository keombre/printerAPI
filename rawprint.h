#ifndef RAWPRINT_H
#define RAWPRINT_H

#include <Windows.h>
#include <winspool.h>
#include <QString>
#include <QFile>
#include <QTextStream>

// source: https://support.microsoft.com/cs-cz/help/138594/howto-send-raw-data-to-a-printer-by-using-the-win32-api
// RawDataToPrinter - sends binary data directly to a printer
//
// Params:
//   szPrinterName - NULL terminated string specifying printer name
//   lpData        - Pointer to raw data bytes
//   dwCount       - Length of lpData in bytes
//
// Returns: TRUE for success, FALSE for failure.
//
bool RawDataToPrinter(LPWSTR szPrinterName, LPBYTE lpData, DWORD dwCount, bool debug)
{
 HANDLE     hPrinter;
 DOC_INFO_1 DocInfo;
 DWORD      dwJob;
 DWORD      dwBytesWritten;
 if( ! OpenPrinter( szPrinterName, &hPrinter, nullptr ) ) {
     if (debug)
        QTextStream(stdout) << "Failed to init printer\n";
     return false;
 }
 DocInfo.pDocName = const_cast<LPWSTR>(L"Document");
 DocInfo.pOutputFile = nullptr;
 DocInfo.pDatatype = const_cast<LPWSTR>(L"RAW");
 if( (dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo)) == 0 )
 {
   ClosePrinter( hPrinter );
   if (debug)
       QTextStream(stdout) << "Failed to create document\n";
   return false;
 }
 if( ! StartPagePrinter( hPrinter ) )
 {
   EndDocPrinter( hPrinter );
   ClosePrinter( hPrinter );
   if (debug)
       QTextStream(stdout) << "Failed to start page\n";
   return false;
 }
 if( ! WritePrinter( hPrinter, lpData, dwCount, &dwBytesWritten ) )
 {
   EndPagePrinter( hPrinter );
   EndDocPrinter( hPrinter );
   ClosePrinter( hPrinter );
   if (debug)
       QTextStream(stdout) << "Failed to send job to printer\n";
   return false;
 }
 if( ! EndPagePrinter( hPrinter ) )
 {
   EndDocPrinter( hPrinter );
   ClosePrinter( hPrinter );
   if (debug)
       QTextStream(stdout) << "Failed to end page\n";
   return false;
 }
 if( ! EndDocPrinter( hPrinter ) )
 {
   ClosePrinter( hPrinter );
   if (debug)
       QTextStream(stdout) << "Failed to end document\n";
   return false;
 }
 ClosePrinter( hPrinter );
 if( dwBytesWritten != dwCount ) {
     if (debug)
         QTextStream(stdout) << "Unknown error\n";
     return false;
 }
 return true;
}

bool loadRaw(bool debug, QString path, QString printer) {
    QFile file(path);

    QTextStream(stdout) << "Loading file: " << path << "\n";

    if(!file.open(QIODevice::ReadOnly)) {
        QTextStream(stderr) << file.errorString() << "\n";
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QString data;

    while(!in.atEnd())
        data.append(in.readLine());

    file.close();

    if (debug)
        QTextStream(stdout) << "File loaded\n";

    wchar_t * pname = new wchar_t[static_cast<unsigned int>(data.length() + 1)];;

    printer.toWCharArray(pname);
    pname[printer.length()] = '\0';

    if (debug)
        QTextStream(stdout) << "Converted printer name\n";

    QByteArray ba = data.toLocal8Bit();
    const char *c_str = ba.data();

    if (debug)
        QTextStream(stdout) << "Loaded data: " << c_str << "\nOf lenght: " << ba.length() << "\n";

    QTextStream(stdout) << "Sending file to printer " << printer << "\n";

    bool ok = RawDataToPrinter(pname, const_cast<LPBYTE>(reinterpret_cast<const unsigned char *>(c_str)), static_cast<DWORD>(ba.length()), debug);

    if (ok)
        QTextStream(stdout) << "File sent\n";
    else
        QTextStream(stdout) << "File failed to send\n";

    delete [] pname;
    return ok;
}

#endif // RAWPRINT_H
