/*
 * main.cpp - RIFF tree structure viewer
 *
 *   Copyright 2013 Jesús Torres <jmtorres@ull.es>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define USE_MMAP

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <getopt.h>

#include "riff.h"

#define OUTPUT_IDENT_WDITH      4

//
// Alias for standard output and standard error based on QTextStream.
// We will use them instead of std::cout and std::cerr for convenience.
//

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

//
// Print help message to standard error
//

void printHelp(const QString& programName)
{
    cerr << QString("Usage: %1 [options] FILE\n").arg(programName);
    cerr << "Parses a RIFF file and show its tree structure.\n";
    cerr << endl;
    cerr << "  -h, --help     display this help and exit\n";
    cerr << endl;
}

//
// Traverse a RIFF tree from specified chunk and print the structure
//

void traverseRiff(const riff::RiffList<>::Chunk* listChunk, int indentWidth)
{
    cout << QString(indentWidth, ' ')
         << QString("%1(%2) ->\n").arg(listChunk->typeToQString())
                                  .arg(listChunk->data->listTypeToQString());

    QString childIndent(indentWidth + OUTPUT_IDENT_WDITH, ' ');

    const riff::RiffChunk<>* child = listChunk->data->chunks;
    const void* end = listChunk->dataEnd();
    while (child < end) {
        if (child->hasTypeList())
            traverseRiff(child->castTo<riff::RiffList<> >(),
                         indentWidth + OUTPUT_IDENT_WDITH);
        else
            cout << childIndent << child->typeToQString() << endl;

        child = child->nextChunk();
    }
}

//
// Main function
//

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString programName = QFileInfo(
                QCoreApplication::applicationFilePath()).fileName();

    // Retrieve command line arguments from Qt and parse options
    int appArgC = a.arguments().size();
    char** appArgV = new char*[appArgC];
    for (int i = 0; i < appArgC; ++i)
        appArgV[i] = a.arguments()[i].toLocal8Bit().data();

    const char shortOptions[] = "h";
    option longOptions[] = {
        {"help", 0, NULL, 'h'}
    };

    opterr = 0;             // disable getopt error messages

    int lastOption;
    while ((lastOption = getopt_long(appArgC, appArgV,
                                     shortOptions, longOptions, NULL)) != -1)
    {
        switch (lastOption) {
        case '?':
            cerr << QString("%1: invalid option -- '%2'\n")
                    .arg(programName)
                    .arg(static_cast<char>(optopt));
            cerr << endl;
            printHelp(programName);
            return 1;

        case 'h':
            printHelp(programName);
            return 0;

        default:
            abort();
        }
    }

    if (optind == appArgC) {
        printHelp(programName);
        return 99;
    }

    // Map the RIFF file into memory
    QFile rifffile(appArgV[optind]);
    if (! rifffile.open(QIODevice::ReadOnly)) {
        cerr << QString("%1: %2\n").arg(programName).arg(rifffile.errorString());
        return 10;
    }

    // QFile::map doesn't allow options like MAP_HUGETLB, MAP_PRIVATE or MAP_LOCKED
    // but it is nore portable between different operating systems than mmap().
    // Previously, we tried to use MAP_HUGETLB with mmap() syscall but it is only
    // valid for anonymous memory.
    uchar* buffer = rifffile.map(0, rifffile.size());
    if(buffer == NULL) {
        cerr << QString("%1: %2\n")
                .arg(programName)
                .arg(rifffile.errorString());
        return 11;
    }

    riff::RiffChunk<>* chunk = reinterpret_cast<riff::RiffChunk<>*>(buffer);
    if (! chunk->hasTypeRiff()) {
        cerr << QString("%1: '%2' is not a valid RIFF file\n")
                .arg(programName)
                .arg(rifffile.fileName());
        return 20;
    }

    // Traverse the RIFF file and print its structure
    traverseRiff(chunk->castTo<riff::RiffList<> >(), 0);

//    return a.exec();
}
