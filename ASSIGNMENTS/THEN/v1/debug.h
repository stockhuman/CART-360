/**
 * Logs a string to Serial - many ovverides
 * Michael Hemingway
 * v 1.0
 * @param s  string (most of the time)
 */

#include "Ansiterm.h"
Ansiterm ansi;

class Console {
  public:
    Console();
    void clear();
    void home();
    void log(const char *buffer);
    void error(const char *buffer);
    void info(const char *buffer);
    void warn(const char *buffer);
};


Console::Console () {}

void Console::clear () { ansi.eraseScreen(); };

void Console::home () { ansi.home(); }

void Console::log (const char *buffer) {
  Serial.write(buffer);
  Serial.write('\n');
};

void Console::error (const char *buffer) {
  ansi.setForegroundColor(ANSI_RED);
  Serial.write("[Error] ");
  Serial.write(buffer);
  Serial.write('\n');
  ansi.setForegroundColor(ANSI_WHITE);
}

void  Console::info (const char *buffer) {
  ansi.setForegroundColor(ANSI_BLUE);
  Serial.write("[Info] ");
  Serial.write(buffer);
  Serial.write('\n');
  ansi.setForegroundColor(ANSI_WHITE);
}

void  Console::warn (const char *buffer) {
  ansi.setForegroundColor(ANSI_YELLOW);
  Serial.write("[Warn] ");
  Serial.write(buffer);
  Serial.write('\n');
  ansi.setForegroundColor(ANSI_WHITE);
}
