/*
Клиент социальной игры десктопная версия
copyright (c) 2013 Евгений Лежнин
  */
#include <QtGui/QApplication>
#include <QTranslator>
#include "lobby.h"
#include <QLocale>
#include <QTextCodec>
using namespace livesheap;

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setOrganizationName( "perdukomzxc" );
    app.setApplicationName( "livesheap" );

    //поддержка не-ascii символов
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

    //выбор языка / language selection
    QTranslator translator;
    //для перевода на русский
    translator.load("ru");
    app.installTranslator(&translator);

    Lobby lobby;
    return app.exec();
}
